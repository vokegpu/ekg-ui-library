/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, free to use in anything (utility, hacking, cheat, game, software).
 * 5- Malware, rat and others virus. We do not care.
 * 6- Do not modify this license under any instance.
 *
 * @VokeGpu 2022 all rights reserved.
 */

#include "ekg/ui/slider/ui_slider_widget.hpp"
#include "ekg/util/util_ui.hpp"
#include "ekg/ui/slider/ui_slider.hpp"

void ekg::ui::slider::set_bar_axis(ekg::axis axis) {
    if (this->bar_axis != axis) {
        this->bar_axis = axis;

        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }
}

ekg::axis ekg::ui::slider::get_bar_axis() {
    return this->bar_axis;
}

void ekg::ui::slider::set_place(uint16_t flags) {
    if (this->dock_flags != flags) {
        this->dock_flags = flags;
        ekg::synclayout(this->id);
        ekg::scissor(this->parent_id);
    }
}

void ekg::ui::slider::set_bar_align(uint16_t dock) {
    if (this->bar_flags != dock) {
        this->bar_flags = dock;
        ekg::reload(this->id);
    }
}

uint16_t ekg::ui::slider::get_bar_align() {
    return this->bar_flags;;
}

void ekg::ui::slider::set_font_size(ekg::font font) {
    if (this->font_size != font) {
        this->font_size = font;

        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
        ekg::scissor(this->parent_id);
    }
}

ekg::font ekg::ui::slider::get_font_size() {
    return ekg::font::big;
}

void ekg::ui::slider::set_width(float width) {
    if (this->sync_ui.w != width) {
        this->sync_ui.w = width;

        ekg::bitwise::add(this->sync_flags, (uint16_t) ekg::uisync::dimension);
        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }
}

float ekg::ui::slider::get_width() {
    return this->rect_widget.w;
}

void ekg::ui::slider::set_scaled_height(int32_t scaled_factor_height) {
    if (this->scaled_height != scaled_factor_height) {
        this->scaled_height = scaled_factor_height;

        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
        ekg::scissor(this->parent_id);
    }
}

int32_t ekg::ui::slider::get_scaled_height() {
    return this->scaled_height;
}

float ekg::ui::slider::get_height() {
    return this->rect_widget.h;
}

void ekg::ui::slider::set_text_align(uint16_t flags) {
    if (this->text_flags != flags) {
        this->text_flags = flags;
        ekg::reload(this->id);
    }
}

uint16_t ekg::ui::slider::get_text_align() {
    return this->text_flags;
}

void ekg::ui::slider::set_value(float val) {
    val = ekg::clamp(val, this->minimum_value, this->maximum_value);;
    if (this->value != val) {
        this->value = val;
        ekg::reload(this->id);
    }
}

float ekg::ui::slider::get_value() {
    return this->value;
}

void ekg::ui::slider::set_value_min(float val_min) {
    if (this->minimum_value != val_min) {
        this->minimum_value = val_min;
        ekg::reload(this->id);
    }
}

float ekg::ui::slider::get_value_min() {
    return this->minimum_value;
}

void ekg::ui::slider::set_value_max(float val_max) {
    if (this->maximum_value != val_max) {
        this->maximum_value = val_max;
        ekg::reload(this->id);
    }
}

float ekg::ui::slider::get_value_max() {
    return this->maximum_value;
}

void ekg::ui::slider::set_precision(int32_t precision) {
    if (this->float_precision != precision) {
        this->float_precision = precision;
        ekg::reload(this->id);
    }
}

int32_t ekg::ui::slider::get_precision() {
    return this->float_precision;
}
