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

#include "ekg/draw/font_renderer.hpp"
#include "ekg/util/io.hpp"

bool ekg::draw::reload_font_face(
	ekg::draw::font_face_t *p_font_face,
  bool font_size_changed,
  uint32_t font_size
) {
  if (p_font_face->font_face_changed) {
    if (p_font_face->font_face_loaded) {
      FT_Done_Face(p_font_face->ft_face);
      p_font_face->font_face_loaded = false;
    }

    p_font_face->font_face_loaded = FT_New_Face(
      ekg::draw::font_renderer::ft_library,
      p_font_face->font_path.data(),
      0,
      &p_font_face->ft_face
    );

    if (p_font_face->font_face_loaded) {
      ekg::log() << "Could not load font " << p_font_face->font_path;
      return true;
    }

    ekg::log() << "Font '" << p_font_face->font_path << "' loaded!"; 

    p_font_face->font_face_loaded = true;
    p_font_face->font_face_changed = false;
  }

  if (p_font_face->font_face_loaded && font_size_changed) {
    FT_Set_Pixel_Sizes(p_font_face->ft_face, 0, font_size);
  }

  return false;
}