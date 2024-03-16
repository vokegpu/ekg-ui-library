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

#ifndef EKG_SERVICE_INPUT_H
#define EKG_SERVICE_INPUT_H

#include <vector>
#include <unordered_map>
#include <array>

#include "ekg/util/io.hpp"
#include "ekg/util/geometry.hpp"
#include "ekg/os/platform.hpp"

namespace ekg::service {
  class input {
  protected:
    std::unordered_map<std::string, std::vector<std::string>> input_bind_map {};
    std::unordered_map<std::string, bool> input_register_map {};
    std::unordered_map<std::string, bool> input_map {};
    std::array<std::string, 7> special_keys {};

    std::vector<std::string> special_keys_unit_pressed {};
    std::vector<std::string> double_click_mouse_buttons_pressed {};
    std::vector<std::string> input_register_callback {};
    std::vector<std::string> input_released_list {};
    std::vector<std::string> immediate_register_list {};

    bool finger_hold_event {};
    bool finger_swipe_event {};
    bool is_special_keys_released {};

    ekg::vec4 last_finger_interact {};
    ekg::timing double_interact {};
  public:
    ekg::timing timing_last_interact {};
    ekg::vec4 interact {};

    bool was_pressed {};
    bool was_released {};
    bool has_motion {};
    bool was_wheel {};
    bool was_typed {};
  protected:
    void complete_with_units(std::string &string_builder, std::string_view key_name);

    bool contains_unit(std::string_view label);

    bool is_special_key(int32_t sdl_key_code);
  public:
    void init();

    void bind(std::string_view input_tag, std::string_view key);

    void unbind(std::string_view input_tag, std::string_view key);

    void callback(std::string_view key, bool callback);

    void fire(std::string_view key);

    bool pressed(std::string_view key);

    bool receive(std::string_view key);

    void on_event(ekg::os::io_event_serial &io_event_serial);

    void on_update();
  };
}

#endif