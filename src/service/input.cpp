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

#include "ekg/service/input.hpp"
#include <string>
#include <algorithm>
#include "ekg/ekg.hpp"

void ekg::service::input::init() {
  uint64_t it {};

  /**
   * Forced null termination at end due the necessary optmization,
   * when changing dynamically the special key state.
   * 
   * If lshift is pressed, then the last char is set to `+`,
   * or be `lshift+` (from `lshift\0` to `lshift+`).
   * Unpressed special key result in `\0.
   */

  this->special_keys[0][0] = '\0';
  this->special_keys[0][1] = 's';
  this->special_keys[0][2] = 'h';
  this->special_keys[0][3] = 'i';
  this->special_keys[0][4] = 'f';
  this->special_keys[0][5] = 't';
  this->special_keys[0][6] = '+';
  this->special_keys[0][7] = '\0';

  this->special_keys[1][0] = '\0'; 
  this->special_keys[1][1] = 's'; 
  this->special_keys[1][2] = 'h'; 
  this->special_keys[1][3] = 'i';
  this->special_keys[1][4] = 'f';
  this->special_keys[1][5] = 't';
  this->special_keys[1][6] = '+';
  this->special_keys[1][7] = '\0';

  this->special_keys[2][0] = '\0'; 
  this->special_keys[2][1] = 'c'; 
  this->special_keys[2][2] = 't'; 
  this->special_keys[2][3] = 'r';
  this->special_keys[2][4] = 'l';
  this->special_keys[2][5] = '+';
  this->special_keys[2][6] = '\0';
  this->special_keys[2][7] = '\0';
  
  this->special_keys[3][0] = '\0'; 
  this->special_keys[3][1] = 'c'; 
  this->special_keys[3][2] = 't'; 
  this->special_keys[3][3] = 'r';
  this->special_keys[3][4] = 'l';
  this->special_keys[3][5] = '+';
  this->special_keys[3][6] = '\0';
  this->special_keys[3][7] = '\0';

  this->special_keys[4][0] = '\0'; 
  this->special_keys[4][1] = 'l'; 
  this->special_keys[4][2] = 't'; 
  this->special_keys[4][3] = '+';
  this->special_keys[4][4] = '\0';
  this->special_keys[4][5] = '\0';
  this->special_keys[4][6] = '\0';
  this->special_keys[4][7] = '\0';

  this->special_keys[5][0] = '\0'; 
  this->special_keys[5][1] = 'l'; 
  this->special_keys[5][2] = 't'; 
  this->special_keys[5][3] = 'g';
  this->special_keys[5][4] = 'r';
  this->special_keys[5][5] = '+';
  this->special_keys[5][6] = '\0';
  this->special_keys[5][7] = '\0';

  this->special_keys[6][0] = '\0'; 
  this->special_keys[6][1] = 'a'; 
  this->special_keys[6][2] = 'b'; 
  this->special_keys[6][3] = '+';
  this->special_keys[6][4] = '\0';
  this->special_keys[6][5] = '\0';
  this->special_keys[6][6] = '\0';
  this->special_keys[6][7] = '\0';
}

void ekg::service::input::on_event(ekg::os::io_event_serial &io_event_serialized) {
  this->was_pressed = false;
  this->was_released = false;
  this->has_motion = false;
  this->was_typed = false;

  float wheel_precise_interval {};

  switch (io_event_serialized.event_type) {
    case ekg::platform_event_type::text_input: {
      this->was_pressed = true;
      this->was_typed = true;
      break;
    }

    case ekg::platform_event_type::key_down: {
      this->was_pressed = true;

      std::string key_name {};
      std::string string_builder {};

      ekg::core->p_os_platform->get_key_name(
        io_event_serialized.key,
        key_name
      );

      ekg::special_key special_key {ekg::special_key::unknown};
      ekg::core->p_os_platform->get_special_key(io_event_serialized.key, special_key);

      if (special_key != ekg::special_key::unknown) {
        this->special_keys[static_cast<uint64_t>(special_key)][0] = key_name[0];
        string_builder += key_name;

        this->callback(string_builder, true);
        this->is_special_keys_released = true;
      } else {
        std::transform(key_name.begin(), key_name.end(), key_name.begin(), ::tolower);
        string_builder += "abs-";
        string_builder += key_name;

        this->callback(string_builder, true);
        this->input_released_list.push_back(string_builder);

        string_builder.clear();
        this->complete_with_units(string_builder, key_name);
        this->callback(string_builder, true);
        this->input_released_list.push_back(string_builder);

        if (string_builder != key_name && !this->contains_unit(string_builder)) {
          this->special_keys_unit_pressed.push_back(string_builder);
        }
      }

      break;
    }

    case ekg::platform_event_type::key_up: {
      this->was_released = true;
      std::string key_name {};
      std::string string_builder {};

      ekg::core->p_os_platform->get_key_name(
        io_event_serialized.key,
        key_name
      );

      ekg::special_key special_key {ekg::special_key::unknown};
      ekg::core->p_os_platform->get_special_key(io_event_serialized.key, special_key);

      if (special_key != ekg::special_key::unknown) {
        this->special_keys[static_cast<uint64_t>(special_key)][0] = '\0';
        string_builder += key_name;

        this->callback(string_builder, false);
        this->is_special_keys_released = true;

        string_builder += "-up";
        this->callback(string_builder, true);
      } else {
        std::transform(key_name.begin(), key_name.end(), key_name.begin(), ::tolower);
        string_builder += "abs-";
        string_builder += key_name;
        string_builder += "-up";

        this->callback(string_builder, true);
        this->input_released_list.push_back(string_builder);

        string_builder.clear();
        this->complete_with_units(string_builder, key_name);
        string_builder += "-up";

        this->callback(string_builder, true);
        this->input_released_list.push_back(string_builder);
      }
      
      break;
    }

    case ekg::platform_event_type::mouse_button_down: {
      std::string string_builder {};
      std::string key_name {"mouse-"};
      key_name += std::to_string(io_event_serialized.mouse_button);

      this->was_pressed = true;
      this->complete_with_units(string_builder, key_name);
      this->callback(string_builder, true);
      this->input_released_list.push_back(string_builder);

      bool double_click_factor {ekg::reach(this->double_interact, 500)};
      if (!double_click_factor) {
        string_builder += "-double";
        this->callback(string_builder, true);

        this->double_click_mouse_buttons_pressed.push_back(string_builder);
        this->input_released_list.push_back(string_builder);
      }

      if (double_click_factor) {
        ekg::reset(this->double_interact);
      }

      break;
    }

    case ekg::platform_event_type::mouse_button_up: {
      this->was_released = true;
      std::string string_builder {};
      std::string key_name {"mouse-"};
      key_name += std::to_string(io_event_serialized.mouse_button);

      this->complete_with_units(string_builder, key_name);
      string_builder += "-up";

      this->callback(string_builder, true);
      this->input_released_list.push_back(string_builder);
      break;
    }

    case ekg::platform_event_type::mouse_motion: {
      this->has_motion = true;
      this->interact.x = static_cast<float>(io_event_serialized.mouse_motion_x);
      this->interact.y = static_cast<float>(io_event_serialized.mouse_motion_y);
      break;
    }

    case ekg::platform_event_type::mouse_wheel: {
      std::string string_builder {};
      this->complete_with_units(string_builder, "mouse-wheel");
      this->callback(string_builder, true);
      this->input_released_list.push_back(string_builder);
      this->was_wheel = true;

      this->callback("mouse-wheel-up", io_event_serialized.mouse_wheel_y > 0);
      this->callback("mouse-wheel-down", io_event_serialized.mouse_wheel_y < 0);
      this->callback("mouse-wheel-right", io_event_serialized.mouse_wheel_x > 0);
      this->callback("mouse-wheel-left", io_event_serialized.mouse_wheel_x < 0);

      /**
       * I do not know how actually implement smooth scroll,
       * SDL/GLFW does not give an option to receive a precise smooth scrolling intensity.
       * 
       * So the interval between old event and new event, inverse to, 1000, returns
       * the intensity.
       **/

      wheel_precise_interval = static_cast<float>(
        1000 - ekg_clamp(ekg::interval(this->last_time_wheel_was_fired), 0, 1000)
      );

      wheel_precise_interval = (wheel_precise_interval / 1000.0f);
      wheel_precise_interval = wheel_precise_interval + (static_cast<float>(wheel_precise_interval > 0.99) * 0.5f);
      wheel_precise_interval = ekg_min(wheel_precise_interval, 0.2f);

      this->interact.z = io_event_serialized.mouse_wheel_precise_x * wheel_precise_interval;
      this->interact.w = io_event_serialized.mouse_wheel_precise_y * wheel_precise_interval;
      
      ekg::reset(this->last_time_wheel_was_fired);
      break;
    }

    case ekg::platform_event_type::finger_down: {
      this->was_pressed = true;
      ekg::reset(this->timing_last_interact);
      bool reach_double_interact {ekg::reach(this->double_interact, 500)};

      this->interact.x = io_event_serialized.finger_x * static_cast<float>(ekg::ui::width);
      this->interact.y = io_event_serialized.finger_y * static_cast<float>(ekg::ui::height);

      this->last_finger_interact.x = this->interact.x;
      this->last_finger_interact.y = this->interact.y;

      this->callback("finger-click", true);
      this->callback("finger-click-double", !reach_double_interact);

      if (reach_double_interact) {
        ekg::reset(this->double_interact);
      }

      break;
    }

    case ekg::platform_event_type::finger_up: {
      this->was_released = true;
      this->callback("finger-hold", (this->finger_hold_event = ekg::reach(this->timing_last_interact, 750)));
      this->callback("finger-click", false);
      this->callback("finger-click-double", false);

      /**
       * Should stop the swipe event when there is no finger touching on screen.
       **/
      this->callback("finger-swipe", false);
      this->callback("finger-swipe-up", false);
      this->callback("finger-swipe-down", false);

      this->interact.x = io_event_serialized.finger_x * static_cast<float>(ekg::ui::width);
      this->interact.y = io_event_serialized.finger_y * static_cast<float>(ekg::ui::height);

      this->last_finger_interact.x = this->interact.x;
      this->last_finger_interact.y = this->interact.y;

      this->interact.z = 0.0f;
      this->interact.w = 0.0f;
      break;
    }

    case ekg::platform_event_type::finger_motion: {
      this->has_motion = true;
      this->interact.x = io_event_serialized.finger_x * static_cast<float>(ekg::ui::width);
      this->interact.y = io_event_serialized.finger_y * static_cast<float>(ekg::ui::height);

      this->interact.z = (io_event_serialized.finger_dx * (static_cast<float>(ekg::ui::width) / 9.0f));
      this->interact.w = (io_event_serialized.finger_dy * static_cast<float>(ekg::ui::height) / 9.0f);

      float swipe_factor = 0.01f;

      this->callback(
        "finger-swipe",
        (this->interact.w > swipe_factor || this->interact.w < -swipe_factor) ||
        (this->interact.z > swipe_factor || this->interact.z < -swipe_factor)
      );

      this->callback("finger-swipe-up", this->interact.w > swipe_factor);
      this->callback("finger-swipe-down", this->interact.w < -swipe_factor);

      this->finger_swipe_event = true;
      ekg::reset(this->timing_last_interact);
      break;
    }
  }

  if (this->has_motion && !this->double_click_mouse_buttons_pressed.empty()) {
    for (const std::string &button: this->double_click_mouse_buttons_pressed) {
      this->callback(button, false);
    }

    this->double_click_mouse_buttons_pressed.clear();
  }
}

void ekg::service::input::on_update() {
  if (this->was_wheel) {
    this->callback("mouse-wheel", false);
    this->callback("mouse-wheel-up", false);
    this->callback("mouse-wheel-down", false);
    this->was_wheel = false;
  }

  if (this->finger_swipe_event) {
    this->callback("finger-swipe", false);
    this->callback("finger-swipe-up", false);
    this->callback("finger-swipe-down", false);
    this->finger_swipe_event = false;
  }

  this->finger_hold_event = false;

  if (this->is_special_keys_released) {
    for (std::string &units : this->special_keys_unit_pressed) {
      this->callback(units, false);
    }

    this->special_keys_unit_pressed.clear();
    this->is_special_keys_released = false;
  }

  if (!this->input_released_list.empty()) {
    for (std::string &inputs: this->input_released_list) {
      this->callback(inputs, false);
    }

    this->input_released_list.clear();
  }

  if (!this->immediate_register_list.empty()) {
    for (std::string &inputs: this->immediate_register_list) {
      this->input_register_map[inputs] = false;
    }

    this->immediate_register_list.clear();
  }
}

void ekg::service::input::bind(std::string_view input_tag, std::string_view key) {
  bool should_bind {true};
  auto &bind_list = this->input_bind_map[key.data()];

  for (std::string &inputs: bind_list) {
    if (inputs == input_tag) {
      should_bind = false;
    }
  }

  if (should_bind) {
    bind_list.emplace_back(input_tag.data());
  }
}

void ekg::service::input::unbind(std::string_view input_tag, std::string_view key) {
  auto &bind_list = this->input_bind_map[key.data()];
  for (uint64_t it {}; it < bind_list.size(); it++) {
    if (bind_list[it] == input_tag) {
      bind_list.erase(bind_list.cbegin() + it);
      break;
    }
  }
}

void ekg::service::input::callback(std::string_view key, bool callback) {
  auto &bind_map {this->input_bind_map[key.data()]};
  this->input_map[key.data()] = callback;

  if (!this->input_register_map.empty()) {
    this->input_register_callback.clear();
  }

  for (std::string &binds : bind_map) {
    this->input_register_map[binds] = callback;
    if (callback) {
      this->input_register_callback.push_back(binds);
    }
  }
}

void ekg::service::input::complete_with_units(std::string &string_builder, std::string_view key_name) {
  string_builder += this->special_keys[static_cast<uint64_t>(ekg::special_key::left_ctrl)];
  string_builder += this->special_keys[static_cast<uint64_t>(ekg::special_key::right_ctrl)];
  string_builder += this->special_keys[static_cast<uint64_t>(ekg::special_key::left_shift)];
  string_builder += this->special_keys[static_cast<uint64_t>(ekg::special_key::right_shift)];
  string_builder += this->special_keys[static_cast<uint64_t>(ekg::special_key::left_alt)];
  string_builder += this->special_keys[static_cast<uint64_t>(ekg::special_key::right_alt)];
  string_builder += this->special_keys[static_cast<uint64_t>(ekg::special_key::tab)];
  string_builder += key_name;
}

void ekg::service::input::fire(std::string_view key) {
  this->input_register_map[key.data()] = true;
  this->immediate_register_list.emplace_back(key);
}

bool ekg::service::input::contains_unit(std::string_view label) {
  for (std::string &units : this->special_keys_unit_pressed) {
    if (units == label) {
      return true;
    }
  }

  return false;
}

bool ekg::service::input::pressed(std::string_view key) {
  return this->input_register_map[key.data()];
}

bool ekg::service::input::receive(std::string_view key) {
  return this->input_map[key.data()];
}