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

#include "ekg/ekg.hpp"

void ekg::gpu::api::set_rendering_shader_fragment_source(std::string_view source) {
  this->rendering_shader_fragment_source = source;
}

void ekg::allocate_sampler(
  const ekg::gpu::sampler_allocate_info *p_sampler_allocate_info,
  ekg::gpu::sampler_t *p_sampler
) {
  ekg::core->p_gpu_api->allocate_sampler(p_sampler_allocate_info, p_sampler);
}

void ekg::fill_sampler(
  const ekg::gpu::sampler_fill_info *p_sampler_fill_info,
  ekg::gpu::sampler_t *p_sampler
) {
  ekg::core->p_gpu_api->fill_sampler(p_sampler_fill_info, p_sampler);
}