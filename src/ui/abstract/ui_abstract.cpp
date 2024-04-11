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

#include "ekg/ui/abstract/ui_abstract.hpp"
#include "ekg/ekg.hpp"

ekg::ui::abstract::abstract() {

}

ekg::ui::abstract::~abstract() {
}

ekg::ui::abstract *ekg::ui::abstract::add_child(int32_t to_insert_id) {
  bool contains {};
  for (int32_t &ids: this->child_id_list) {
    if ((contains = ids == to_insert_id)) {
      break;
    }
  }

  ekg::ui::abstract_widget *p_child_widget {};
  ekg::ui::abstract_widget *p_parent_widget {};

  if (contains == false && (
      (p_child_widget = ekg::core->get_fast_widget_by_id(to_insert_id)) != nullptr
      &&
      (p_parent_widget = ekg::core->get_fast_widget_by_id(this->id)) != nullptr
    )) {
    p_child_widget->p_data->set_parent_id(this->id);
    p_child_widget->p_parent = &this->rect_widget;
    p_child_widget->p_parent_scissor = &p_parent_widget->scissor;

    this->child_id_list.push_back(to_insert_id);
  }

  return this;
}

std::vector<int32_t> &ekg::ui::abstract::get_child_id_list() {
  return this->child_id_list;
}

ekg::ui::abstract *ekg::ui::abstract::remove_child(int32_t id) {
  ekg::ui::abstract_widget *p_widget {};

  uint64_t it {};
  bool contains {};

  for (it = 0; it < this->child_id_list.size(); it++) {
    if ((contains = this->child_id_list.at(it) == id)) {
      break;
    }
  }

  if (contains == false && (p_widget = ekg::core->get_fast_widget_by_id(id)) != nullptr) {
    this->child_id_list.erase(this->child_id_list.begin() + it);
    p_widget->p_data->set_parent_id(0);
    p_widget->p_parent = &p_widget->empty_parent;
  }

  return this;
}

ekg::ui::abstract *ekg::ui::abstract::unsafe_set_id(int32_t id) {
  this->id = id;
  return this;
}

int32_t ekg::ui::abstract::get_id() {
  return this->id;
}

ekg::ui::abstract *ekg::ui::abstract::set_parent_id(int32_t id) {
  this->parent_id = id;
  return this;
}

int32_t ekg::ui::abstract::get_parent_id() {
  return this->parent_id;
}

ekg::ui::abstract *ekg::ui::abstract::set_alive(bool state) {
  this->alive = state;
  return this;
}

bool ekg::ui::abstract::is_alive() {
  return this->alive;
}

ekg::ui::abstract *ekg::ui::abstract::set_visible(bool state) {
  this->visible = state;
  return this;
}

bool ekg::ui::abstract::is_visible() {
  return this->visible;
}

void ekg::ui::abstract::destroy() {
  this->set_alive(false);
  this->unsafe_destroy_childs();
  ekg::refresh(this->id);
}

void ekg::ui::abstract::unsafe_destroy_childs() {
  for (int32_t &ids: this->child_id_list) {
    ekg::ui::abstract_widget *p_widget {ekg::core->get_fast_widget_by_id(ids)};
    if (p_widget != nullptr && p_widget->p_data != nullptr) {
      p_widget->p_data->destroy();
    }
  }
}

ekg::ui::abstract *ekg::ui::abstract::set_state(const ekg::state &enum_state) {
  this->state = enum_state;
  return this;
}

ekg::state ekg::ui::abstract::get_state() {
  return this->state;
}

ekg::ui::abstract *ekg::ui::abstract::set_level(const ekg::level &_level) {
  this->level = _level;
  return this;
}

ekg::level ekg::ui::abstract::get_level() {
  return this->level;
}

ekg::ui::abstract *ekg::ui::abstract::unsafe_set_type(ekg::type enum_type) {
  this->type = enum_type;
  return this;
}

ekg::type ekg::ui::abstract::get_type() {
  return this->type;
}

uint16_t ekg::ui::abstract::get_place_dock() {
  return this->dock_flags;
}

ekg::rect &ekg::ui::abstract::widget() {
  return this->rect_widget;
}

ekg::rect &ekg::ui::abstract::ui() {
  return this->sync_ui;
}

ekg::ui::abstract *ekg::ui::abstract::set_tag(std::string_view string) {
  this->tag = string;
  return this;
}

std::string_view ekg::ui::abstract::get_tag() {
  return this->tag;
}

uint16_t &ekg::ui::abstract::get_sync() {
  return this->sync_flags;
}

ekg::ui::abstract *ekg::ui::abstract::set_task(ekg::task *p_task, ekg::action action) {
  this->action_register[static_cast<uint64_t>(action)] = p_task;
  return this;
}

ekg::task *ekg::ui::abstract::get_task(ekg::action action) {
  return this->action_register[static_cast<uint64_t>(action)];
}

void ekg::ui::abstract::reset() {
  ekg_bitwise_add(this->sync_flags, static_cast<uint64_t>(ekg::ui_sync::reset));
  ekg::reload(this->id);
}

ekg::ui::abstract *ekg::ui::abstract::unsafe_set_scaled_height_layout(int32_t scaled_height_int) {
  this->scaled_height = scaled_height_int;
  return this;
}

bool ekg::ui::abstract::has_parent() const {
  return this->parent_id != 0;
}

bool ekg::ui::abstract::has_children() {
  return !this->child_id_list.empty();
}