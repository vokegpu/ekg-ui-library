/**
 * MIT License
 * 
 * Copyright (c) 2022-2024 Rina Wilk / vokegpu@gmail.com
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

#ifndef EKG_OS_OPENGL_H
#define EKG_OS_OPENGL_H

#if defined(__ANDROID__)
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif

#include "ekg/gpu/api.hpp"

namespace ekg::os {
  class opengl : public ekg::gpu::api {
  public:
    uint32_t compile_shader(std::string_view shader_source, uint32_t shader_type);
  public:
    void init() override;
    void quit() override;
    void invoke_pipeline() override;
    void revoke_pipeline() override;
    void update_viewport(int32_t w, int32_t h) override;
    void re_alloc_rendering_geometry(const float *p_data, uint64_t size) override;
    void draw() override;

    uint64_t allocate_sampler(
      const ekg::gpu::sampler_allocate_info *p_sampler_allocate_info,
      ekg::gpu::sampler_t *p_sampler,
    ) override;

    uint64_t fill_sampler(
      const ekg::gpu::sampler_fill_info *sampler_fill_info,
      ekg::gpu::sampler_t *p_sampler,
    ) override;

    bool bind_sampler(ekg::gpu::sampler_t *p_sampler) override;
  }
}

#endif