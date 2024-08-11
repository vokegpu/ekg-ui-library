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
#include "ekg/util/io.hpp"

#include <fstream>

/* start of item */

ekg::item::item(
  std::string_view insert_value,
  uint16_t insert_attr_bits
) {
  this->placement.rect.w = 100.0f;
  this->value = insert_value;
  this->attr_bits = insert_attr_bits;
  this->placement.text_dock_flags = ekg::dock::left | ekg::dock::center;
}

ekg::item::item(
  std::string_view insert_value,
  std::vector<ekg::item> insert_item_list,
  uint16_t insert_attr_bits
) {
  this->placement.rect.w = 100.0f;
  this->value = insert_value;
  this->insert(this->begin(), insert_item_list.begin(), insert_item_list.end());
  this->attr_bits = insert_attr_bits;
  this->placement.text_dock_flags = ekg::dock::left | ekg::dock::center;
}

ekg::item::~item() {
  if (this->p_semaphore != nullptr) {
    *this->p_semaphore = true; 
  }
}

void ekg::item::unsafe_set_addressed(ekg::item *p_set_addressed) {
  this->p_addressed = p_set_addressed; 
}

ekg::item *ekg::item::unsafe_get_addressed() {
  return this->p_addressed;
}

ekg::placement &ekg::item::unsafe_get_placement() {
  return this->placement;
}

void ekg::item::unsafe_set_semaphore(bool *p_set_semaphore) {
  this->p_semaphore = p_set_semaphore;
}

void ekg::item::unsafe_set_visible_count(uint64_t count) {
  this->visible_count = count;
}

uint64_t ekg::item::get_visible_count() {
  return this->visible_count;
}

void ekg::item::set_value(std::string_view new_value) {
  this->value = new_value;
}

std::string ekg::item::get_value() {
  return this->value;
}

void ekg::item::set_attr(uint16_t bits) {
  this->attr_bits = bits;
}

uint16_t ekg::item::get_attr() {
  return this->attr_bits;
}

void ekg::item::set_text_align(uint16_t bits) {
  if (this->placement.text_dock_flags != bits) {
    this->placement.text_dock_flags = bits;
  }
}

uint16_t ekg::item::get_text_align() {
  return this->placement.text_dock_flags;
}

/* end of item */

std::ostringstream ekg::log::buffer {};
bool ekg::log::buffered {};
bool ekg::log::tracked {};

int64_t ekg::timing::second {};
int64_t ekg::timing::ticks {};

bool ekg::reach(ekg::timing &timing, int64_t ms) {
  timing.ticks_going_on = ekg::timing::ticks;
  timing.current_ticks = timing.ticks_going_on - timing.elapsed_ticks;
  return timing.current_ticks > ms;
}

bool ekg::reset(ekg::timing &timing) {
  timing.elapsed_ticks = timing.ticks_going_on;
  return true;
}

bool ekg::extend(ekg::timing &timing, int64_t ms) {
  timing.elapsed_ticks = timing.ticks_going_on - ms;
  return true;
}

int64_t ekg::interval(ekg::timing &timing) {
  timing.ticks_going_on = ekg::timing::ticks;
  return (
    timing.current_ticks = timing.ticks_going_on - timing.elapsed_ticks
  );
}

bool ekg::file_to_string(std::string &file_content, std::string_view path) {
  std::ifstream ifs {path.data()};
  if (ifs.is_open()) {
    std::string line_content {};
    while (std::getline(ifs, line_content)) {
      file_content += line_content;
      file_content += '\n';
    }

    ifs.close();
    return false;
  } else {
    ekg::log() << "Failed to read file '" << path << "\'!";
  }

  return true;
}

bool ekg::input::action(std::string_view key_input) {
  return ekg::core->service_input.pressed(key_input);
}

void ekg::input::bind(std::string_view key_input, std::string_view value_input) {
  return ekg::core->service_input.bind(key_input, value_input);
}

void ekg::input::bind(std::string_view key_input, const std::vector<std::string_view> &bind_list) {
  for (const std::string_view &bind : bind_list) {
    ekg::input::bind(key_input, bind);
  }
}

void ekg::input::fire(std::string_view key_input) {
  ekg::core->service_input.fire(key_input);
}

void ekg::input::clipboard(ekg::clipboard clipboard_op) {
  // meow
}

bool ekg::input::motion() {
  return ekg::core->service_input.has_motion;
}

bool ekg::input::released() {
  return ekg::core->service_input.was_released;
}

bool ekg::input::pressed() {
  return ekg::core->service_input.was_pressed;
}

bool ekg::input::wheel() {
  return ekg::core->service_input.was_wheel;
}

bool ekg::input::receive(std::string_view tag) {
  return ekg::core->service_input.receive(tag);
}

bool ekg::input::typed() {
  return ekg::core->service_input.was_typed;
}

ekg::vec4 &ekg::input::interact() {
  return ekg::core->service_input.interact;
}