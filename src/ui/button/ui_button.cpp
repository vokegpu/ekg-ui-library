#include "ekg/ui/button/ui_button.hpp"
#include "ekg/util/util_event.hpp"
#include "ekg/util/util_ui.hpp"

void ekg::ui::button::set_value(bool state) {
    if (this->value != state) {
        this->value = state;
        ekg::reload(this->id);
    }
}

bool ekg::ui::button::get_value() {
    return this->value;
}

void ekg::ui::button::set_text(std::string_view new_text) {
    if (this->text != new_text) {
        this->text = new_text;
        ekg::reload(this->id);
    }
}

std::string_view ekg::ui::button::get_text() {
    return this->text;
}

void ekg::ui::button::set_text(uint16_t enum_docks) {
    if (this->dock_text != enum_docks) {
        this->dock_text = enum_docks;
        ekg::reload(this->id);
    }
}

uint16_t ekg::ui::button::get_text_dock() {
    return this->dock_text;
}

void ekg::ui::button::set_callback(ekg::cpu::event* event) {
    event->flags = ekg::bitwise::add(event->flags, ekg::event::shared);

    if (this->callback != event && this->callback != nullptr) {
        delete this->callback;
        this->callback = nullptr;
    }

    this->callback = event;
}

ekg::cpu::event *ekg::ui::button::get_callback() {
    return callback;
}

void ekg::ui::button::set_width(float width) {
    if (this->sync_ui.w != width) {
        this->sync_ui.w = width;
        this->set_sync_with_ui(true);
    }
}

float ekg::ui::button::get_width() {
    return this->rect_widget.w;
}

void ekg::ui::button::set_scaled_height(int32_t scaled_height_factor) {
    if (this->scaled_height != scaled_height_factor) {
        this->scaled_height = scaled_height_factor;
        ekg::reload(this->id);
    }
}

float ekg::ui::button::get_height() {
    return this->rect_widget.h;
}

int32_t ekg::ui::button::get_scaled_height() {
    return this->scaled_height;
}

void ekg::ui::button::set_dock(uint16_t flags) {
    if (this->dock != flags) {
        this->dock = flags;
        ekg::sync_layout(this->id);
    }
}

void ekg::ui::button::set_font_size(ekg::font font) {
    if (this->font_size != font) {
        this->font_size = font;
        ekg::reload(this->id);
    }
}

ekg::font ekg::ui::button::get_font_size() {
    return this->font_size;
}
