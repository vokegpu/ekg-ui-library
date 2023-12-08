/*
* MIT License
* 
* Copyright (c) 2022-2023 Rina Wilk / vokegpu@gmail.com
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

#include "ekg/ui/label/ui_label.hpp"
#include "ekg/util/gui.hpp"

ekg::ui::label *ekg::ui::label::set_place(uint16_t flags) {
  if (this->dock_flags != flags) {
    this->dock_flags = flags;
    ekg::synclayout(this->parent_id);
  }

  return this;
}

ekg::ui::label *ekg::ui::label::set_text(std::string_view string) {
  if (this->text != string) {
    this->text = string;
    ekg::reload(this->id);
  }

  return this;
}

std::string ekg::ui::label::get_text() {
  return this->text;
}

ekg::ui::label *ekg::ui::label::set_text_align(uint16_t flags) {
  if (this->dock_text != flags) {
    this->dock_text = flags;
    ekg::reload(this->id);
  }

  return this;
}

uint16_t ekg::ui::label::get_text_align() {
  return this->dock_text;
}

ekg::ui::label *ekg::ui::label::set_width(float width) {
  if (this->sync_ui.w != width) {
    this->sync_ui.w = width;

    ekg::bitwise::add(this->sync_flags, (uint16_t) ekg::ui_sync::dimension);
    ekg::reload(this->id);
    ekg::synclayout(this->parent_id);
  }

  return this;
}

float ekg::ui::label::get_width() {
  return this->rect_widget.w;
}

ekg::ui::label *ekg::ui::label::set_scaled_height(int32_t scaled_factor_height) {
  if (this->scaled_height != scaled_factor_height) {
    this->scaled_height = scaled_factor_height;

    ekg::reload(this->id);
    ekg::synclayout(this->parent_id);
  }

  return this;
}

float ekg::ui::label::get_height() {
  return this->rect_widget.h;
}

int32_t ekg::ui::label::get_scaled_height() {
  return this->scaled_height;
}

ekg::ui::label *ekg::ui::label::set_font_size(ekg::font font) {
  if (this->font_size != font) {
    this->font_size = font;

    ekg::reload(this->id);
    ekg::synclayout(this->parent_id);
  }

  return this;
}

ekg::font ekg::ui::label::get_font_size() {
  return this->font_size;
}