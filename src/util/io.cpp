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

ekg::item::item(std::string_view insert_value) {
  this->value = insert_value;
}

ekg::item::item(std::string_view insert_value, uint16_t insert_attr_bits) {
  this->value = insert_value;
  this->attr_bits = insert_attr_bits;
}

ekg::item::~item() {
  
}

std::ostringstream ekg::log::buffer {};
bool ekg::log::buffered {};
bool ekg::log::tracked {};

int64_t ekg::timing::second {};
int64_t ekg::timing::ticks {};

bool ekg::reach(ekg::timing &timing, uint64_t ms) {
  timing.ticks_going_on = ekg::timing::ticks;
  timing.current_ticks = timing.ticks_going_on - timing.elapsed_ticks;
  return timing.current_ticks > ms;
}

bool ekg::reset(ekg::timing &timing) {
  timing.elapsed_ticks = timing.ticks_going_on;
  return true;
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