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

#include "ekg/ekg.hpp"

ekg::rect ekg::empty {};

int32_t ekg::hovered::id {};
ekg::type ekg::hovered::type {};

int32_t ekg::hovered::up {};
int32_t ekg::hovered::down {};

ekg::type ekg::hovered::downtype {};
ekg::type ekg::hovered::uptype {};

void ekg::reload(int32_t id) {
    ekg::core->do_task_reload(ekg::core->get_fast_widget_by_id(id));
}

void ekg::reload(ekg::ui::abstract_widget *pwidget) {
    ekg::core->do_task_reload(pwidget);
}

void ekg::synclayout(int32_t id) {
    ekg::core->do_task_synclayout(ekg::core->get_fast_widget_by_id(id));
}

void ekg::synclayout(ekg::ui::abstract_widget *pwidget) {
    ekg::core->do_task_synclayout(pwidget);
}

void ekg::push_back_stack(ekg::ui::abstract_widget *pwidget, ekg::stack &stack) {
    if (pwidget == nullptr || stack.registry[pwidget->data->get_id()]) {
        return;
    }

    stack.registry[pwidget->data->get_id()] = true;
    stack.ordered_list.push_back(pwidget);

    for (int32_t &ids : pwidget->data->get_child_id_list()) {
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

void ekg::refresh(int32_t id) {
    ekg::core->do_task_refresh(ekg::core->get_fast_widget_by_id(id));
}

void ekg::refresh(ekg::ui::abstract_widget *widget) {
    ekg::core->do_task_refresh(widget);
}

void ekg::update_high_frequency(int32_t id) {
    ekg::core->set_update_high_frequency(ekg::core->get_fast_widget_by_id(id));
}

void ekg::update_high_frequency(ekg::ui::abstract_widget *widget) {
    ekg::core->set_update_high_frequency(widget);
}

void ekg::dispatch(ekg::cpu::event* event) {
    ekg::core->service_handler.generate() = *event;
}

void ekg::dispatch(ekg::env env) {
    switch (env) {
    case ekg::env::redraw:
        ekg::core->redraw_gui();
        break;
    default:
        ekg::core->service_handler.dispatch_pre_allocated_task((uint64_t) env);
        break;
    }
}

bool ekg::listen(ekg::cpu::uievent &ekg_event, SDL_Event &sdl_event) {
    if (sdl_event.type == ekg::listener) {
        ekg::cpu::uievent *p_ekg_event {static_cast<ekg::cpu::uievent*>(sdl_event.user.data1)};
        ekg_event = *p_ekg_event;
        delete p_ekg_event;
        return true;
    }

    return false;
}

bool &ekg::set(bool &value, bool result) {
    if (value != result) {
        ekg::dispatch(ekg::env::redraw);
    }

    return (value = result);
}

std::string &ekg::set(std::string &value, std::string_view result) {
    if (value != result) {
        ekg::dispatch(ekg::env::redraw);
    }

    return (value = result);
}

int32_t &ekg::set(int32_t &value, int32_t result) {
    if (value != result) {
        ekg::dispatch(ekg::env::redraw);
    }

    return (value = result);
}

bool ekg::assert_task_flag {};

float ekg::assert_task(float val, float result) {
    if (EQUALS_FLOAT(val, result)) {
        return val;
    }

    ekg::assert_task_flag = true;
    return result;
}