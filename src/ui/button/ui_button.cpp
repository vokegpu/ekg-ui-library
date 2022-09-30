#include "ekg/ui/button/ui_button.hpp"
#include "ekg/util/util_virtual_thread.hpp"
#include "ekg/ekg.hpp"

void ekg::ui::button::set_value(bool state) {
    if (this->value != state) {
        this->value = state;

        ekg::core->update_widget((ui::abstract_widget*) this);
    }
}

bool ekg::ui::button::get_value() {
    return this->value;
}
