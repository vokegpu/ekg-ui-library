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

#ifndef EKG_DRAW_FONT_RENDERER_H
#define EKG_DRAW_FONT_RENDERER_H

#include <unordered_map>

#include "ekg/gpu/allocator.hpp"
#include "ekg/util/geometry.hpp"
#include "ekg/gpu/api.hpp"

#define FT_CONFIG_OPTION_USE_PNG

/**
 * EKG font-renderer may work ok to capture new glyph(s) on rendering,
 * but with some tests noticed zero wsize, which broken some widgets;
 * for solving it, easy validate the sample state and load once the char
 * when width is being calculated.
 * 
 * The point of making it a macro:
 * i wamt
 **/
#define ekg_validate_sample_state_and_get_wsize() \
if (!char_data.was_sampled) { \
  if (FT_Load_Char(ft_face, char32, FT_LOAD_RENDER | FT_LOAD_DEFAULT | FT_LOAD_COLOR)) { \
    continue; \
  } \
\
  char_data.wsize = static_cast<float>(static_cast<int32_t>(ft_glyph_slot->advance.x >> 6)); \
  this->loaded_sampler_generate_list.emplace_back(char32); \
  char_data.was_sampled = true; \
}

namespace ekg::draw {
  class font_renderer {
  public:
    static FT_Library ft_library;
  public:
    ekg::draw::font_face_t font_face_text {};
    ekg::draw::font_face_t font_face_emoji {};

    FT_Bool ft_bool_kerning {};
    FT_UInt ft_uint_previous {};

    std::string_view font_path {};
    uint32_t font_size {18};
    ekg::gpu::sampler_t sampler_texture {};

    int32_t full_width {};
    int32_t full_height {};

    float offset_text_height {};
    float text_height {};
    float non_swizzlable_range {};

    bool flag_unloaded {};
    bool flag_first_time {true};
    bool font_face_changed {};
    bool font_size_changed {};

    ekg::gpu::allocator *p_allocator {};
    std::unordered_map<char32_t, ekg::draw::glyph_char_t> mapped_glyph_char_data {};
    std::vector<char32_t> loaded_sampler_generate_list {};
    uint64_t last_sampler_generate_list_size {};
  public:
    /**
     * Return the sampler atlas font.
     */
    ekg::gpu::sampler_t *get_sampler_texture();

    /**
     * Return the text width.
     * Note: font-rendering is UTF-8-based.
     */
    float get_text_width(std::string_view text);

    /**
     * Return the text width and the lines `\n`.
     * Note: font-rendering is UTF-8-based.
     */
    float get_text_width(std::string_view text, int32_t &lines);

    /**
     * Return the font face height.
     */
    float get_text_height();

    /**
     * Set a new font face for emoji, check FreeType docs.
     */
    void set_font_emoji(std::string_view font_face_emoji_path);

    /**
     * Set a new font face, check FreeType docs.
     */
    void set_font(std::string_view font_face_path);

    /**
     * Set the font face height.
     */
    void set_size(uint32_t font_face_size);

    /**
     * Reload the font face with the new metrics and file path.
     */
    void reload();

    /**
     * Bind an external GPU allocator, but is not recommend pass a nullptr value.
     */
    void bind_allocator(ekg::gpu::allocator *p_allocator_bind);

    /**
     * Generate GPU data to draw text on screen.
     */
    void blit(std::string_view text, float x, float y, const ekg::vec4 &color);

    /**
     * Init the internal-system of font-rendering.
     */
    void init();

    /**
     * Quit the internal-system and free FreeType features from memory.
     */
    void quit();

    /**
     * flush new glyph(s) to re-generate texture atlas and map for the GPU-side.
     **/
    void flush();
  };
}

#endif