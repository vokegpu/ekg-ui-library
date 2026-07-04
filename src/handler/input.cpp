/**
 * MIT License
 * 
 * Copyright (c) 2022-2025 Rina Wilk / vokegpu@gmail.com
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
#include "ekg/handler/input.hpp"
#include "ekg/core/runtime.hpp"

ekg::input_info_t &ekg::input() {
  return ekg::p_core->handler_input.input;
}

bool ekg::fired(std::string_view tag) {
  return ekg::p_core->handler_input.get_input_bind_state(tag);
}

bool ekg::input(std::string_view input) {
  return ekg::p_core->handler_input.get_input_state(input);
}

void ekg::bind(std::string_view tag, std::string_view input) {
  ekg::p_core->handler_input.insert_input_bind(tag, input);
}

void ekg::bind(std::string_view tag, std::vector<std::string_view> inputs) {
  for (std::string_view &input : inputs) {
    ekg::bind(tag, inputs);
  }
}

void ekg::listener(ekg::input_bind_function_t input_bind_listener_function) {
  ekg::p_core->handler_input.set_input_bind_listener(input_bind_listener_function);
}
