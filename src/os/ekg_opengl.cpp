#include "ekg/os/ekg_opengl.hpp"
#include "ekg/gpu/api.hpp"
#include <stdio.h>

void ekg::os::opengl::init() {

}

void ekg::os::opengl::quit() {
  
}

void ekg::os::opengl::re_alloc_rendering_geometry(const float *p_data, uint64_t size) {

}

void ekg::os::opengl::bind_texture(uint32_t texture_id) {

}

uint64_t ekg::os::opengl::allocate_sampler(
  const ekg::gpu::sampler_allocate_info *p_sampler_allocate_info,
  ekg::gpu::sampler_t *p_sampler,
) {
  if (p_sampler == nullptr) {
    return 1;
  }

  if (!p_sampler->gl_id) {
    glGenTextures(1, &p_sampler->gl_id);
  }

  glBindTexture(
    GL_TEXTURE_2D,
    p_sampler->gl_id
  );

  p_sampler->w = p_sampler_allocate_info->w;
  p_sampler->h = p_sampler_allocate_info->h;

  if (p_sampler_allocate_info->gl_filter[0]) {
    glParamateri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, p_sampler_allocate_info->gl_filter[0]);
  }

  if (p_sampler_allocate_info->gl_filter[1]) {
    glParamateri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, p_sampler_allocate_info->gl_filter[1]);
  }

  if (p_sampler_allocate_info->gl_wrap_mode[0]) {
    glParamateri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, p_sampler_allocate_info->gl_wrap_mode[0]);
  }

  if (p_sampler_allocate_info->gl_wrap_mode[1]) {
    glParamateri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, p_sampler_allocate_info->gl_wrap_mode[1]);
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
}

uint64_t ekg::os::opengl::fill_sampler(
  const ekg::gpu::sampler_fill_info *p_sampler_fill_info,
  ekg::gpu::sampler_t *p_sampler,
) {
  if (p_sampler == nullptr || !p_sampler->gl_id) {
    return 1;
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
    p_sampler_fill_info->p_data,
  );

  glBindTexture(GL_TEXTURE_2D, 0);
}

uint64_t ekg::so::opengl::bind_sampler(ekg::gpu::sampler_t *p_sampler) {
  uint64_t size {this->bound_sampler_list->size()};
  for (uint64_t it {}; it < this->bound_sampler_list.size(); it++) {
    ekg::gpu::sampler_t *p_in_list_sampler {this->bound_sampler_list.at(it)};
    if (p_in_list_sampler->gl_id == p_sampler->gl_id) {
      return it;
    }
  }

  /**
   * Protected sampleres such as font, icons, and default EKG samplers,
   * should not overhead the glBindTexture calls,
   * this reason the active index only increase
   * when it is a protected sampler.
   **/
  if (ekg_is_sampler_protected(p_sampler->gl_protected_active_index)) {
    glActiveTexture(
      GL_TEXTURE0 + this->protected_texture_active_index
    );

    p_sampler->gl_protected_active_index = this->protected_texture_active_index++;
    glBindTexture(GL_TEXTURE_2D, p_sampler->gl_id);
  }

  this->bound_sampler_list.emplace_back() = p_sampler;
  return size;
}

void ekg::os::opengl::draw(
  const ekg::gpu::data_t *p_gpu_data,
  uint64_t loaded_gpu_data_size,
  const ekg::gpu::sampler_t *p_sampler_data,
  uint64_t loaded_sampler_data_size,
) {
  glUseProgram(this->gl_program);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glActiveTexture(
    GL_TEXTURE0 + this->protected_texture_active_index
  );

  int32_t previous_sampler_bound {};
  bool sampler_going_on {};

  for (uint64_t it {}; it < loaded_gpu_data_size; it++) {
    ekg::gpu::data_t &data {p_gpu_data[it]};]
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
        case false;
          glUniform1i(this->uniform_active_tex_slot, this->protected_texture_active_index);
          glBindTexture(GL_TEXTURE_2D, p_sampler->gl_id);
          break;
      }

      previous_sampler_bound = data.sampler_index;
    } else if (!samper_going_on && previous_sampler_bound > -1) {
      glUniform1i(this->uniform_active_texture, false);
      previous_sampler_bound = -1;
    }

    glUniform4fv(this->uniform_color, GL_TRUE, &data.buffer_content[4]);
    glUniform4fv(this->uniform_rect, GL_TRUE, data.buffer_content);
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

  glUseProgram(0);
}