#ifndef EKG_DRAW_FONT_H
#define EKG_DRAW_FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include "ekg/gpu/allocator.hpp"
#include "ekg/util/geometry.hpp"

namespace ekg {
    struct char_data {
        float x {};
        float texture_x {};

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
            uint8_t font_size {18};
            GLuint texture {};

            float full_width {};
            float full_height {};

            float offset_text_height {};
            float text_height {};

            bool flag_unloaded {};
            bool flag_first_time {true};

            ekg::gpu::allocator* allocator {};
            ekg::char_data allocated_char_data[128];

            float get_text_width(const std::string &text);
            float get_text_height();

            void set_font(const std::string &path);
            void set_size(uint8_t size);
            void reload();
            void bind_allocator(ekg::gpu::allocator* gpu_allocator);
            void blit(const std::string &text, float x, float y, const ekg::vec4 &color);

            void init();
            void quit();
        };
    }
}

#endif