#include "ekg/ui/slider/ui_slider_widget.hpp"
#include "ekg/util/util_ui.hpp"
#include "ekg/ui/slider/ui_slider.hpp"

void ekg::ui::slider::set_dock(uint16_t flags) {
    if (this->dock != flags) {
        this->dock = flags;
        ekg::sync_layout(this->id);
    }
}

void ekg::ui::slider::set_bar(ekg::dock orientation) {
    if (this->dock_bar != orientation) {
        this->dock_bar = orientation;
        ekg::reload(this->id);
    }
}

ekg::dock ekg::ui::slider::get_bar() {
    return this->dock_bar;
}

void ekg::ui::slider::set_font_size(ekg::font font) {
    if (this->font_size != font) {
        this->font_size = font;
        ekg::reload(this->id);
    }
}

ekg::font ekg::ui::slider::get_font_size() {
    return ekg::font::big;
}

void ekg::ui::slider::set_width(float width) {
    if (this->sync_ui.w != width) {
        this->sync_ui.w = width;
        this->set_sync_with_ui(true);
    }
}

float ekg::ui::slider::get_width() {
    return this->rect_widget.w;
}

void ekg::ui::slider::set_scaled_height(int32_t scaled_factor_height) {
    if (this->scaled_height != scaled_factor_height) {
        this->scaled_height = scaled_factor_height;
        ekg::reset(this->id);
    }
}

int32_t ekg::ui::slider::get_scaled_height() {
    return this->scaled_height;
}

float ekg::ui::slider::get_height() {
    return this->rect_widget.h;
}

void ekg::ui::slider::set_value(float val) {
    if (this->value != val) {
        this->value = val;
        this->ieee754 = false;
        ekg::reset(this->id);
    }
}

void ekg::ui::slider::set_value(int32_t val) {
    if (this->value != static_cast<float>(val)) {
        this->value = static_cast<float>(val);
        this->ieee754 = true;
        ekg::reset(this->id);
    }
}

float ekg::ui::slider::get_value() {
    return this->value;
}

void ekg::ui::slider::set_value_min(int32_t val_min) {
    if (this->minimum_value != static_cast<float>(val_min)) {
        this->minimum_value = static_cast<float>(val_min);
        this->ieee754 = true;
        ekg::reload(this->id);
    }
}

void ekg::ui::slider::set_value_min(float val_min) {
    if (this->minimum_value != val_min) {
        this->minimum_value = val_min;
        this->ieee754 = false;
        ekg::reload(this->id);
    }
}

float ekg::ui::slider::get_value_min() {
    return this->minimum_value;
}

void ekg::ui::slider::set_value_max(int32_t val_max) {
    if (this->maximum_value != static_cast<float>(val_max)) {
        this->maximum_value = static_cast<float>(val_max);
        this->ieee754 = true;
        ekg::reload(this->id);
    }
}

void ekg::ui::slider::set_value_max(float val_max) {
    if (this->maximum_value != val_max) {
        this->maximum_value = val_max;
        this->ieee754 = false;
        ekg::reload(this->id);
    }
}

float ekg::ui::slider::get_value_max() {
    return this->maximum_value;
}

bool ekg::ui::slider::is_float_precision() {
    return this->ieee754;
}
