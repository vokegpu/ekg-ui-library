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

#include "ekg/ekg.hpp"
#include "ekg/ui/textbox/ui_textbox.hpp"
#include "ekg/ui/textbox/ui_textbox_widget.hpp"

ekg::ui::textbox *ekg::ui::textbox::set_must_format_text(bool state) {
  this->must_format_text = state;
  return this;
}

bool ekg::ui::textbox::is_must_format_text() {
  return this->must_format_text;
}

ekg::ui::textbox *ekg::ui::textbox::set_tab_size(uint8_t size) {
  this->tab_size = ekg_min(size, 1);
  return this;
}

uint8_t ekg::ui::textbox::get_tab_size() {
  return this->tab_size;
}

ekg::ui::textbox *ekg::ui::textbox::set_enabled(bool state) {
  this->enabled = state;
  return this;
}

bool ekg::ui::textbox::is_enabled() {
  return this->enabled;
}

ekg::ui::textbox *ekg::ui::textbox::set_place(uint16_t flags) {
  if (this->dock_flags != flags) {
    this->dock_flags = flags;
    ekg::synclayout(this->parent_id);
  }

  return this;
}

std::string ekg::ui::textbox::get_text() {
  if (this->must_format_text) {
    uint64_t text_chunk_size {this->size()};
    for (uint64_t it {}; it < this->size(); it++) {
      this->formatted_text += this->at(it);
      this->formatted_text += '\n' && (it + 1 == text_chunk_size);
    }

    this->must_format_text = false;
  }

  return this->formatted_text;
}

ekg::ui::textbox *ekg::ui::textbox::set_width(float w) {
  if (this->sync_ui.w != w) {
    this->sync_ui.w = w;

    ekg::bitwise::add(this->sync_flags, (uint16_t) ekg::ui_sync::dimension);
    ekg::reload(this->id);
    ekg::synclayout(this->parent_id);
  }

  return this;
}

float ekg::ui::textbox::get_width() {
  return this->rect_widget.w;
}

ekg::ui::textbox *ekg::ui::textbox::set_scaled_height(int32_t scaled_factor_height) {
  if (this->scaled_height != scaled_factor_height) {
    this->scaled_height = scaled_factor_height;

    ekg::reload(this->id);
    ekg::synclayout(this->parent_id);
  }

  return this;
}

float ekg::ui::textbox::get_height() {
  return this->rect_widget.h;
}

int32_t ekg::ui::textbox::get_scaled_height() {
  return this->scaled_height;
}

ekg::ui::textbox *ekg::ui::textbox::set_font_size(ekg::font font) {
  if (this->font_size != font) {
    this->font_size = font;

    ekg::reload(this->id);
    ekg::synclayout(this->parent_id);
  }

  return this;
}

ekg::font ekg::ui::textbox::get_font_size() {
  return this->font_size;
}

ekg::ui::textbox *ekg::ui::textbox::set_max_lines(uint64_t lines) {
  if (this->max_lines != lines) {
    this->max_lines = lines;
    ekg::reload(this->id);
  }

  return this;
}

uint64_t ekg::ui::textbox::get_max_lines() {
  return this->max_lines;
}

ekg::ui::textbox *ekg::ui::textbox::set_max_chars_per_line(uint64_t chars_per_line) {
  if (this->max_chars_per_line != chars_per_line) {
    this->max_chars_per_line = chars_per_line;
    ekg::reload(this->id);
  }

  return this;
}

uint64_t ekg::ui::textbox::get_max_chars_per_line() {
  return this->max_chars_per_line;
}