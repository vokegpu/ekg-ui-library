#include "ekg/util/util_ui.hpp"
#include "ekg/ekg.hpp"

ekg::rect ekg::empty {};

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

void ekg::sync_layout(uint32_t id) {
    ekg::core->sync_layout_widget(ekg::core->get_fast_widget_by_id(id));
}

void ekg::sync_layout(ekg::ui::abstract_widget *widget) {
    ekg::core->sync_layout_widget(widget);
}

void ekg::push_back_stack(ekg::ui::abstract_widget* widget, ekg::stack &stack) {
    if (widget == nullptr || stack.registry[widget->data->get_id()]) {
        return;
    }

    stack.registry[widget->data->get_id()] = true;
    stack.ordered_list.push_back(widget);

    for (uint32_t &ids : widget->data->get_parent_id_list()) {
        auto widgets = ekg::core->get_fast_widget_by_id(ids);

        if (widgets == nullptr) {
            continue;
        }

        ekg::push_back_stack(widgets, stack);
    }
}

void ekg::stack::clear() {
    this->ordered_list.clear();
    this->registry.clear();
}
