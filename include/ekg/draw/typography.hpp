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

#ifndef EKG_DRAW_TYPOGRAPHY_H
#define EKG_DRAW_TYPOGRAPHY_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include <cstdint>

namespace ekg::draw {
  struct glyph_char_t {
  public:
    float x {};
    float wsize {};
    float w {};
    float h {};
    float top {};
    float left {};
    float kerning {};
    bool was_sampled {};
  };

  struct font_face_t {
  public:
    FT_Face ft_face {};
    FT_GlyphSlot ft_glyph_slot {};
    std::string_view font_path {};
    bool font_face_changed {};
    bool font_face_loaded {};
  };

  bool reload_font_face(
    ekg::draw::font_face_t *p_font_face,
    bool font_size_changed,
    uint32_t font_size
  );
}

#endif