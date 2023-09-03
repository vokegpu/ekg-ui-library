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

#ifndef EKG_DRAW_FONT_H
#define EKG_DRAW_FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "ekg/gpu/allocator.hpp"
#include "ekg/util/geometry.hpp"

namespace ekg {
    struct char_data {
        float x {};
        float wsize {};
        float w {};
        float h {};
        float top {};
        float left {};
    };

    namespace draw {
        class font_renderer {
        public:
            static FT_Library ft_library;

            FT_Face ft_face {};
            FT_GlyphSlot ft_glyph_slot {};
            FT_Bool ft_bool_kerning {};
            FT_UInt ft_uint_previous {};
            FT_Vector_ ft_vector_previous_char {};

            std::string font_path {};
            uint32_t font_size {18};
            uint32_t texture {};

            float full_width {};
            float full_height {};

            float offset_text_height {};
            float text_height {};

            bool flag_unloaded {};
            bool flag_first_time {true};

            ekg::gpu::allocator *allocator {};
            ekg::char_data allocated_char_data[256] {};
        public:
            float get_text_width(std::string_view);
            float get_text_width(std::string_view, int32_t&);
            float get_text_height();

            void set_font(const std::string&);
            void set_size(uint32_t);
            void reload();
            void bind_allocator(ekg::gpu::allocator*);
            void blit(std::string_view, float, float, const ekg::vec4&);

            void init();
            void quit();
        };
    }
}

#endif