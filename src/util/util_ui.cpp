/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, free to use in anything (utility, hacking, cheat, game, software).
 * 5- Malware, rat and others virus. We do not care.
 * 6- Do not modify this license under any instance.
 *
 * @VokeGpu 2022 all rights reserved.
 */

#include "ekg/util/util_ui.hpp"
#include "ekg/ekg.hpp"

ekg::rect ekg::empty {};

void ekg::reload(uint32_t id) {
    ekg::core->reload_widget(ekg::core->get_fast_widget_by_id(id));
}

void ekg::reload(ekg::ui::abstract_widget *pwidget) {
    ekg::core->reload_widget(pwidget);
}

void ekg::reset(uint32_t id) {
    ekg::core->reset_widget(ekg::core->get_fast_widget_by_id(id));
}

void ekg::reset(ekg::ui::abstract_widget *pwidget) {
    ekg::core->reset_widget(pwidget);
}

void ekg::sync_layout(uint32_t id) {
    ekg::core->sync_layout_widget(ekg::core->get_fast_widget_by_id(id));
}

void ekg::sync_layout(ekg::ui::abstract_widget *pwidget) {
    ekg::core->sync_layout_widget(pwidget);
}

void ekg::push_back_stack(ekg::ui::abstract_widget *pwidget, ekg::stack &stack) {
    if (pwidget == nullptr || stack.registry[pwidget->data->get_id()]) {
        return;
    }

    stack.registry[pwidget->data->get_id()] = true;
    stack.ordered_list.push_back(pwidget);

    for (uint32_t &ids : pwidget->data->get_parent_id_list()) {
        auto widgets {ekg::core->get_fast_widget_by_id(ids)};

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

ekg::ui::abstract_widget *ekg::find_absolute_parent_master(ekg::ui::abstract_widget *pwidget) {
    if (pwidget == nullptr || pwidget->data->get_parent_id() == 0) {
        return pwidget;
    }

    auto widget {ekg::core->get_fast_widget_by_id(pwidget->data->get_parent_id())};
    return ekg::find_absolute_parent_master(widget);
}
