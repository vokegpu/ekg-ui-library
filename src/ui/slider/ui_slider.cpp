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

#include "ekg/ui/slider/ui_slider_widget.hpp"
#include "ekg/util/gui.hpp"
#include "ekg/ui/slider/ui_slider.hpp"

ekg::ui::slider *ekg::ui::slider::unsafe_set_number(ekg::number number) {
  this->number = number;
  return this;
}

std::vector<ekg::ui::slider::range_t> &ekg::ui::slider::get_range_list() {
  return this->range_list;
}

ekg::number ekg::ui::slider::get_number() {
  return this->number;
}

ekg::ui::slider *ekg::ui::slider::set_axis(ekg::axis new_axis) {
  if (this->axis != new_axis) {
    this->axis = new_axis;
    ekg::reload(this->id);
    ekg::synclayout(this->parent_id);

  }
  return this;
}

ekg::axis ekg::ui::slider::get_axis() {
  return this->axis;
}

ekg::ui::slider *ekg::ui::slider::set_text_align(ekg::flags enum_docks) {
  if (this->dock_text != enum_docks) {
    this->dock_text = enum_docks;

    ekg::reload(this->id);
    ekg::synclayout(this->parent_id);
  }

  return this;
}

ekg::flags ekg::ui::slider::get_text_align() {
  return this->dock_text;
}

ekg::ui::slider *ekg::ui::slider::set_width(float w) {
  if (this->sync_ui.w != w) {
    this->sync_ui.w = w;

    ekg_bitwise_add(this->sync_flags, static_cast<ekg::flags>(ekg::ui_sync::dimension));
    ekg::reload(this->id);
    ekg::synclayout(this->parent_id);
  }

  return this;
}

float ekg::ui::slider::get_width() {
  return this->rect_widget.w;
}

ekg::ui::slider *ekg::ui::slider::set_scaled_height(int32_t scaled_height_factor) {
  if (this->scaled_height != scaled_height_factor) {
    this->scaled_height = scaled_height_factor;

    ekg::reload(this->id);
    ekg::synclayout(this->parent_id);
  }

  return this;
}

float ekg::ui::slider::get_height() {
  return this->rect_widget.h;
}

int32_t ekg::ui::slider::get_scaled_height() {
  return this->scaled_height;
}

ekg::ui::slider *ekg::ui::slider::set_place(ekg::flags flags) {
  if (this->dock_flags != flags) {
    this->dock_flags = flags;

    ekg::reload(this->id);
    ekg::synclayout(this->id);
  }

  return this;
}

ekg::ui::slider *ekg::ui::slider::set_font_size(ekg::font font) {
  if (this->font_size != font) {
    this->font_size = font;

    ekg::reload(this->id);
    ekg::synclayout(this->parent_id);
  }

  return this;
}

ekg::font ekg::ui::slider::get_font_size() {
  return this->font_size;
}