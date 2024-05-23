#include "ekg/os/ekg_opengl.hpp"
#include "ekg/ekg.hpp"
#include "ekg/gpu/api.hpp"
#include <cstdio>
#include <unordered_map>

ekg::os::opengl::opengl(std::string_view set_glsl_version) {
  this->glsl_version = "#version 450";

  if (set_glsl_version.empty()) {
    ekg::log() << "[GPU][API] not viable glsl version, empty, must 330 higher";
    return;
  }

  uint64_t size {set_glsl_version.size() - 1};
  uint8_t number_size {};

  for (uint64_t it {}; it < set_glsl_version.size(); it++) {
    auto &a_char {set_glsl_version.at((size) - it)};
    if (a_char >= 48 && a_char <= 58) {
      number_size++;
    } else {
      break;
    }
  }

  if (number_size != 3) {
    ekg::log() << "[GPU][API] not viable glsl version, unknown number, must `#version 330` higher";
    return;
  }

  size += 1;
  std::string version_metadata {set_glsl_version.substr(size - number_size, number_size)};
  int32_t version {std::stoi(version_metadata)};

  if (version < 330) {
    ekg::log() << "[GPU][API] not viable glsl version, incorrect number, must `#version 330` higher";
    return;
  }

  this->glsl_version = set_glsl_version;
}

void ekg::os::opengl::log_vendor_details() {
  ekg::log() << glGetString(GL_VENDOR);
}

void ekg::os::opengl::init() {
  std::string no_view_glsl_version {
    this->glsl_version
  };

  std::string vsh_src {
    no_view_glsl_version + "\n"
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"

    "uniform mat4 uProjection;\n"
    "uniform vec4 uRect;\n"

    "out vec2 vTexCoord;\n"
    "out vec4 vRect;\n"

    "void main() {\n"
      "vec2 vertex = aPos;\n"

      "if (uRect.z > -1.0f && uRect.w > -1.0f) {"
        "vertex *= uRect.zw;\n"
      "}\n"

      "vertex += uRect.xy;\n"

      "gl_Position = uProjection * vec4(vertex, 0.0f, 1.0f);\n"
      "vTexCoord = aTexCoord;\n"
      "vRect = uRect;\n"
    "}\n"
  };

  std::string fsh_src {
    no_view_glsl_version + "\n"
    "layout (location = 0) out vec4 vFragColor;\n"
    "uniform sampler2D uTextureSampler;\n"

    "in vec2 vTexCoord;\n"
    "in vec4 vRect;\n"

    "uniform int uLineThickness;\n"
    "uniform bool uActiveTexture;\n"
    "uniform float uViewportHeight;\n"
    "uniform float uContent[8];\n"

    "void main() {\n"
      "vFragColor = vec4(\n"
        "uContent[0],\n"
        "uContent[1],\n"
        "uContent[2],\n"
        "uContent[3]\n"
      ");\n"

      "vec2 fragPos = vec2(gl_FragCoord.x, uViewportHeight - gl_FragCoord.y);\n"

      /**
       * The scissoring works like swapchain-one (does not stack), of course,
       * this scissor is a little different, the pixel-perfect precision makes
       * a better cut of fragments. And does not require any overhead from
       * calling command buffers to GPU rastarizer.
       **/
      "bool shouldDiscard = (\n"
        "fragPos.x <= uContent[4] ||\n"
        "fragPos.y <= uContent[5] ||\n"
        "fragPos.x >= uContent[4] + uContent[6] ||\n"
        "fragPos.y >= uContent[5] + uContent[7]\n"
      ");\n"

      "float lineThicknessf = float(uLineThickness);\n"

      /**
       * The pixel-perfect outline is possible on fragment shader,
       * due the precision of fragments position, and the
       * normalised-space.
       **/
      "if (uLineThickness > 0) {\n"
        "vec4 outline = vec4(\n"
          "vRect.x + lineThicknessf,\n"
          "vRect.y + lineThicknessf,\n"
          "vRect.z - (lineThicknessf * 2.0f),\n"
          "vRect.w - (lineThicknessf * 2.0f)\n"
        ");\n"

        "shouldDiscard = (\n"
          "shouldDiscard || (\n"
            "fragPos.x > outline.x &&\n"
            "fragPos.x < outline.x + outline.z &&\n"
            "fragPos.y > outline.y &&\n"
            "fragPos.y < outline.y + outline.w\n"
          ")\n"
        ");\n"
      "} else if (uLineThickness < 0) {\n"
        "float radius = vRect.z / 2.0f;\n"

        "vec2 diff = vec2(\n"
          "(vRect.x + radius) - fragPos.x,\n"
          "(vRect.y + radius) - fragPos.y\n"
        ");\n"

        "float dist = (diff.x * diff.x + diff.y * diff.y);\n"
        "vFragColor.w = (\n"
          "1.0f - smoothstep(0.0, radius * radius, dot(dist, dist))\n"
        ");\n"
      "}\n"

      /**
       * The discard must not call `discard` keyword,
       * discarding pixels using keyword is performanceless
       * comparated to alpha blending equals to zero.
       **/
      "if (shouldDiscard) {\n"
        "vFragColor.w = 0.0f;\n"
      "} else if (uActiveTexture) {\n"
        "vec4 textureColor = texture(uTextureSampler, vTexCoord);\n"

        /**
         * The formula I created is
         *
         * t(r, g, b, a) = Texture color
         * c(x, y, z, w) = Color to blend
         *
         * f(t, c) = (t(r, g, b) - ((1 - c(r, g, b) - 1), t(w) - (1 - c(w)))
         **/
        "vFragColor = vec4(\n"
          "textureColor.xyz - ((1.0f - vFragColor.xyz) - 1.0f),\n"
          "textureColor.w - (1.0f - vFragColor.w)\n"
        ");\n"
      "}\n"
    "}\n"
  };

  if (!this->rendering_shader_fragment_source.empty()) {
    fsh_src = this->rendering_shader_fragment_source;
  }

  ekg::log() << "Loading internal shaders...";

  /* Create main shading program using two basic shaders (vertex & fragment). */
  this->create_pipeline_program(this->pipeline_program, {
    {vsh_src, GL_VERTEX_SHADER},
    {fsh_src, GL_FRAGMENT_SHADER}
  });

  glGenVertexArrays(1, &this->vbo_array);
  glGenBuffers(1, &this->geometry_buffer);
  glGenBuffers(1, &this->ebo_simple_shape);

  /* Generate base shape rendering. */
  uint8_t simple_shape_mesh_indices[6] {
      0, 2, 3,
      3, 1, 0
  };

  glBindVertexArray(this->vbo_array);

  /* Start of geometry resources buffer attributes. */
  glBindBuffer(GL_ARRAY_BUFFER, this->geometry_buffer);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void *) (sizeof(float) * 2));
  /* End of geometry resources buffer attributes. */

  /* Start of simple shape indexing buffer bind to VAO. */
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo_simple_shape);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(simple_shape_mesh_indices), simple_shape_mesh_indices, GL_STATIC_DRAW);
  glBindVertexArray(0);
  /* End  of simple shape indexing buffer bind to VAO. */

  /* reduce glGetLocation calls when rendering the batch */
  this->uniform_active_texture = glGetUniformLocation(this->pipeline_program, "uActiveTexture");
  this->uniform_active_tex_slot = glGetUniformLocation(this->pipeline_program, "uTextureSampler");
  this->uniform_content = glGetUniformLocation(this->pipeline_program, "uContent");
  this->uniform_rect = glGetUniformLocation(this->pipeline_program, "uRect");
  this->uniform_line_thickness = glGetUniformLocation(this->pipeline_program, "uLineThickness");
  this->uniform_scissor = glGetUniformLocation(this->pipeline_program, "uScissor");
  this->uniform_viewport_height = glGetUniformLocation(this->pipeline_program, "uViewportHeight");
  this->uniform_projection = glGetUniformLocation(this->pipeline_program, "uProjection");

  ekg::log() << "GPU shaders, pipeline program, and uniforms done";
}

void ekg::os::opengl::quit() {

}

void ekg::os::opengl::pre_re_alloc() {
  this->protected_texture_active_index = 0;
}

void ekg::os::opengl::update_viewport(int32_t w, int32_t h) {
  ekg::gpu::api::viewport[0] = 0.0f;
  ekg::gpu::api::viewport[1] = 0.0f;
  ekg::gpu::api::viewport[2] = static_cast<float>(w);
  ekg::gpu::api::viewport[3] = static_cast<float>(h);

  ekg::ortho(
    ekg::gpu::api::projection,
    0,
    ekg::gpu::api::viewport[2],
    ekg::gpu::api::viewport[3],
    0
  );

  glUseProgram(this->pipeline_program);
  glUniformMatrix4fv(this->uniform_projection, GL_TRUE, 0, ekg::gpu::api::projection);
  glUniform1f(this->uniform_viewport_height, ekg::gpu::api::viewport[3]);
  glUseProgram(0);
}

bool ekg::os::opengl::create_pipeline_program(uint32_t &program, const std::unordered_map<std::string_view, uint32_t> &resources) {
  if (resources.empty()) {
    ekg::log() << "Error: Invalid shader, empty resources";
    return true;
  }

  std::string shader_src {};
  std::vector<uint32_t> compiled_shader_list {};
  int32_t status {};
  program = glCreateProgram();

  std::string msg {};
  for (auto &[value, key]: resources) {
    uint32_t shader {glCreateShader(key)};
    const char *p_src {value.data()};
    glShaderSource(shader, 1, &p_src, nullptr);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &status);
      msg.resize(status);
      glGetShaderInfoLog(shader, status, nullptr, msg.data());
      ekg::log() << "Error: Failed to compile shader '" << p_src  << "' stage: \n" << msg;
      break;
    }

    compiled_shader_list.push_back(shader);
  }

  bool keep {compiled_shader_list.size() == resources.size()};

  for (uint32_t &shaders: compiled_shader_list) {
    if (keep) {
      glAttachShader(program, shaders);
    }

    glDeleteShader(shaders);
  }

  if (keep) {
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if (status == GL_FALSE) {
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &status);
      msg.resize(status);
      glGetProgramInfoLog(program, status, nullptr, msg.data());
      ekg::log() << "Error: Failed to link program: \n" << msg;
    }
  }

  return false;
}

void ekg::os::opengl::re_alloc_geometry_resources(
  const float *p_data,
  uint64_t size
) {
  glBindVertexArray(this->vbo_array);

  glBindBuffer(GL_ARRAY_BUFFER, this->geometry_buffer);
  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(float)*size,
    p_data,
    GL_STATIC_DRAW
  );

  glBindVertexArray(0);
}

uint64_t ekg::os::opengl::allocate_sampler(
  const ekg::gpu::sampler_allocate_info *p_sampler_allocate_info,
  ekg::gpu::sampler_t *p_sampler
) {
  if (p_sampler == nullptr) {
    return ekg_failed;
  }

  if (!p_sampler->gl_id) {
    glGenTextures(1, &p_sampler->gl_id);
  }

  glBindTexture(
    GL_TEXTURE_2D,
    p_sampler->gl_id
  );

  if (p_sampler_allocate_info->gl_unpack_alignment) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  }

  p_sampler->w = p_sampler_allocate_info->w;
  p_sampler->h = p_sampler_allocate_info->h;

  if (p_sampler_allocate_info->gl_parameter_filter[0]) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, p_sampler_allocate_info->gl_parameter_filter[0]);
  }

  if (p_sampler_allocate_info->gl_parameter_filter[1]) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, p_sampler_allocate_info->gl_parameter_filter[1]);
  }

  if (p_sampler_allocate_info->gl_wrap_modes[0]) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, p_sampler_allocate_info->gl_wrap_modes[0]);
  }

  if (p_sampler_allocate_info->gl_wrap_modes[1]) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, p_sampler_allocate_info->gl_wrap_modes[1]);
  }

  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    p_sampler_allocate_info->gl_internal_format,
    p_sampler_allocate_info->w,
    p_sampler_allocate_info->h,
    0,
    p_sampler_allocate_info->gl_format,
    p_sampler_allocate_info->gl_type,
    p_sampler_allocate_info->p_data
  );

  glBindTexture(GL_TEXTURE_2D, 0);
  return ekg_ok;
}

uint64_t ekg::os::opengl::fill_sampler(
  const ekg::gpu::sampler_fill_info *p_sampler_fill_info,
  ekg::gpu::sampler_t *p_sampler
) {
  if (p_sampler == nullptr || !p_sampler->gl_id) {
    return ekg_failed;
  }

  glBindTexture(
    GL_TEXTURE_2D,
    p_sampler->gl_id
  );
  
  glTexSubImage2D(
    GL_TEXTURE_2D,
    0,
    p_sampler_fill_info->offset[0],
    p_sampler_fill_info->offset[1],
    p_sampler_fill_info->w,
    p_sampler_fill_info->h,
    p_sampler_fill_info->gl_format,
    p_sampler_fill_info->gl_type,
    p_sampler_fill_info->p_data
  );

  glBindTexture(GL_TEXTURE_2D, 0);
  return ekg_ok;
}

uint64_t ekg::os::opengl::generate_font_atlas(
  ekg::gpu::sampler_t *p_sampler,
  ekg::draw::font_face_t *p_font_face_text,
  ekg::draw::font_face_t *p_font_face_emoji,
  int32_t atlas_width,
  int32_t atlas_height,
  std::vector<char32_t> &loaded_sampler_generate_list,
  std::unordered_map<char32_t, ekg::draw::glyph_char_t> &mapped_glyph_char_data
) {
  int32_t internal_format {GL_RED};
#if defined(__ANDROID__)
  /*
   * Android does not support GL_RED, perharps because of the GL ES version.
   * For this reason, the format is GL_ALPHA and not GL_RED.
   * Also both of internal format, and format is the same.
   */
  internal_format = GL_ALPHA;
#endif
  if (!p_sampler->gl_id) {
    glGenTextures(1, &p_sampler->gl_id);
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glBindTexture(GL_TEXTURE_2D, p_sampler->gl_id);

  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    internal_format,
    atlas_width,
    atlas_height,
    0,
    internal_format,
    GL_UNSIGNED_BYTE,
    nullptr
  );

  FT_GlyphSlot ft_glyph_slot {};
  FT_Face ft_face {};
  FT_ULong c {};

  uint64_t flags {};
  float offset {};

  for (char32_t &char32 : loaded_sampler_generate_list) {
    flags = FT_LOAD_RENDER;

    switch (char32 < 256 || !p_font_face_emoji->font_face_loaded) {
      case true: {
        ft_face = p_font_face_text->ft_face;
        ft_glyph_slot = p_font_face_text->ft_face->glyph;
        break;
      }

      default: {
        ft_face = p_font_face_emoji->ft_face;
        ft_glyph_slot = p_font_face_emoji->ft_face->glyph;
        flags |= (FT_LOAD_COLOR | FT_LOAD_DEFAULT) * FT_HAS_COLOR(ft_face);

        break;
      }
    }

    if (FT_Load_Char(ft_face, char32, flags)) {
      continue;
    }

    ekg::draw::glyph_char_t &char_data {mapped_glyph_char_data[char32]};

    char_data.x = offset / static_cast<float>(atlas_width);
    char_data.w = static_cast<float>(ft_glyph_slot->bitmap.width);
    char_data.h = static_cast<float>(ft_glyph_slot->bitmap.rows);

    char_data.left = static_cast<float>(ft_glyph_slot->bitmap_left);
    char_data.top = static_cast<float>(ft_glyph_slot->bitmap_top);
    char_data.wsize = static_cast<float>(static_cast<int32_t>(ft_glyph_slot->advance.x >> 6));

    glTexSubImage2D(
      GL_TEXTURE_2D,
      0,
      static_cast<GLint>(offset),
      0,
      static_cast<GLsizei>(char_data.w),
      static_cast<GLsizei>(char_data.h),
      internal_format,
      GL_UNSIGNED_BYTE,
      ft_glyph_slot->bitmap.buffer
    );

    offset += char_data.w;
    char_data.was_sampled = true;
  }

#if defined(__ANDROID__)
  // GLES 3 does not support swizzle function, the format GL_ALPHA supply this issue.
#else
  GLint swizzle_format[] {GL_ZERO, GL_ZERO, GL_ZERO, GL_RED};

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle_format);
#endif

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  return ekg_ok;
}

uint64_t ekg::os::opengl::bind_sampler(ekg::gpu::sampler_t *p_sampler) {
  uint64_t size {this->bound_sampler_list.size()};
  for (uint64_t it {}; it < this->bound_sampler_list.size(); it++) {
    ekg::gpu::sampler_t *p_in_list_sampler {this->bound_sampler_list.at(it)};
    if (p_in_list_sampler->gl_id == p_sampler->gl_id) {
      return it;
    }
  }

  if (ekg_is_sampler_protected(p_sampler->gl_protected_active_index)) {
    p_sampler->gl_protected_active_index = this->protected_texture_active_index++;
  }

  this->bound_sampler_list.emplace_back() = p_sampler;
  return size;
}

void ekg::os::opengl::draw(
  ekg::gpu::data_t *p_gpu_data,
  uint64_t loaded_gpu_data_size
) {
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glUseProgram(this->pipeline_program);
  glBindVertexArray(this->vbo_array);

  /**
   * Protected samplers such as font, icons, and default EKG samplers,
   * should not overhead the glBindTexture calls,
   * this reason the active index only increase
   * when THIS is a protected sampler.
   **/
  for (ekg::gpu::sampler_t *&p_sampler : this->bound_sampler_list) {
    if (p_sampler->gl_protected_active_index == -1) continue;

    glActiveTexture(
      GL_TEXTURE0 + p_sampler->gl_protected_active_index
    );

    glBindTexture(GL_TEXTURE_2D, p_sampler->gl_id);
  }

  int32_t previous_sampler_bound {};
  bool sampler_going_on {};

  for (uint64_t it {}; it < loaded_gpu_data_size-1; it++) {
    ekg::gpu::data_t &data {p_gpu_data[it]};
    sampler_going_on = data.sampler_index > -1;

    if (sampler_going_on &&
        (
          previous_sampler_bound != data.sampler_index ||
          !ekg_is_sampler_protected(this->bound_sampler_list.at(data.sampler_index)->gl_protected_active_index)
        )
      ) {
        ekg::gpu::sampler_t *&p_sampler {
          this->bound_sampler_list.at(data.sampler_index)
        };

        switch (ekg_is_sampler_protected(p_sampler->gl_protected_active_index)) {
        case true:
          glUniform1i(this->uniform_active_tex_slot, p_sampler->gl_protected_active_index);
          glUniform1i(this->uniform_active_texture, true);
          break;
          case false:
          glUniform1i(this->uniform_active_tex_slot, this->protected_texture_active_index);
          glBindTexture(GL_TEXTURE_2D, p_sampler->gl_id);
          break;
      }

      previous_sampler_bound = data.sampler_index;
    } else if (!sampler_going_on && previous_sampler_bound > -1) {
      glUniform1i(this->uniform_active_texture, false);
      previous_sampler_bound = -1;
    }

    glUniform1i(this->uniform_line_thickness, data.line_thickness);
    glUniform4fv(this->uniform_rect, GL_TRUE, data.buffer_content);
    glUniform1fv(this->uniform_content, 8, &data.buffer_content[4]);

    switch (data.begin_stride) {
      case 0: {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
        break;
      }

      default: {
        glDrawArrays(GL_TRIANGLES, data.begin_stride, data.end_stride);
        break;
      }
    }
  }

  glDisable(GL_BLEND);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, 0);

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, 0);

  glUseProgram(0);
}