#include "ekg/ui/button/ui_button.hpp"

void ekg::ui::button::set_value(bool state) {
    this->value = state;
}

bool ekg::ui::button::get_value() {
    return this->value;
}
