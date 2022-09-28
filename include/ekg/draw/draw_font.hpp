#pragma once
#ifndef EKG_DRAW_FONT_H
#define EKG_DRAW_FONT_H

#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "ekg/gpu/gpu_allocator.hpp"
#include "ekg/util/util.hpp"

namespace ekg::draw {
    class font_renderer {
    public:
        static FT_Library ft_library;

        FT_Face ft_face {};
        FT_GlyphSlot ft_glyph_slot {};
        FT_Bool ft_bool_kerning {};
        FT_UInt ft_uint_previous {};
        FT_Vector_ ft_vector_previous_char {};

        std::string font_path {};
        uint8_t font_size {18};

        float full_width {};
        float full_height {};

        GLuint texture {};
        ekg::gpu::allocator* allocator {};

        float get_text_width(const std::string &text);
        float get_text_height();

        void set_size(uint8_t size);
        void reload();
        void bind_allocator(ekg::gpu::allocator* gpu_allocator);
        void blit(const std::string &text, float x, float y, const ekg::vec4 &color);

        void init();
        void quit();
    };
}

#endif