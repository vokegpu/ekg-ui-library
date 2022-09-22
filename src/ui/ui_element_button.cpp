#include "ekg/ui/ui_element_button.hpp"

void ekg::boolean::set_value(bool value) {
    this->value = value;
}

bool ekg::boolean::get_value() {
    return this->value;
}
