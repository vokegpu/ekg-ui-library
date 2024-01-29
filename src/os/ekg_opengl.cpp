#include "ekg/os/ekg_opengl.hpp"

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
}

void ekg::os::opengl::draw(ekg::) {

}