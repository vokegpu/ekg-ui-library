#include "ekg/util/util_ui.hpp"
#include "ekg/ekg.hpp"

void ekg::reload(uint32_t id) {
    ekg::core->reload_widget(ekg::core->get_fast_widget_by_id(id));
}

void ekg::reload(ekg::ui::abstract_widget *widget) {
    ekg::core->reload_widget(widget);
}

void ekg::reset(uint32_t id) {
    ekg::core->reset_widget(ekg::core->get_fast_widget_by_id(id));
}

void ekg::reset(ekg::ui::abstract_widget *widget) {
    ekg::core->reset_widget(widget);
}

void ekg::stack(ekg::ui::abstract_widget* widget, std::map<uint32_t, ekg::ui::abstract_widget*> &map) {
    if (widget == nullptr || map[widget->data->get_id()] != nullptr) {
        return;
    }

    map[widget->data->get_id()] = widget;

    for (uint32_t &ids : widget->data->get_parent_id_list()) {
        auto widgets = ekg::core->get_fast_widget_by_id(ids);

        if (widgets == nullptr) {
            continue;
        }

        ekg::stack(widgets, map);
    }
}
