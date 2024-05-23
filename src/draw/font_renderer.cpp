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
#include "ekg/util/text.hpp"
#include "ekg/util/io.hpp"
#include "ekg/ekg.hpp"

FT_Library ekg::draw::font_renderer::ft_library {};

ekg::gpu::sampler_t *ekg::draw::font_renderer::get_sampler_texture() {
  return &this->sampler_texture;
}

float ekg::draw::font_renderer::get_text_width(std::string_view text, int32_t &lines) {
  if (text.empty()) {
    return 0.0f;
  }

  lines = 1;

  FT_Face ft_face {};
  FT_UInt ft_uint_previous {};
  FT_Vector ft_vector_previous_char {};

  float text_width {};
  float largest_text_width {};

  uint64_t text_size {text.size()};
  char32_t char32 {};
  uint8_t char8 {};
  std::string utf_string {};

  bool break_text {};
  bool r_n_break_text {};

  for (uint64_t it {}; it < text_size; it++) {
    char8 = static_cast<uint8_t>(text.at(it));
    it += ekg::utf_check_sequence(char8, char32, utf_string, text, it);

    break_text = char8 == '\n';
    if (break_text || (r_n_break_text = (char8 == '\r' && it < text_size && text.at(it + 1) == '\n'))) {
      it += static_cast<uint64_t>(r_n_break_text);
      largest_text_width = ekg_min(largest_text_width, text_width);
      text_width = 0.0f;
      lines++;
      continue;
    }

    if (this->ft_bool_kerning && ft_uint_previous) {
      switch (char32 < 256 || !this->font_face_emoji.font_face_loaded) {
        case true: {
        ft_face = this->font_face_text.ft_face;
        break;
      }

      default: {
        ft_face = this->font_face_emoji.ft_face;
        break;
      }
    }

      FT_Get_Kerning(ft_face, ft_uint_previous, char32, 0, &ft_vector_previous_char);
      text_width += static_cast<float>(ft_vector_previous_char.x >> 6);
    }

    ft_uint_previous = char32;
    text_width += this->mapped_glyph_char_data[char32].wsize;
  }

  largest_text_width = ekg_min(largest_text_width, text_width);
  return largest_text_width;
}

float ekg::draw::font_renderer::get_text_width(std::string_view text) {
  if (text.empty()) {
    return 0.0f;
  }

  FT_Face ft_face {};
  FT_Vector ft_vec {};
  FT_UInt ft_uint_previous {};
  FT_Vector ft_vector_previous_char {};

  float text_width {};
  float largest_text_width {};
  char32_t char32 {};

  uint64_t text_size {text.size()};
  std::string utf_string {};
  uint8_t char8 {};

  bool break_text {};
  bool r_n_break_text {};

  for (uint64_t it {}; it < text_size; it++) {
    char8 = static_cast<uint8_t>(text.at(it));
    it += ekg::utf_check_sequence(char8, char32, utf_string, text, it);

    break_text = char8 == '\n';
    if (break_text || (r_n_break_text = (char8 == '\r' && it < text_size && text.at(it + 1) == '\n'))) {
      it += static_cast<uint64_t>(r_n_break_text);
      largest_text_width = ekg_min(largest_text_width, text_width);
      text_width = 0.0f;
      continue;
    }

    if (this->ft_bool_kerning && ft_uint_previous) {
      switch (char32 < 256 || !this->font_face_emoji.font_face_loaded) {
        case true: {
          ft_face = this->font_face_text.ft_face;
          break;
        }

        default: {
          ft_face = this->font_face_emoji.ft_face;
          break;
        }
      }

      FT_Get_Kerning(ft_face, ft_uint_previous, char32, 0, &ft_vector_previous_char);
      text_width += static_cast<float>(ft_vector_previous_char.x >> 6);
    }

    ft_uint_previous = char32;
    text_width += this->mapped_glyph_char_data[char32].wsize;
  }

  largest_text_width = ekg_min(largest_text_width, text_width);
  return largest_text_width;
}

float ekg::draw::font_renderer::get_text_height() {
  return this->text_height;
}

void ekg::draw::font_renderer::set_font(std::string_view path) {
  if (this->font_face_text.font_path != path) {
    this->font_face_text.font_path = path;
    this->font_face_text.font_face_changed = true;
    this->reload();
  }
}

void ekg::draw::font_renderer::set_font_emoji(std::string_view path) {
  if (this->font_face_emoji.font_path != path) {
    this->font_face_emoji.font_path = path;
    this->font_face_emoji.font_face_changed = true;
    this->reload();
  }
}

void ekg::draw::font_renderer::set_size(uint32_t size) {
  if (this->font_size != size) {
    this->font_size = size;
    this->font_size_changed = true;
    this->reload();
  }
}

void ekg::draw::font_renderer::reload() {
  if (
      (!this->font_face_text.font_path.empty() && ekg::draw::reload_font_face(&this->font_face_text, this->font_size_changed, this->font_size)) ||
      (!this->font_face_emoji.font_path.empty() && ekg::draw::reload_font_face(&this->font_face_emoji, this->font_size_changed, this->font_size))
    ) {
    ekg::log() << "Failed to load font face, text: " << this->font_face_text.font_face_loaded << " , emoji: " << this->font_face_emoji.font_face_loaded;
    return;
  }

  this->font_size_changed = false;

  /* Phase of getting bitmap texture bounds. */
  this->full_width = 0;
  this->full_height = 0;

  this->ft_bool_kerning = FT_HAS_KERNING(this->font_face_text.ft_face);
  this->font_face_text.ft_glyph_slot = this->font_face_text.ft_face->glyph;

  if (this->font_face_emoji.font_face_loaded) {
    this->font_face_emoji.ft_glyph_slot = this->font_face_emoji.ft_face->glyph;
  }

  FT_ULong c {};
  FT_GlyphSlot ft_glyph_slot {};
  FT_Face ft_face {};

  for (char32_t &char32 : this->loaded_sampler_generate_list) {
    switch (char32 < 256 || !this->font_face_emoji.font_face_loaded) {
      case true: {
        ft_face = this->font_face_text.ft_face;
        ft_glyph_slot = this->font_face_text.ft_face->glyph;
        break;
      }

      default: {
        ft_face = this->font_face_emoji.ft_face;
        ft_glyph_slot = this->font_face_emoji.ft_face->glyph;
        break;
      }
    }

    if (FT_Load_Char(ft_face, char32, FT_LOAD_RENDER)) {
      continue;
    }

    this->full_width += static_cast<int32_t>(ft_glyph_slot->bitmap.width);
    this->full_height = std::max(this->full_height, static_cast<int32_t>(ft_glyph_slot->bitmap.rows));
  }

  this->text_height = static_cast<float>(this->full_height);
  this->offset_text_height = (this->text_height / 6.0f) / 2.0f;
  this->text_height += static_cast<int32_t>(this->offset_text_height);

  ekg::core->p_gpu_api->generate_font_atlas(
    &this->sampler_texture,
    &this->font_face_text,
    &this->font_face_emoji,
    this->full_width,
    this->full_height,
    this->loaded_sampler_generate_list,
    this->mapped_glyph_char_data
  );
}

void ekg::draw::font_renderer::bind_allocator(ekg::gpu::allocator *p_allocator_bind) {
  this->p_allocator = p_allocator_bind;
}

void ekg::draw::font_renderer::blit(std::string_view text, float x, float y, const ekg::vec4 &color) {
  if (this->p_allocator == nullptr || this->flag_unloaded || color.w < 0.1f || text.empty()) {
    return;
  }

  x = static_cast<float>(static_cast<int32_t>(x));
  y = static_cast<float>(static_cast<int32_t>(y - this->offset_text_height));

  ekg::gpu::data_t &data {this->p_allocator->bind_current_data()};

  data.buffer_content[0] = x;
  data.buffer_content[1] = y;
  data.buffer_content[2] = static_cast<float>(ekg::gpu::allocator::concave);
  data.buffer_content[3] = static_cast<float>(ekg::gpu::allocator::concave);

  data.buffer_content[4] = color.x;
  data.buffer_content[5] = color.y;
  data.buffer_content[6] = color.z;
  data.buffer_content[7] = color.w;

  ekg::rect vertices {};
  ekg::rect coordinates {};

  x = 0.0f;
  y = 0.0f;

  data.factor = 1;
  char32_t char32 {};
  uint8_t char8 {};

  std::string utf_string {};
  uint64_t text_size {text.size()};

  bool break_text {};
  bool r_n_break_text {};

  FT_ULong c {};
  FT_GlyphSlot ft_glyph_slot {};
  FT_Face ft_face {};
  FT_Vector ft_vector_previous_char {};

  for (uint64_t it {}; it < text_size; it++) {
    char8 = static_cast<uint8_t>(text.at(it));
    it += ekg::utf_check_sequence(char8, char32, utf_string, text, it);

    break_text = char8 == '\n';
    if (break_text || (r_n_break_text = (char8 == '\r' && it < text_size && text.at(it + 1) == '\n'))) {
      it += static_cast<uint64_t>(r_n_break_text);
      data.factor += static_cast<int32_t>(y + char32);
      y += this->text_height;
      x = 0.0f;
      continue;
    }

    if (this->ft_bool_kerning && ft_uint_previous) {
      switch (char32 < 256 || !this->font_face_emoji.font_face_loaded) {
        case true: {
          ft_face = this->font_face_text.ft_face;
          break;
        }

        default: {
          ft_face = this->font_face_emoji.ft_face;
          break;
        }
      }

      FT_Get_Kerning(ft_face, ft_uint_previous, char32, 0, &ft_vector_previous_char);
      x += static_cast<float>(ft_vector_previous_char.x >> 6);
    }

    ekg::draw::glyph_char_t &char_data {this->mapped_glyph_char_data[char32]};

    if (!char_data.was_sampled) {
      this->loaded_sampler_generate_list.emplace_back(char32);
      char_data.was_sampled = true;
    }

    vertices.x = x + char_data.left;
    vertices.y = y + this->full_height - char_data.top;

    vertices.w = char_data.w;
    vertices.h = char_data.h;

    coordinates.x = char_data.x;
    coordinates.w = vertices.w / this->full_width;
    coordinates.h = vertices.h / this->full_height;

    this->p_allocator->push_back_geometry(
      vertices.x, vertices.y, coordinates.x, coordinates.y
    );

    this->p_allocator->push_back_geometry(
      vertices.x, vertices.y + vertices.h, coordinates.x,
      coordinates.y + coordinates.h
    );

    this->p_allocator->push_back_geometry(
      vertices.x + vertices.w, vertices.y + vertices.h,
      coordinates.x + coordinates.w, coordinates.y + coordinates.h
    );

    this->p_allocator->push_back_geometry(
      vertices.x + vertices.w, vertices.y + vertices.h,
      coordinates.x + coordinates.w, coordinates.y + coordinates.h
    );

    this->p_allocator->push_back_geometry(
      vertices.x + vertices.w, vertices.y, coordinates.x + coordinates.w,
      coordinates.y
    );

    this->p_allocator->push_back_geometry(vertices.x, vertices.y, coordinates.x, coordinates.y);

    x += char_data.wsize;
    ft_uint_previous = char32;
    data.factor += static_cast<int32_t>(x + char32);
  }

  this->flush();
  this->p_allocator->bind_texture(&this->sampler_texture);
  this->p_allocator->dispatch();
}

void ekg::draw::font_renderer::flush() {
  uint64_t size {this->loaded_sampler_generate_list.size()};
  if (this->last_sampler_generate_list_size != size) {
    this->reload();
    this->last_sampler_generate_list_size = size;
  }
}

void ekg::draw::font_renderer::init() {
  this->loaded_sampler_generate_list.resize(256);
  for (char32_t char32 {}; char32 < 256; char32++) {
    this->loaded_sampler_generate_list[char32] = char32;
  }

  ekg::log() << "Initializing 256 chars rendereable";
}

void ekg::draw::font_renderer::quit() {
  if (this->font_face_text.font_face_loaded) {
    FT_Done_Face(this->font_face_text.ft_face);
    this->font_face_text.font_face_loaded = false;
  }

  if (this->font_face_emoji.font_face_loaded) {
    FT_Done_Face(this->font_face_emoji.ft_face);
    this->font_face_emoji.font_face_loaded = false;
  }
}