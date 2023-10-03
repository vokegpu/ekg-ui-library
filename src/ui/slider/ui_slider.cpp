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

#include "ekg/ui/slider/ui_slider_widget.hpp"
#include "ekg/util/gui.hpp"
#include "ekg/ui/slider/ui_slider.hpp"

ekg::ui::slider *ekg::ui::slider::set_dragging(bool state) {
    this->dragging = state;
    return this;
}

bool ekg::ui::slider::is_dragging() {
    return this->dragging;
}

ekg::ui::slider *ekg::ui::slider::set_bar_axis(ekg::axis axis) {
    if (this->bar_axis != axis) {
        this->bar_axis = axis;

        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }

    return this;
}

ekg::axis ekg::ui::slider::get_bar_axis() {
    return this->bar_axis;
}

ekg::ui::slider *ekg::ui::slider::set_place(uint16_t flags) {
    if (this->dock_flags != flags) {
        this->dock_flags = flags;
        ekg::synclayout(this->id);
    }

    return this;
}

ekg::ui::slider *ekg::ui::slider::set_bar_align(uint16_t dock) {
    if (this->bar_flags != dock) {
        this->bar_flags = dock;
        ekg::reload(this->id);
    }

    return this;
}

uint16_t ekg::ui::slider::get_bar_align() {
    return this->bar_flags;;
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

ekg::ui::slider *ekg::ui::slider::set_width(float width) {
    if (this->sync_ui.w != width) {
        this->sync_ui.w = width;

        ekg::bitwise::add(this->sync_flags, (uint16_t) ekg::ui_sync::dimension);
        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }

    return this;
}

float ekg::ui::slider::get_width() {
    return this->rect_widget.w;
}

ekg::ui::slider *ekg::ui::slider::set_scaled_height(int32_t scaled_factor_height) {
    if (this->scaled_height != scaled_factor_height) {
        this->scaled_height = scaled_factor_height;

        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }

    return this;
}

int32_t ekg::ui::slider::get_scaled_height() {
    return this->scaled_height;
}

float ekg::ui::slider::get_height() {
    return this->rect_widget.h;
}

ekg::ui::slider *ekg::ui::slider::set_text_align(uint16_t flags) {
    if (this->text_flags != flags) {
        this->text_flags = flags;
        ekg::reload(this->id);
    }

    return this;
}

uint16_t ekg::ui::slider::get_text_align() {
    return this->text_flags;
}

ekg::ui::slider *ekg::ui::slider::set_value(float val) {
    val = ekg::clamp(val, this->minimum_value, this->maximum_value);;
    if (this->value != val) {
        this->value = val;
        ekg::reload(this->id);
    }

    return this;
}

float ekg::ui::slider::get_value() {
    return this->value;
}

ekg::ui::slider *ekg::ui::slider::set_value_min(float val_min) {
    if (this->minimum_value != val_min) {
        this->minimum_value = val_min;
        ekg::reload(this->id);
    }

    return this;
}

float ekg::ui::slider::get_value_min() {
    return this->minimum_value;
}

ekg::ui::slider *ekg::ui::slider::set_value_max(float val_max) {
    if (this->maximum_value != val_max) {
        this->maximum_value = val_max;
        ekg::reload(this->id);
    }

    return this;
}

float ekg::ui::slider::get_value_max() {
    return this->maximum_value;
}

ekg::ui::slider *ekg::ui::slider::set_precision(int32_t precision) {
    if (this->float_precision != precision) {
        this->float_precision = precision;
        ekg::reload(this->id);
    }

    return this;
}

int32_t ekg::ui::slider::get_precision() {
    return this->float_precision;
}