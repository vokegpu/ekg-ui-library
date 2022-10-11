#include "ekg/ui/button/ui_button.hpp"
#include "ekg/util/util_event.hpp"
#include "ekg/util/util_ui.hpp"

void ekg::ui::button::set_value(bool state) {
    if (this->value != state) {
        this->value = state;
        ekg::update(this->id);
    }
}

bool ekg::ui::button::get_value() {
    return this->value;
}

void ekg::ui::button::set_text(std::string_view new_text) {
    if (this->text != new_text) {
        this->text = new_text;
        ekg::update(this->id);
    }
}

std::string_view ekg::ui::button::get_text() {
    return this->text;
}

void ekg::ui::button::set_text(uint16_t enum_docks) {
    if (this->dock_text != enum_docks) {
        this->dock_text = enum_docks;
        ekg::update(this->id);
    }
}

uint16_t ekg::ui::button::get_text_dock() {
    return this->dock_text;
}

void ekg::ui::button::set_callback(ekg::cpu::event *callback) {
    callback->flags = ekg::bitwise::add(callback->flags, ekg::event::shared);
}

ekg::cpu::event *ekg::ui::button::get_callback() {
    return callback;
}
