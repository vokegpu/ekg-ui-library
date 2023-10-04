/*
 * MIT License
 * 
 * Copyright (c) 2022-2023 Rina Wilk / vokegpu@gmail.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "ekg/ekg.hpp"

ekg::rect ekg::empty {};
int32_t   ekg::hovered::id {};
ekg::type ekg::hovered::type {};
int32_t   ekg::hovered::up {};
int32_t   ekg::hovered::down {};
ekg::type ekg::hovered::downtype {};
ekg::type ekg::hovered::uptype {};

void ekg::reload(int32_t id) {
    ekg::core->do_task_reload(ekg::core->get_fast_widget_by_id(id));
}

void ekg::reload(ekg::ui::abstract_widget *p_widget) {
    ekg::core->do_task_reload(p_widget);
}

void ekg::synclayout(int32_t id) {
    ekg::core->do_task_synclayout(ekg::core->get_fast_widget_by_id(id));
}

void ekg::synclayout(ekg::ui::abstract_widget *p_widget) {
    ekg::core->do_task_synclayout(p_widget);
}

void ekg::push_back_stack(ekg::ui::abstract_widget *p_widget, ekg::stack &stack) {
    if (p_widget == nullptr || stack.registry[p_widget->p_data->get_id()]) {
        return;
    }

    stack.registry[p_widget->p_data->get_id()] = true;
    stack.ordered_list.push_back(p_widget);

    for (int32_t &ids : p_widget->p_data->get_child_id_list()) {
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

ekg::ui::abstract_widget *ekg::find_absolute_parent_master(ekg::ui::abstract_widget *p_widget) {
    if (p_widget == nullptr || p_widget->p_data->get_parent_id() == 0) {
        return p_widget;
    }

    auto widget {ekg::core->get_fast_widget_by_id(p_widget->p_data->get_parent_id())};
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

void ekg::update_high_frequency(ekg::ui::abstract_widget *p_widget) {
    ekg::core->set_update_high_frequency(p_widget);
}

void ekg::dispatch(task *p_event) {
    ekg::core->service_handler.generate() = *p_event;
}

void ekg::dispatch(ekg::env env) {
    switch (env) {
    case ekg::env::redraw:
        ekg::core->request_redraw_gui();
        break;
    default:
        ekg::core->service_handler.dispatch_pre_allocated_task(static_cast<uint64_t>(env));
        break;
    }
}

bool ekg::listen(event &ekg_event, SDL_Event &sdl_event) {
    if (sdl_event.type == ekg::listener) {
        event *p_ekg_event {static_cast<event*>(sdl_event.user.data1)};
        ekg_event = *p_ekg_event;
        delete p_ekg_event;
        return true;
    }

    return false;
}

bool &ekg::set(bool &value, bool result) {
    if (value != result) {
        ekg::core->request_redraw_gui();
    }

    return (value = result);
}

std::string &ekg::set(std::string &value, std::string_view result) {
    if (value != result) {
        ekg::core->request_redraw_gui();
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