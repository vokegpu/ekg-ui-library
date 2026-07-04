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
#ifndef EKG_HANDLER_INPUT_HPP
#define EKG_HANDLER_INPUT_HPP

#include "ekg/io/timing.hpp"
#include "ekg/math/geometry.hpp"

#include <cstdint>
#include <string_view>
#include <vector>

namespace ekg {
  enum class system_cursor {
    arrow,
    ibeam,
    wait,
    crosshair,
    wait_arrow,
    size_nwse,
    size_nesw,
    size_we,
    size_ns,
    size_all,
    no,
    hand
  };

  enum class special_key {
    left_shift,
    right_shift,
    left_ctrl,
    right_ctrl,
    left_alt,
    right_alt,
    tab,
    unknown
  };

  struct input_info_t {
  public:
    float scroll_speed {1.0f};
    ekg::timing_t ui_timeout_timing {};
    ekg::timing_t ui_scrolling_timing {};
    ekg::timing_t timing_last_interact {};
    ekg::timing_t ui_timing {};
    ekg::vec4_t<float> interact {};
    bool was_pressed {};
    bool was_released {};
    bool has_motion {};
    bool was_wheel {};
    bool was_typed {};
    std::string_view typed {};
  };

  using input_bind_function_t = ekg::result (*)(std::string_view, bool);

  struct input_key_t {
  public:
    int32_t key {};
    int32_t scancode {};
  };

  struct input_bind_t {
  public:
    std::vector<std::string> registry {};
    bool state {};
    bool *p_address {};
  };
}

namespace ekg {
  ekg::input_info_t &input();
  bool fired(std::string_view tag);
  bool input(std::string_view input);
  void bind(std::string_view tag, std::string_view input);
  void bind(std::string_view tag, std::vector<std::string_view> inputs);
  void listener(ekg::input_bind_function_t input_bind_listener_function);
}

#endif
