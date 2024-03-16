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

#ifndef EKG_GPU_BASE_IMPL_H
#define EKG_GPU_BASE_IMPL_H

#include <iostream>
#include <cstdint>
#include <string_view>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "ekg/util/geometry.hpp"
#include "ekg/draw/typography.hpp"

#define ekg_ok 0
#define ekg_failed 1

namespace ekg {
  enum class api {
    vulkan, opengl
  };
  
  namespace gpu {
    struct sampler_info {
    public:
      const char *p_tag {};
      int32_t offset[2] {};
      int32_t w {};
      int32_t h {};
      int32_t gl_parameter_filter[2] {};
      int32_t gl_wrap_modes[2] {};
      int32_t gl_internal_format {};
      uint32_t gl_format {};
      uint32_t gl_type {};
      bool gl_unpack_alignment {};
      void *p_data {};
    };

    typedef sampler_info sampler_allocate_info;
    typedef sampler_info sampler_fill_info;

    struct sampler_t {
    public:
      const char *p_tag {};
      uint32_t w {};
      uint32_t h {};
      uint32_t channel {};
      uint32_t gl_id {};
      int8_t gl_protected_active_index {-1};
    };

    struct data_t {
    public:
      float buffer_content[12] {};
      int32_t sampler_index {};
      int8_t line_thickness {};
      int32_t begin_stride {};
      int32_t end_stride {};
      int32_t factor {};
      int32_t scissor_id {-1};
    };

    class api {
    protected:
      std::string_view rendering_shader_fragment_source {};
    public:
      static float projection[16];
      static float viewport[4];
    public:
      void set_rendering_shader_fragment_source(std::string_view source);
    public:
      virtual void log_vendor_details() {};
      virtual void init() {};
      virtual void quit() {};
      virtual void invoke_pipeline() {};
      virtual void revoke_pipeline() {};
      virtual void update_viewport(int32_t w, int32_t h) {};
      virtual void re_alloc_geometry_resources(const float *p_data, uint64_t size) {};

      virtual void draw(
        ekg::gpu::data_t *p_gpu_data,
        uint64_t loaded_gpu_data_size
      ) {};

      virtual uint64_t allocate_sampler(
        const ekg::gpu::sampler_allocate_info *p_sampler_allocate_info,
        ekg::gpu::sampler_t *p_sampler
      )  { return ekg_ok; }

      virtual uint64_t fill_sampler(
        const ekg::gpu::sampler_fill_info *p_sampler_fill_info,
        ekg::gpu::sampler_t *p_sampler
      ) { return ekg_ok; };

      /**
       */
      virtual uint64_t generate_font_atlas(
        ekg::gpu::sampler_t *p_sampler,
        FT_Face &font_face,
        int32_t w,
        int32_t h,
        ekg::draw::glyph_char_t *p_glyph_char_data
      ) { return ekg_ok; };

      virtual uint64_t bind_sampler(ekg::gpu::sampler_t *p_sampler) { return ekg_ok; };
    };
  }

  void allocate_sampler(
    const ekg::gpu::sampler_allocate_info *p_sampler_allocate_info,
    ekg::gpu::sampler_t *p_sampler
  );

  void fill_sampler(
    const ekg::gpu::sampler_fill_info *p_sampler_fill_info,
    ekg::gpu::sampler_t *p_sampler
  );
}

#endif