#include "ekg/ui/checkbox/ui_checkbox.hpp"
#include "ekg/util/util_ui.hpp"

void ekg::ui::checkbox::set_font_size(ekg::font font) {
    if (this->font_size != font) {
        this->font_size = font;
        ekg::reset(this->id);
        ekg::sync_layout(this->parent_id);
    }
}

ekg::font ekg::ui::checkbox::get_font_size() {
    return this->font_size;
}

void ekg::ui::checkbox::set_dock(uint16_t docks) {
    if (this->dock != docks) {
        this->dock = docks;
        ekg::sync_layout(this->id);
    }
}

void ekg::ui::checkbox::set_width(float width) {
    if (this->sync_ui.w != width) {
        this->sync_ui.w = width;
        this->set_sync_with_ui(true);
        ekg::sync_layout(this->parent_id);
    }
}

float ekg::ui::checkbox::get_width() {
    return this->rect_widget.w;
}

void ekg::ui::checkbox::set_scaled_height(int32_t scaled_height_factor) {
    if (this->scaled_height != scaled_height_factor) {
        this->scaled_height = scaled_height_factor;
        ekg::reload(this->id);
        ekg::sync_layout(this->parent_id);
    }
}

int32_t ekg::ui::checkbox::get_scaled_height() {
    return this->scaled_height;
}

float ekg::ui::checkbox::get_height() {
    return this->rect_widget.h;
}

void ekg::ui::checkbox::set_text(std::string_view string) {
    if (this->text != string) {
        this->text = string;
        ekg::reload(this->id);
        ekg::sync_layout(this->parent_id);
    }
}

std::string_view ekg::ui::checkbox::get_text() {
    return this->text;
}

void ekg::ui::checkbox::set_value(bool val) {
    if (this->value != val) {
        this->value = val;
        ekg::reload(this->id);
    }
}

bool ekg::ui::checkbox::get_value() {
    return this->value;
}

void ekg::ui::checkbox::set_text(uint16_t docks) {
    if (this->dock_text != docks) {
        this->dock_text = docks;
        ekg::reload(this->id);
    }
}

uint16_t ekg::ui::checkbox::get_text_dock() {
    return this->dock_text;
}
