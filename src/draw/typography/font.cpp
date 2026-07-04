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

#include "ekg/draw/typography/font.hpp"
#include "ekg/io/utf.hpp"
#include "ekg/core/runtime.hpp"
#include "ekg/core/context.hpp"
#include "ekg/io/log.hpp"
#include "ekg/core/pools.hpp"

void ekg::draw::font::init() {
  if (this->was_initialized) {
    return;
  }

  this->was_initialized = true;

  this->new_glyphs_to_atlas.resize(256);
  for (char32_t char32 {}; char32 < 256; char32++) {
    this->new_glyphs_to_atlas.at(char32) = char32;
  }

  ekg::log() << "Initializing 256 default chars!";
}

void ekg::draw::font::quit() {
  ekg::io::font_face_t &text_font_face {this->faces[ekg::io::font_face_type::text]};
  ekg::io::font_face_t &emojis_font_face {this->faces[ekg::io::font_face_type::emojis]};
  ekg::io::font_face_t &kanjis_font_face {this->faces[ekg::io::font_face_type::kanjis]};

  if (text_font_face.was_loaded) {
    FT_Done_Face(text_font_face.ft_face);
    text_font_face.was_loaded = false;
  }

  if (emojis_font_face.was_loaded) {
    FT_Done_Face(emojis_font_face.ft_face);
    emojis_font_face.was_loaded = false;
  }

  if (kanjis_font_face.was_loaded) {
    FT_Done_Face(kanjis_font_face.ft_face);
    kanjis_font_face.was_loaded = false;
  }
}

ekg::sampler_t &ekg::draw::font::get_atlas_texture_sampler() {
  return ekg::query<ekg::sampler_t>(this->atlas_texture_sampler_at);
}

float ekg::draw::font::get_text_width(
  const std::string_view &text,
  int32_t &lines
) {
  if (
      text.empty()
      ||
      !this->is_any_functional_font_face_loaded
    ) {
    return 0.0f;
  }

  FT_Face ft_face {};
  FT_UInt ft_uint_previous {};
  FT_Vector ft_vector_previous_char {};
  FT_GlyphSlot ft_glyph_slot {};

  float text_width {};
  float largest_text_width {};

  int32_t lines_count {};
  uint64_t text_size {text.size()};
  
  char32_t char32 {};
  uint8_t char8 {};
  std::string utf_string {};

  bool break_text {};
  bool r_n_break_text {};

  ekg::io::font_face_t &text_font_face {this->faces[ekg::io::font_face_type::text]};
  ekg::io::font_face_t &emojis_font_face {this->faces[ekg::io::font_face_type::emojis]};
  ekg::io::font_face_t &kanjis_font_face {this->faces[ekg::io::font_face_type::kanjis]};

  for (uint64_t it {}; it < text_size; it++) {
    char8 = static_cast<uint8_t>(text.at(it));
    it += ekg::utf8_check_sequence(char8, char32, utf_string, text, it);
    break_text = char8 == '\n';
    if (break_text || (r_n_break_text = (char8 == '\r' && it < text_size && text.at(it + 1) == '\n'))) {
      it += static_cast<uint64_t>(r_n_break_text);
      largest_text_width = ekg::clamp_min(largest_text_width, text_width);
      text_width = 0.0f;
      lines_count++;
      continue;
    }

    switch (char32 < 256 || !emojis_font_face.was_loaded) {
      case true: {
        ft_face = text_font_face.ft_face;
        ft_glyph_slot = text_font_face.ft_face->glyph;
        break;
      }

      default: {
        ft_face = emojis_font_face.ft_face;
        ft_glyph_slot = emojis_font_face.ft_face->glyph;
        break;
      }
    }

    if (this->ft_bool_kerning && ft_uint_previous) {
      FT_Get_Kerning(ft_face, ft_uint_previous, char32, 0, &ft_vector_previous_char);
      text_width += static_cast<float>(ft_vector_previous_char.x >> 6);
    }

    ekg::io::glyph_t &glyph {this->mapped_glyph[char32]};

    if (!glyph.was_sampled) {
      if (
          FT_Load_Char(
            ft_face,
            char32,
            FT_LOAD_RENDER | FT_LOAD_DEFAULT | FT_LOAD_COLOR
          )
        ) {
        continue;
      }

      glyph.wsize = static_cast<float>(static_cast<int32_t>(ft_glyph_slot->advance.x >> 6));
      this->new_glyphs_to_atlas.emplace_back(char32);
      glyph.was_sampled = true;
    }

    ft_uint_previous = char32;
    text_width += this->mapped_glyph[char32].wsize;
  }

  lines = ekg::clamp_min(lines, lines_count);
  largest_text_width = ekg::clamp_min(largest_text_width, text_width);

  return largest_text_width;
}

float ekg::draw::font::get_text_width(const std::string_view &text) {
  if (
      text.empty()
      ||
      !this->is_any_functional_font_face_loaded
    ) {
    return 0.0f;
  }

  FT_Face ft_face {};
  FT_UInt ft_uint_previous {};
  FT_Vector ft_vector_previous_char {};
  FT_GlyphSlot ft_glyph_slot {};

  float text_width {};
  float largest_text_width {};
  char32_t char32 {};

  uint64_t text_size {text.size()};
  std::string utf_string {};
  uint8_t char8 {};

  bool break_text {};
  bool r_n_break_text {};

  ekg::io::font_face_t &text_font_face {this->faces[ekg::io::font_face_type::text]};
  ekg::io::font_face_t &emojis_font_face {this->faces[ekg::io::font_face_type::emojis]};
  ekg::io::font_face_t &kanjis_font_face {this->faces[ekg::io::font_face_type::kanjis]};

  for (uint64_t it {}; it < text_size; it++) {
    char8 = static_cast<uint8_t>(text.at(it));
    it += ekg::utf8_check_sequence(char8, char32, utf_string, text, it);

    break_text = char8 == '\n';
    if (break_text || (r_n_break_text = (char8 == '\r' && it < text_size && text.at(it + 1) == '\n'))) {
      it += static_cast<uint64_t>(r_n_break_text);
      largest_text_width = ekg::clamp_min(largest_text_width, text_width);
      text_width = 0.0f;
      continue;
    }

    switch (char32 < 256 || !emojis_font_face.was_loaded) {
      case true: {
        ft_face = text_font_face.ft_face;
        ft_glyph_slot = text_font_face.ft_face->glyph;
        break;
      }

      default: {
        ft_face = emojis_font_face.ft_face;
        ft_glyph_slot = emojis_font_face.ft_face->glyph;
        break;
      }
    }

    if (this->ft_bool_kerning && ft_uint_previous) {
      FT_Get_Kerning(ft_face, ft_uint_previous, char32, 0, &ft_vector_previous_char);
      text_width += static_cast<float>(ft_vector_previous_char.x >> 6);
    }

    ekg::io::glyph_t &glyph {this->mapped_glyph[char32]};

    if (!glyph.was_sampled) {
      if (
          FT_Load_Char(
            ft_face,
            char32,
            FT_LOAD_RENDER | FT_LOAD_DEFAULT | FT_LOAD_COLOR
          )
        ) {
        continue;
      }

      glyph.wsize = static_cast<float>(static_cast<int32_t>(ft_glyph_slot->advance.x >> 6));
      this->new_glyphs_to_atlas.emplace_back(char32);
      glyph.was_sampled = true;
    }

    ft_uint_previous = char32;
    text_width += glyph.wsize;
  }

  largest_text_width = ekg::clamp_min(largest_text_width, text_width);
  return largest_text_width;
}

float ekg::draw::font::get_text_height() {
  return this->text_height;
}

void ekg::draw::font::set_font(const std::string_view &path) {
  ekg::io::font_face_t &font_face {
    this->faces[ekg::io::font_face_type::text]
  };

  if (!path.empty() && font_face.path != path) {
    font_face.path = path;
    font_face.was_face_changed = true;
    font_face.was_size_changed = true;
    this->reload();
  }
}

void ekg::draw::font::set_font_emoji(const std::string_view &path) {
  ekg::io::font_face_t &font_face {
    this->faces[ekg::io::font_face_type::emojis]
  };

  if (!path.empty() && font_face.path != path) {
    font_face.path = path;
    font_face.was_face_changed = true;
    font_face.was_size_changed = true;

    this->reload();
  }
}

void ekg::draw::font::set_size(uint32_t size) {
  if (this->font_size != size) {
    for (size_t it {}; it < ekg::io::enum_font_face_type_size; it++) {
      ekg::io::font_face_t &font_face {
        this->faces[it]
      };

      font_face.size = size;
      font_face.was_size_changed = true;
    }

    this->font_size = size;
    this->reload();
  }
}

void ekg::draw::font::reload() {
  if (this->font_size == 0) {
    return;
  }

  if (this->atlas_texture_sampler_at == ekg::at_t::not_found) {
    ekg::sampler_t &sampler {ekg::make<ekg::sampler_t>({})};
    this->atlas_texture_sampler_at = sampler.at;
    sampler.tag = "font-renderer";
    sampler.gl_protected_active_index = true;
  }

  size_t functional_fonts {};
  ekg::flags_t flags {};

  for (size_t it {}; it < ekg::io::enum_font_face_type_size; it++) {
    ekg::io::font_face_t &font_face {
      this->faces[it]
    };

    flags = ekg::io::font(
      font_face
    );

    if (ekg::has(flags, ekg::result::success)) {
      functional_fonts++;
    }
  }

  this->is_any_functional_font_face_loaded = functional_fonts != 0;
  if (!this->is_any_functional_font_face_loaded) {
    ekg::log() << "EKG could not make font rendering reload, there are no functional font faces loaded!";
    return;
  }

  this->atlas_rect.w = 0;
  this->atlas_rect.h = 0;

  ekg::io::font_face_t &text_font_face {this->faces[ekg::io::font_face_type::text]};
  ekg::io::font_face_t &emojis_font_face {this->faces[ekg::io::font_face_type::emojis]};
  ekg::io::font_face_t &kanjis_font_face {this->faces[ekg::io::font_face_type::kanjis]};

  text_font_face.highest_glyph_size = FT_Vector {};
  emojis_font_face.highest_glyph_size = FT_Vector {};

  this->ft_bool_kerning = FT_HAS_KERNING(text_font_face.ft_face);
  text_font_face.ft_glyph_slot = text_font_face.ft_face->glyph;

  if (emojis_font_face.was_loaded) {
    emojis_font_face.ft_glyph_slot = emojis_font_face.ft_face->glyph;
  }

  FT_GlyphSlot ft_glyph_slot {};
  FT_Face ft_face {};

  ekg::io::font_face_t *p_font_face_picked {};
  for (char32_t &char32 : this->new_glyphs_to_atlas) {
    switch (char32 < 256 || !emojis_font_face.was_loaded) {
      case true: {
        ft_face = text_font_face.ft_face;
        ft_glyph_slot = text_font_face.ft_face->glyph;
        flags = FT_LOAD_RENDER;
        p_font_face_picked = &text_font_face;
        break;
      }

      default: {
        ft_face = emojis_font_face.ft_face;
        ft_glyph_slot = emojis_font_face.ft_face->glyph;
        flags = FT_LOAD_RENDER | FT_LOAD_COLOR;
        p_font_face_picked = &emojis_font_face;
        break;
      }
    }

    if (FT_Load_Char(ft_face, char32, flags)) {
      continue;
    }

    ekg::io::glyph_t &glyph {
      this->mapped_glyph[char32]
    };

    glyph.w = static_cast<float>(ft_glyph_slot->bitmap.width);
    glyph.h = static_cast<float>(ft_glyph_slot->bitmap.rows);

    glyph.left = static_cast<float>(ft_glyph_slot->bitmap_left);
    glyph.top = static_cast<float>(ft_glyph_slot->bitmap_top);
    glyph.wsize = static_cast<float>(static_cast<int32_t>(ft_glyph_slot->advance.x >> 6));

    this->atlas_rect.w += static_cast<int32_t>(glyph.w);
    this->atlas_rect.h = ekg::clamp_min<int32_t>(this->atlas_rect.h, static_cast<int32_t>(glyph.h));

    p_font_face_picked->highest_glyph_size.x = ekg::clamp_min(
      static_cast<int32_t>(p_font_face_picked->highest_glyph_size.x),
      static_cast<int32_t>(glyph.w)
    );

    p_font_face_picked->highest_glyph_size.y = ekg::clamp_min(
      static_cast<int32_t>(p_font_face_picked->highest_glyph_size.y),
      static_cast<int32_t>(glyph.h)
    );
  }

  this->is_monospaced = FT_IS_FIXED_WIDTH(text_font_face.ft_face);
  this->space_wsize = this->text_height / 3.0f;

  FT_Vector space_char_metrics {};
  FT_Get_Kerning(text_font_face.ft_face, 32, 32, 0, &space_char_metrics);

  this->text_height = static_cast<float>(this->font_size);
  this->offset_text_height = this->text_height / 6;

  ekg::p_core->p_gpu_api->gen_font_atlas_and_map_glyph(
    ekg::query<ekg::sampler_t>(this->atlas_texture_sampler_at),
    &text_font_face,
    &emojis_font_face,
    nullptr, // must impl kanjis wmwmwm
    this->atlas_rect,
    this->new_glyphs_to_atlas,
    this->mapped_glyph,
    this->non_swizzlable_range
  );
}

void ekg::draw::font::blit(
  const std::string_view &text,
  float x,
  float y,
  const ekg::rgba_t<uint8_t> &color
) {
  if (
    !this->is_any_functional_font_face_loaded
    ||
    color.w < 0.1f 
    ||
    text.empty()
  ) {
    return;
  }

  x = static_cast<float>(static_cast<int32_t>(x));
  y = static_cast<float>(static_cast<int32_t>(y - this->offset_text_height));

  ekg::gpu::data_t &data {
    ekg::p_core->draw_allocator.bind_current_data()
  };

  data.buffer[0] = x;
  data.buffer[1] = y;
  data.buffer[2] = static_cast<float>(-this->non_swizzlable_range);

  data.buffer[4] = static_cast<float>(color.x / 255);
  data.buffer[5] = static_cast<float>(color.y / 255);
  data.buffer[6] = static_cast<float>(color.z / 255);
  data.buffer[7] = static_cast<float>(color.w / 255);

  ekg::rect_t<float> vertex {};
  ekg::rect_t<float> uv {};

  x = 0.0f;
  y = 0.0f;

  data.hash = 1;
  char32_t char32 {};
  uint8_t char8 {};

  std::string utf_string {};
  uint64_t text_size {text.size()};

  bool break_text {};
  bool r_n_break_text {};

  FT_Face ft_face {};
  FT_Vector ft_vector_previous_char {};
  char32_t ft_uint_previous {};

  ekg::io::font_face_t &text_font_face {this->faces[ekg::io::font_face_type::text]};
  ekg::io::font_face_t &emojis_font_face {this->faces[ekg::io::font_face_type::emojis]};
  ekg::io::font_face_t &kanjis_font_face {this->faces[ekg::io::font_face_type::kanjis]};

  for (uint64_t it {}; it < text_size; it++) {
    char8 = static_cast<uint8_t>(text.at(it));
    it += ekg::utf8_check_sequence(char8, char32, utf_string, text, it);

    break_text = char8 == '\n';
    if (
        break_text
        ||
        (
          r_n_break_text = (
            char8 == '\r' && it < text_size && text.at(it + 1) == '\n'
          )
        )
      ) {

      ekg::io::glyph_t &glyph {this->mapped_glyph[char32]};

      it += static_cast<uint64_t>(r_n_break_text);
      data.hash += ekg_generate_hash(y, char32, glyph.x);

      y += this->text_height;
      x = 0.0f;
      continue;
    }

    if (this->ft_bool_kerning && ft_uint_previous) {
      switch (char32 < 256 || !emojis_font_face.was_loaded) {
        case true: {
          ft_face = text_font_face.ft_face;
          break;
        }

        default: {
          ft_face = emojis_font_face.ft_face;
          break;
        }
      }

      FT_Get_Kerning(ft_face, ft_uint_previous, char32, 0, &ft_vector_previous_char);
      x += static_cast<float>(ft_vector_previous_char.x >> 6);
    }

    ekg::io::glyph_t &glyph {this->mapped_glyph[char32]};

    if (!glyph.was_sampled) {
      this->new_glyphs_to_atlas.emplace_back(char32);
      glyph.was_sampled = true;
    }

    vertex.x = x + glyph.left;
    vertex.y = y + this->atlas_rect.h - glyph.top;

    vertex.w = glyph.w;
    vertex.h = glyph.h;

    uv.x = glyph.x;
    uv.w = vertex.w / this->atlas_rect.w;
    uv.h = vertex.h / this->atlas_rect.h;

    ekg::p_core->draw_allocator.push_back_geometry(
      vertex.x,
      vertex.y,
      uv.x,
      uv.y
    );

    ekg::p_core->draw_allocator.push_back_geometry(
      vertex.x,
      vertex.y + vertex.h,
      uv.x,
      uv.y + uv.h
    );

    ekg::p_core->draw_allocator.push_back_geometry(
      vertex.x + vertex.w,
      vertex.y + vertex.h,
      uv.x + uv.w,
      uv.y + uv.h
    );

    ekg::p_core->draw_allocator.push_back_geometry(
      vertex.x + vertex.w,
      vertex.y + vertex.h,
      uv.x + uv.w,
      uv.y + uv.h
    );

    ekg::p_core->draw_allocator.push_back_geometry(
      vertex.x + vertex.w,
      vertex.y,
      uv.x + uv.w,
      uv.y
    );

    ekg::p_core->draw_allocator.push_back_geometry(
      vertex.x,
      vertex.y,
      uv.x,
      uv.y
    );

    x += glyph.wsize;
    ft_uint_previous = char32;

    /**
     * Peek `ekg/io/memory.hpp` for better hash definition and purpose.
     **/
    data.hash += ekg_generate_hash(x, char32, glyph.x);
  }
  this->flush();

  ekg::draw::allocator::is_simple_shape = false;
  ekg::p_core->draw_allocator.bind_texture(this->atlas_texture_sampler_at);
  ekg::p_core->draw_allocator.dispatch();
}

void ekg::draw::font::flush() {
  uint64_t size {this->new_glyphs_to_atlas.size()};
  if (this->last_sampler_generate_list_size != size) {
    ekg::log() << "Sampler updated from-to: " << this->last_sampler_generate_list_size << " " << size;

    this->reload();
    this->last_sampler_generate_list_size = size;

    ekg::gui.ui.redraw = true;
  }
}

ekg::draw::font &ekg::draw::get_font_renderer(ekg::font font) {
  switch (font) {
  case ekg::font::small:
    return ekg::p_core->draw_font_small;
  case ekg::font::big:
    return ekg::p_core->draw_font_big;
  default:
    break;
  }

  return ekg::p_core->draw_font_medium;;
}
