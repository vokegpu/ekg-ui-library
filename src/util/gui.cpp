/**
 * MIT License
 * 
 * Copyright (c) 2022-2024 Rina Wilk / vokegpu@gmail.com
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
#include "ekg/ui/abstract/ui_abstract_widget.hpp"

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
  if (p_widget == nullptr) {
    return;
  }

  if (stack.target_id == p_widget->p_data->get_id()) {
    stack.target_id_found = true;
  }

  stack.ordered_list.push_back(p_widget);

  for (int32_t &ids: p_widget->p_data->get_child_id_list()) {
    ekg::ui::abstract_widget *p_widgets {ekg::core->get_fast_widget_by_id(ids)};
    if (p_widgets == nullptr) {
      continue;
    }

    ekg::push_back_stack(p_widgets, stack);
  }
}

ekg::stack::stack(std::string_view tag, bool do_not_trigger_stack_update) {
  this->tag = tag;

  if (do_not_trigger_stack_update) {
    return;
  }

  this->push();
}

ekg::stack::~stack() {
  this->destroy();
}

void ekg::stack::push() {
  ekg::core->set_current_stack(this);
}

void ekg::stack::pop() {
  ekg::core->set_current_stack(nullptr);
}

void ekg::stack::destroy() {
  for (ekg::ui::abstract_widget *p_widget : this->ordered_list) {
    p_widget->p_data->destroy();
  }

  this->ordered_list.clear();
}

void ekg::stack::clear() {
  this->ordered_list.clear();
  this->target_id_found = false;
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

void ekg::dispatch(task *p_task) {
  if (!p_task->is_dispatched) {
    ekg::core->service_handler.dispatch(p_task);
    //p_task->is_dispatched = true;
  }
}

void ekg::dispatch(ekg::env env) {
  switch (env) {
    case ekg::env::redraw:
      ekg::ui::redraw = true;
      break;
    default:
      ekg::core->service_handler.dispatch_pre_allocated_task(static_cast<uint64_t>(env));
      break;
  }
}

bool &ekg::set(bool &value, bool result) {
  if (value != result) {
    ekg::ui::redraw = true;
  }

  return (value = result);
}

std::string &ekg::set(std::string &value, std::string_view result) {
  if (value != result) {
    ekg::ui::redraw = true;
  }

  return (value = result);
}

int32_t &ekg::set(int32_t &value, int32_t result) {
  if (value != result) {
    ekg::dispatch(ekg::env::redraw);
  }

  return (value = result);
}