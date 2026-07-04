/**
 * MIT License
 * 
 * Copyright (c) 2022-2025 Rina Wilk / vokegpu@gmail.com
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
#ifndef EKG_DRAW_TYPOGRAPHY_FONT_HPP
#define EKG_DRAW_TYPOGRAPHY_FONT_HPP

#include <iostream>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <array>

#include "ekg/math/geometry.hpp"
#include "ekg/io/font.hpp"
#include "ekg/gpu/sampler.hpp"

namespace ekg::draw {
  class font {
  public:
    std::vector<char32_t> new_glyphs_to_atlas {};
    size_t last_sampler_generate_list_size {};

    std::unordered_map<char32_t, ekg::io::glyph_t> mapped_glyph {};
    std::array<ekg::io::font_face_t, ekg::io::enum_font_face_type_size> faces {};

    ekg::at_t atlas_texture_sampler_at {ekg::at_t::not_found};
    ekg::rect_t<int32_t> atlas_rect {};
    float offset_text_height {};

    uint32_t font_size {};
    float text_height {};
    float non_swizzlable_range {};
    float space_wsize {};
    FT_Bool ft_bool_kerning {};

    bool font_size_changed {};
    bool was_initialized {};
    bool is_any_functional_font_face_loaded {};
    bool is_monospaced {};
  public:
    void init();
    void quit();
    void flush();

    ekg::sampler_t &get_atlas_texture_sampler();
    float get_text_width(const std::string_view &text);
    float get_text_width(const std::string_view &text, int32_t &lines);
    float get_text_height();

    void set_font_emoji(const std::string_view &font_face_emoji_path);
    void set_font(const std::string_view &font_face_path);
    void set_size(uint32_t font_face_size);
    void reload();

    void blit(
      const std::string_view &text,
      float x, float y,
      const ekg::rgba_t<uint8_t> &color
    );
  };

  ekg::draw::font &get_font_renderer(ekg::font font);
}

#endif
