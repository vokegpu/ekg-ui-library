/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, use in any thing (hacking, cheating, games, softwares).
 * 5- All malwares, rat and others virus. We do not care.
 * 6- Do not modify this license under any circunstancie.
 *
 * @VokeGpu 2022 all rights reserved.
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
            ekg::char_data allocated_char_data[128] {};

            float get_text_width(std::string_view text);
            float get_text_height();

            void set_font(const std::string &path);
            void set_size(uint8_t size);
            void reload();
            void bind_allocator(ekg::gpu::allocator* gpu_allocator);
            void blit(std::string_view text, float x, float y, const ekg::vec4 &color);

            void init();
            void quit();
        };
    }
}

#endif