/*
 * MIT License
 * 
 * Copyright (c) 2022-2023 Rina Wilk / vokegpu@gmail.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "ekg/gpu/allocator.hpp"
#include "ekg/ekg.hpp"
#include "ekg/gpu/gl.hpp"
#include "ekg/os/ekg_opengl.hpp"

ekg::gpu::program ekg::gpu::allocator::program {};
float             ekg::gpu::allocator::mat4x4orthographic[16] {};
float             ekg::gpu::allocator::viewport[4] {};
bool              ekg::gpu::allocator::is_out_of_scissor {};

void ekg::gpu::allocator::invoke() {
  /*
   * Invocation segment reset the CPU-batching counters.
   */
  this->data_instance_index = 0;
  this->begin_stride_count = 0;
  this->end_stride_count = 0;
  this->simple_shape_index = 0;
  this->cached_geometry_index = 0;

  /*
   * The first 4 vertices are used by the simple shapes,
   * soon it is necessary to add not even one simple shape indices
   * but everything.
   */
  this->push_back_geometry(0.0f, 0.0f, 0.0f, 0.0f);
  this->push_back_geometry(0.0f, 1.0f, 0.0f, 1.0f);
  this->push_back_geometry(1.0f, 0.0f, 1.0f, 0.0f);
  this->push_back_geometry(1.0f, 1.0f, 1.0f, 1.0f);

  /* unique shape data will break if not clear the first index. */

  this->clear_current_data();
  this->bind_current_data().begin_stride = this->end_stride_count;
  this->begin_stride_count += this->end_stride_count;
  this->end_stride_count = 0;
}

void ekg::gpu::allocator::bind_texture(uint32_t texture) {
  bool should_alloc_new_texture {true};
  uint8_t texture_slot {};

  /* repeating textures increase the active textures, for this reason allocator prevent "dupes" */

  for (std::size_t it = 0; it < this->cached_textures.size(); it++) {
    auto &textures = this->cached_textures.at(it);
    should_alloc_new_texture = textures != texture;

    if (!should_alloc_new_texture) {
      texture_slot = (uint8_t) it;
      break;
    }
  }

  if (should_alloc_new_texture) {
    this->cached_textures.push_back(texture);
    texture_slot = (uint8_t) this->cached_textures.size() - 1;
  }

  auto &data {this->bind_current_data()};
  data.active_tex_slot = texture_slot + 1;
}

void ekg::gpu::allocator::dispatch() {
  auto &data {this->bind_current_data()};

  /* if this data contains a simple rect shape scheme, save this index and reuse later */

  this->simple_shape = static_cast<int32_t>(data.shape_rect[2]) != ekg::concave &&
                       static_cast<int32_t>(data.shape_rect[3]) != ekg::concave;
  if (this->simple_shape) {
    data.begin_stride = this->simple_shape_index;
    data.end_stride = 4; // simple shape contains 4 vertices.
    this->end_stride_count = 0;
  } else {
    data.begin_stride = this->begin_stride_count;
    data.end_stride = this->end_stride_count;
  }

  data.scissor_id = this->scissor_instance_id;
  data.id = static_cast<int32_t>(this->data_instance_index);

  /* flag re alloc buffers if factor changed */

  if (!this->factor_changed) {
    this->factor_changed = this->previous_factor != data.factor;
  }

  this->begin_stride_count += this->end_stride_count;
  this->end_stride_count = 0;

  this->data_instance_index++;
  this->clear_current_data();
}

void ekg::gpu::allocator::revoke() {
  uint64_t cached_geometry_resources_size {this->cached_geometry_index};
  bool should_re_alloc_buffers {this->previous_cached_geometry_resources_size != cached_geometry_resources_size};

  if (this->data_instance_index < this->data_list.size()) {
    this->data_list.erase(this->data_list.begin() + this->data_instance_index + 1, this->data_list.end());
  }

  if (should_re_alloc_buffers || this->factor_changed) {
    glBindVertexArray(this->vbo_array);
    glBindBuffer(GL_ARRAY_BUFFER, this->geometry_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cached_geometry_resources_size,
                 this->cached_geometry_resources.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
  }

  this->factor_changed = false;

  if (this->cached_geometry_resources.size() < this->previous_cached_geometry_resources_size) {
    this->cached_geometry_resources.erase(
      this->cached_geometry_resources.begin() + cached_geometry_resources_size + 1,
      this->cached_geometry_resources.end()
    );
  }

  this->previous_cached_geometry_resources_size = cached_geometry_resources_size;
}

void ekg::gpu::allocator::on_update() {
}

void ekg::gpu::allocator::draw() {
  ekg::gpu::invoke(ekg::gpu::allocator::program);

  glBindVertexArray(this->vbo_array);
  glBindTexture(GL_TEXTURE_2D, 0);

  uint8_t prev_texture_bound {};
  bool texture_enabled {};

  /*
   * Before each rendering section, the allocator iterate alls textures and bind it on global context.
   */
  for (uint32_t it {}; it < this->cached_textures.size(); it++) {
    glActiveTexture(GL_TEXTURE0 + static_cast<int32_t>(it));
    glBindTexture(GL_TEXTURE_2D, this->cached_textures.at(it));
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /*
   * The allocator provides a high-performance dynamic mapped-shapes batching,
   * each simple rect (not a complex shape) is not batched, but complex shapes
   * like textured shapes - font rendering - is batched.
   */

  for (uint64_t it {}; it < this->data_instance_index; it++) {
    ekg::gpu::data &data {this->data_list.at(it)};
    texture_enabled = data.active_tex_slot > 0;

    if (texture_enabled && prev_texture_bound != data.active_tex_slot) {
      glUniform1i(this->uniform_active_tex_slot, data.active_tex_slot - 1);
      glUniform1i(this->uniform_active_texture, true);
      prev_texture_bound = data.active_tex_slot;
    } else if (!texture_enabled && prev_texture_bound > 0) {
      glUniform1i(this->uniform_active_texture, false);
      prev_texture_bound = 0;
    }

    glUniform4fv(this->uniform_color, GL_TRUE, data.material_color);
    glUniform4fv(this->uniform_rect, GL_TRUE, data.shape_rect);
    glUniform1i(this->uniform_line_thickness, data.line_thickness);
    glUniform4fv(this->uniform_scissor, GL_TRUE, this->scissor_map[data.scissor_id].rect);

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

  glUniform1i(this->uniform_active_texture, false);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_BLEND);

  glBindVertexArray(0);
  glUseProgram(0);
}

void ekg::gpu::allocator::init() {
  const std::string vsh_src {
      ekg::glsl_version + "\n"
                          "layout (location = 0) in vec2 aPos;\n"
                          "layout (location = 1) in vec2 aTexCoord;\n"

                          "#define CONVEX -256.0\n"

                          "uniform mat4 uOrthographicMatrix;\n"
                          "uniform vec4 uRect;\n"

                          "out vec2 vTexCoord;\n"
                          "out vec4 vRect;\n"

                          "void main() {"
                          "    vec2 vertex = aPos;\n"
                          "    bool modalShape = uRect.z != CONVEX || uRect.w != CONVEX;\n"

                          "    if (modalShape) {"
                          "        vertex *= uRect.zw;"
                          "    }\n"

                          "    vertex += uRect.xy;\n"
                          "    gl_Position = uOrthographicMatrix * vec4(vertex, 0.0f, 1.0f);\n"

                          "    vTexCoord = aTexCoord;\n"
                          "    vRect = uRect;\n"
                          "}"};

  const std::string fsh_src {
      ekg::glsl_version + "\n"
                          "layout (location = 0) out vec4 vFragColor;\n"
                          "uniform sampler2D uTextureSampler;\n"

                          "in vec2 vTexCoord;\n"
                          "in vec4 vRect;\n"

                          "uniform int uLineThickness;\n"
                          "uniform bool uActiveTexture;\n"
                          "uniform vec4 uColor;\n"
                          "uniform vec4 uScissor;\n"
                          "uniform float uViewportHeight;\n"

                          "void main() {"
                          "    vFragColor = uColor;\n"
                          "    vec2 fragPos = vec2(gl_FragCoord.x, uViewportHeight - gl_FragCoord.y);\n"
                          "    bool shouldDiscard = (fragPos.x <= uScissor.x || fragPos.y <= uScissor.y || fragPos.x >= uScissor.x + uScissor.z || fragPos.y >= uScissor.y + uScissor.w);\n"

                          "    float lineThicknessf = float(uLineThickness);\n"
                          "    if (uLineThickness > 0) {"
                          "        vec4 outline = vec4(vRect.x + lineThicknessf, vRect.y + lineThicknessf, vRect.z - (lineThicknessf * 2.0f), vRect.w - (lineThicknessf * 2.0f));\n"
                          "        shouldDiscard = shouldDiscard || (fragPos.x > outline.x && fragPos.x < outline.x + outline.z && fragPos.y > outline.y && fragPos.y < outline.y + outline.w);\n"
                          "    } else if (uLineThickness < 0) {\n"
                          "       float radius = vRect.z / 2.0f;\n"
                          "       vec2 diff = vec2((vRect.x + radius) - fragPos.x, (vRect.y + radius) - fragPos.y);\n"
                          "       float dist = (diff.x * diff.x + diff.y * diff.y);\n"

                          "       vFragColor.w = (1.0f - smoothstep(0.0, radius * radius, dot(dist, dist)));\n"
                          "    }"

                          "    if (shouldDiscard) {"
                          "       vFragColor.w = 0.0f;\n"
                          "    }\n"

                          "    if (uActiveTexture && !shouldDiscard) {"
                          "        vFragColor = texture(uTextureSampler, vTexCoord);\n"
                          "        vFragColor = vec4(vFragColor.xyz - ((1.0f - uColor.xyz) - 1.0f), vFragColor.w - (1.0f - uColor.w));\n"
                          "    }\n"
                          "}"};

  ekg::log() << "Loading internal shaders...";

  /* Create main shading program using two basic shaders (vertex & fragment). */
  ekg::gpu::create_basic_program(ekg::gpu::allocator::program, {
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
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void *) 0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void *) (sizeof(float) * 2));
  /* End of geometry resources buffer attributes. */

  /* Start of simple shape indexing buffer bind to VAO. */
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo_simple_shape);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(simple_shape_mesh_indices), simple_shape_mesh_indices, GL_STATIC_DRAW);
  glBindVertexArray(0);
  /* End  of simple shape indexing buffer bind to VAO. */

  /* reduce glGetLocation calls when rendering the batch */
  auto &shading_program_id {ekg::gpu::allocator::program.id};
  this->uniform_active_texture = glGetUniformLocation(shading_program_id, "uActiveTexture");
  this->uniform_active_tex_slot = glGetUniformLocation(shading_program_id, "uTextureSampler");
  this->uniform_color = glGetUniformLocation(shading_program_id, "uColor");
  this->uniform_rect = glGetUniformLocation(shading_program_id, "uRect");
  this->uniform_line_thickness = glGetUniformLocation(shading_program_id, "uLineThickness");
  this->uniform_scissor = glGetUniformLocation(shading_program_id, "uScissor");

  ekg::log() << "GPU allocator initialised";
}

void ekg::gpu::allocator::clear_current_data() {
  /* allocator handle automatically the size of data */

  if (this->data_instance_index >= this->data_list.size()) {
    this->data_list.emplace_back();
  }

  ekg::gpu::data &data {this->bind_current_data()};
  data.line_thickness = 0;
  data.active_tex_slot = 0;
  data.scissor_id = -1;

  this->previous_factor = data.factor;
}

ekg::gpu::data &ekg::gpu::allocator::bind_current_data() {
  return this->data_list.at(this->data_instance_index);
}

uint32_t ekg::gpu::allocator::get_current_data_id() {
  return this->data_instance_index;
}

ekg::gpu::data *ekg::gpu::allocator::get_data_by_id(int32_t id) {
  if (id < 0 || id > this->data_instance_index) {
    return nullptr;
  }

  return &this->data_list[id];
}

void ekg::gpu::allocator::quit() {
  glDeleteTextures((int32_t) this->cached_textures.size(), &this->cached_textures[0]);
  glDeleteBuffers(1, &this->vbo_array);
  glDeleteVertexArrays(1, &this->vbo_array);
}

ekg::gpu::scissor *ekg::gpu::allocator::get_scissor_by_id(int32_t id) {
  return &this->scissor_map[id];
}

void ekg::gpu::allocator::erase_scissor_by_id(int32_t id) {
  if (this->scissor_map.count(id)) {
    this->scissor_map.erase(id);
  }
}

uint32_t ekg::gpu::allocator::get_instance_scissor_id() {
  return this->scissor_instance_id;
}

void ekg::gpu::allocator::sync_scissor(ekg::rect &rect_child, int32_t mother_parent_id) {
  auto &scissor {this->scissor_map[this->scissor_instance_id]};

  scissor.rect[0] = rect_child.x;
  scissor.rect[1] = rect_child.y;
  scissor.rect[2] = rect_child.w;
  scissor.rect[3] = rect_child.h;

  ekg::gpu::allocator::is_out_of_scissor = false;

  /*
   * The EKG rendering clipping/scissor part solution is actually very simple,
   * each draws section contains an ID, which map for a scissor data,
   * when batching is going on, the scissor is automatically fixed together.
   */
  if (mother_parent_id == 0) {
    return;
  }

  auto &mother_rect {this->scissor_map[mother_parent_id]};

  if (scissor.rect[0] < mother_rect.rect[0]) {
    scissor.rect[2] -= mother_rect.rect[0] - scissor.rect[0];
    scissor.rect[0] = mother_rect.rect[0];
  }

  if (scissor.rect[1] < mother_rect.rect[1]) {
    scissor.rect[3] -= mother_rect.rect[1] - scissor.rect[1];
    scissor.rect[1] = mother_rect.rect[1];
  }

  if (scissor.rect[0] + scissor.rect[2] > mother_rect.rect[0] + mother_rect.rect[2]) {
    scissor.rect[2] -= (scissor.rect[0] + scissor.rect[2]) - (mother_rect.rect[0] + mother_rect.rect[2]);
  }

  if (scissor.rect[1] + scissor.rect[3] > mother_rect.rect[1] + mother_rect.rect[3]) {
    scissor.rect[3] -= (scissor.rect[1] + scissor.rect[3]) - (mother_rect.rect[1] + mother_rect.rect[3]);
  }

  ekg::gpu::allocator::is_out_of_scissor = !(scissor.rect[0] < mother_rect.rect[0] + mother_rect.rect[2] &&
                                             scissor.rect[0] + scissor.rect[2] > mother_rect.rect[0] &&
                                             scissor.rect[1] < mother_rect.rect[1] + mother_rect.rect[3] &&
                                             scissor.rect[1] + scissor.rect[3] > mother_rect.rect[1]);
}

void ekg::gpu::allocator::bind_scissor(int32_t scissor_id) {
  this->scissor_instance_id = scissor_id;
}

void ekg::gpu::allocator::bind_off_scissor() {
  this->scissor_instance_id = -1;
}

void ekg::gpu::allocator::push_back_geometry(float x, float y, float u, float v) {
  this->end_stride_count++;

  if (this->cached_geometry_index >=
      this->cached_geometry_resources.size()) {

    this->cached_geometry_index += 4;

    this->cached_geometry_resources.emplace_back(x);
    this->cached_geometry_resources.emplace_back(y);

    this->cached_geometry_resources.emplace_back(u);
    this->cached_geometry_resources.emplace_back(v);

    return;
  }

  this->cached_geometry_resources.at(this->cached_geometry_index++) = x;
  this->cached_geometry_resources.at(this->cached_geometry_index++) = y;

  this->cached_geometry_resources.at(this->cached_geometry_index++) = u;
  this->cached_geometry_resources.at(this->cached_geometry_index++) = v;
}