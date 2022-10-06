#include "ekg/util/util_ui.hpp"
#include "ekg/ekg.hpp"

void ekg::update(uint32_t id) {
    ekg::core->update_widget(ekg::core->get_fast_widget_by_id(id));
}

void ekg::update(ekg::ui::abstract_widget *widget) {
    ekg::core->update_widget(widget);
}
