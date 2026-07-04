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
#include <string>
#include <algorithm>

#include "ekg/handler/input/handler.hpp"
#include "ekg/core/runtime.hpp"
#include "ekg/core/context.hpp"
#include "ekg/io/log.hpp"

void ekg::handler::input::init() {
  ekg::log() << "Initialising input-handler binding system-based";

  /**
   * Forced null termination at end due the necessary optmization,
   * when changing dynamically the special key state.
   * 
   * If lshift is pressed, then the last char is set to `+`,
   * or be `lshift+` (from `lshift\0` to `lshift+`).
   * Unpressed special key result in `\0.
   **/

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

  ekg::log() << "Initialising default user-input bindings";

  /**
   * https://github.com/vokegpu/ekg-docs/blob/master/model/input-binding-tag-style.md
   * 
   * Need some ref here.
   **/

  this->insert_input_bind("frame-drag", "mouse-1");
  this->insert_input_bind("frame-drag", "finger-click");
  this->insert_input_bind("frame-resize", "mouse-1");
  this->insert_input_bind("frame-resize", "finger-click");

  this->insert_input_bind("button-active", "mouse-1");
  this->insert_input_bind("button-active", "finger-click");

  this->insert_input_bind("checkbox-active", "mouse-1");
  this->insert_input_bind("checkbox-active", "finger-click");

  this->insert_input_bind("popup-active", "mouse-1");
  this->insert_input_bind("popup-active", "finger-click");

  this->insert_input_bind("textbox-focus", "mouse-1");
  this->insert_input_bind("textbox-focus", "finger-click");

  this->insert_input_bind("textbox-action-select-all", "lctrl+a");
  this->insert_input_bind("textbox-action-select-all", "rctrl+a");

  this->insert_input_bind("textbox-action-select-all-inline", "mouse-1");

  this->insert_input_bind("textbox-action-select", "rshift");
  this->insert_input_bind("textbox-action-select", "rshift");

  this->insert_input_bind("textbox-action-select-word", "mouse-1-double");
  this->insert_input_bind("textbox-action-select-word", "finger-hold");

  this->insert_input_bind("textbox-action-erase-left", "abs-backspace");
  this->insert_input_bind("textbox-action-erase-right", "abs-delete");
  this->insert_input_bind("textbox-action-break-line", "return");
  this->insert_input_bind("textbox-action-break-line", "keypad enter");
  this->insert_input_bind("textbox-action-break-line", "lshift+return");
  this->insert_input_bind("textbox-action-break-line", "rshift+return");
  this->insert_input_bind("textbox-action-tab", "tab");

  this->insert_input_bind("textbox-action-cursor", "abs-mouse-1");
  this->insert_input_bind("textbox-action-multicursor", "lctrl+mouse-1");
  this->insert_input_bind("textbox-action-multicursor", "rctrl+mouse-1");
  this->insert_input_bind("textbox-action-select", "lshift");
  this->insert_input_bind("textbox-action-select", "rshift");
  this->insert_input_bind("textbox-action-modifier", "lctrl");
  this->insert_input_bind("textbox-action-modifier", "rctrl");

  this->insert_input_bind("textbox-action-up", "abs-up");
  this->insert_input_bind("textbox-action-down", "abs-down");
  this->insert_input_bind("textbox-action-right", "abs-right");
  this->insert_input_bind("textbox-action-left", "abs-left");

  this->insert_input_bind("clipboard-copy", "lctrl+c");
  this->insert_input_bind("clipboard-copy", "rctrl+c");
  this->insert_input_bind("clipboard-copy", "copy");
  this->insert_input_bind("clipboard-paste", "lctrl+v");
  this->insert_input_bind("clipboard-paste", "rctrl+v");
  this->insert_input_bind("clipboard-paste", "paste");
  this->insert_input_bind("clipboard-cut", "lctrl+x");
  this->insert_input_bind("clipboard-cut", "rctrl+x");
  this->insert_input_bind("clipboard-cut", "cut");

  this->insert_input_bind("listbox-action-open", "mouse-1-double");
  this->insert_input_bind("listbox-action-open", "finger-hold");

  this->insert_input_bind("listbox-active", "mouse-1");
  this->insert_input_bind("listbox-action-select", "finger-click");
  this->insert_input_bind("listbox-action-select-many", "lctrl+mouse-1");
  this->insert_input_bind("listbox-action-select-many", "rctrl+mouse-1");

  this->insert_input_bind("slider-drag", "mouse-1");
  this->insert_input_bind("slider-drag", "finger-click");
  this->insert_input_bind("slider-bar-increase", "mouse-wheel-up");
  this->insert_input_bind("slider-bar-decrease", "mouse-wheel-down");
  this->insert_input_bind("slider-bar-modifier", "lctrl");
  this->insert_input_bind("slider-bar-modifier", "rctrl");

  this->insert_input_bind("scrollbar-drag", "mouse-1");
  this->insert_input_bind("scrollbar-drag", "finger-click");
  this->insert_input_bind("scrollbar-scroll", "mouse-wheel");
  this->insert_input_bind("scrollbar-scroll", "finger-swipe");
  this->insert_input_bind("scrollbar-scroll-horizontal", "lshift+mouse-wheel");
  this->insert_input_bind("scrollbar-scroll-horizontal", "rshift+mouse-wheel");
}

void ekg::handler::input::quit() {
  ekg::log() << "Quitting input-service";
}

void ekg::handler::input::poll_event() {
  if (this->input.was_wheel) {
    this->set_input_state("mouse-wheel", false);
    this->set_input_state("mouse-wheel-up", false);
    this->set_input_state("mouse-wheel-down", false);
    this->input.was_wheel = false;
  }

  if (this->finger_swipe_event) {
    this->set_input_state("finger-swipe", false);
    this->set_input_state("finger-swipe-up", false);
    this->set_input_state("finger-swipe-down", false);
    this->finger_swipe_event = false;
  }

  this->finger_hold_event = false;

  if (this->is_special_keys_released) {
    for (std::string &units : this->special_keys_unit_pressed) {
      this->set_input_state(units, false);
    }

    this->special_keys_unit_pressed.clear();
    this->is_special_keys_released = false;
  }

  if (!this->input_released_list.empty()) {
    for (std::string &inputs: this->input_released_list) {
      this->set_input_state(inputs, false);
    }

    this->input_released_list.clear();
  }

  if (!this->just_fired_input_bind.empty()) {
    for (bool *p_input_bind_state_address : this->just_fired_input_bind) {
      if (!p_input_bind_state_address) {
        continue;
      }

      *p_input_bind_state_address = false;
    }

    this->just_fired_input_bind.clear();
  }

  this->input.was_pressed = false;
  this->input.was_released = false;
  this->input.has_motion = false;
  this->input.was_typed = false;

  float wheel_precise_interval {};
  ekg::io::event_t &platform_event {
    ekg::p_core->p_platform_base->event
  };

  switch (platform_event.type) {
    default:
      break;

    case ekg::io::event_type::text_input: {
      this->input.was_pressed = true;
      this->input.was_typed = true;
      this->input.typed = platform_event.text_input;
      break;
    }

    case ekg::io::event_type::key_down: {
      this->input.was_pressed = true;

      this->string_builder.clear();

      ekg::p_core->p_platform_base->get_key_name(
        platform_event.key,
        this->key_name
      );

      ekg::special_key special_key {ekg::special_key::unknown};
      ekg::p_core->p_platform_base->get_special_key(platform_event.key, special_key);

      if (special_key != ekg::special_key::unknown) {
        this->special_keys[static_cast<uint64_t>(special_key)][0] = this->key_name[0];

        this->string_builder += "abs-";
        this->string_builder += this->key_name;
        this->set_input_state(this->string_builder, true);
        this->input_released_list.push_back(this->string_builder);

        this->string_builder.clear();
        this->string_builder += this->key_name;
        this->set_input_state(this->string_builder, true);
        this->is_special_keys_released = true;
      } else {
        std::transform(this->key_name.begin(), this->key_name.end(), this->key_name.begin(), ::tolower);
        this->string_builder += "abs-";
        this->string_builder += this->key_name;
        this->set_input_state(this->string_builder, true);
        this->input_released_list.push_back(this->string_builder);

        this->string_builder.clear();
        this->complete_with_units(this->string_builder, this->key_name);
        this->set_input_state(this->string_builder, true);
        this->input_released_list.push_back(this->string_builder);

        if (this->string_builder != this->key_name && !this->contains_unit(this->string_builder)) {
          this->special_keys_unit_pressed.push_back(this->string_builder);
        }
      }

      break;
    }

    case ekg::io::event_type::key_up: {
      this->input.was_released = true;

      ekg::p_core->p_platform_base->get_key_name(
        platform_event.key,
        this->key_name
      );

      ekg::special_key special_key {ekg::special_key::unknown};
      ekg::p_core->p_platform_base->get_special_key(platform_event.key, special_key);

      if (special_key != ekg::special_key::unknown) {
        this->special_keys[static_cast<uint64_t>(special_key)][0] = '\0';

        this->string_builder = "abs-";
        this->string_builder += this->key_name;
        this->set_input_state(this->string_builder, true);
        this->input_released_list.push_back(this->string_builder);

        this->string_builder = this->key_name;
        this->set_input_state(this->string_builder, false);
        this->is_special_keys_released = true;

        this->string_builder += "-up";
        this->set_input_state(this->string_builder, true);
      } else {
        std::transform(this->key_name.begin(), this->key_name.end(), this->key_name.begin(), ::tolower);
        this->string_builder = "abs-";
        this->string_builder += this->key_name;
        this->string_builder += "-up";
        this->set_input_state(this->string_builder, true);
        this->input_released_list.push_back(this->string_builder);

        this->string_builder.clear();
        this->complete_with_units(this->string_builder, this->key_name);
        this->string_builder += "-up";
        this->set_input_state(this->string_builder, true);
        this->input_released_list.push_back(this->string_builder);
      }
      
      break;
    }

    case ekg::io::event_type::mouse_button_down: {
      this->input.was_pressed = true;
      this->key_name = "mouse";

      this->string_builder = "abs-";
      this->string_builder += this->key_name;
      this->set_input_state(this->string_builder, true);
      this->input_released_list.push_back(this->string_builder);

      this->set_input_state(this->key_name, true);
      this->input_released_list.push_back(this->key_name);

      this->key_name = "mouse-";
      this->key_name += std::to_string(platform_event.mouse_button);

      this->string_builder = "abs-";
      this->string_builder += this->key_name;
      this->set_input_state(this->string_builder, true);
      this->input_released_list.push_back(this->string_builder);

      this->string_builder.clear();
      this->complete_with_units(this->string_builder, this->key_name);
      this->set_input_state(this->string_builder, true);
      this->input_released_list.push_back(this->string_builder);

      bool double_click_factor {ekg::reach(this->double_interact, 500)};
      if (!double_click_factor) {
        this->string_builder += "-double";
        this->set_input_state(this->string_builder, true);

        this->string_builder = "abs-" + this->string_builder;
        this->set_input_state(this->string_builder, true);
        this->input_released_list.push_back(this->string_builder);

        this->double_click_mouse_buttons_pressed.push_back(this->string_builder);
        this->input_released_list.push_back(this->string_builder);
      }

      if (double_click_factor) {
        ekg::reset(this->double_interact);
      }

      break;
    }

    case ekg::io::event_type::mouse_button_up: {
      this->input.was_released = true;
      this->key_name = "mouse-up";

      this->string_builder = "abs-";
      this->string_builder += this->key_name;
      this->set_input_state(this->string_builder, true);
      this->input_released_list.push_back(this->string_builder);

      this->set_input_state(this->key_name, true);
      this->input_released_list.push_back(this->key_name);

      this->key_name = "mouse-";
      this->key_name += std::to_string(platform_event.mouse_button);

      this->string_builder = "abs-";
      this->string_builder += this->key_name;
      this->string_builder += "-up";
      this->set_input_state(this->string_builder, true);
      this->input_released_list.push_back(this->string_builder);

      this->string_builder.clear();
      this->complete_with_units(this->string_builder, this->key_name);
      this->string_builder += "-up";

      this->set_input_state(this->string_builder, true);
      this->input_released_list.push_back(this->string_builder);
      break;
    }

    case ekg::io::event_type::mouse_motion: {
      this->input.has_motion = true;
      this->input.interact.x = static_cast<float>(platform_event.mouse_motion_x);
      this->input.interact.y = static_cast<float>(platform_event.mouse_motion_y);
      break;
    }

    case ekg::io::event_type::mouse_wheel: {
      this->string_builder.clear();
      this->complete_with_units(this->string_builder, "mouse-wheel");
      this->set_input_state(this->string_builder, true);
      this->input_released_list.push_back(this->string_builder);
      this->input.was_wheel = true;

      this->set_input_state("mouse-wheel-up", platform_event.mouse_wheel_y > 0);
      this->set_input_state("mouse-wheel-down", platform_event.mouse_wheel_y < 0);
      this->set_input_state("mouse-wheel-right", platform_event.mouse_wheel_x > 0);
      this->set_input_state("mouse-wheel-left", platform_event.mouse_wheel_x < 0);

      /**
       * I do not know how actually implement smooth scroll,
       * SDL/GLFW does not give an option to receive a precise smooth scrolling intensity.
       * 
       * So the interval between old event and new event, inverse to, 1000, returns
       * the intensity.
       **/

      wheel_precise_interval = static_cast<float>(
        1000 - ekg::clamp<int64_t>(ekg::interval(this->last_time_wheel_was_fired), 0, 1000)
      );

      wheel_precise_interval = (wheel_precise_interval / 1000.0f);
      wheel_precise_interval = wheel_precise_interval + (static_cast<float>(wheel_precise_interval > 0.99) * 0.5f);
      wheel_precise_interval = ekg::clamp_min<float>(wheel_precise_interval, 0.2f);

      bool cooldown {ekg::reach(this->cooldown_wheel, 350)};
      ekg::reset(this->cooldown_wheel);

      if (
        !cooldown
        &&
        (
          (platform_event.mouse_wheel_precise_x < 0 && this->input.interact.z < 0)
          ||
          (platform_event.mouse_wheel_precise_x > 0 && this->input.interact.z > 0)
        )
      ) {
        this->input.interact.z += platform_event.mouse_wheel_precise_x * wheel_precise_interval * 0.5f;       
      } else {
        this->input.interact.z = platform_event.mouse_wheel_precise_x * wheel_precise_interval;        
      }

      if (
        !cooldown
        &&
        (
          (platform_event.mouse_wheel_precise_y < 0 && this->input.interact.w < 0)
          ||
          (platform_event.mouse_wheel_precise_y > 0 && this->input.interact.w > 0)
        )
      ) {
        this->input.interact.w += platform_event.mouse_wheel_precise_y * wheel_precise_interval * 0.5f; 
      } else {
        this->input.interact.w = platform_event.mouse_wheel_precise_y * wheel_precise_interval;        
      }

      ekg::reset(this->last_time_wheel_was_fired);
      break;
    }

    case ekg::io::event_type::finger_down: {
      this->input.was_pressed = true;
      ekg::reset(this->input.timing_last_interact);
      bool reach_double_interact {ekg::reach(this->double_interact, 500)};

      this->input.interact.x = platform_event.finger_x * static_cast<float>(ekg::dpi.viewport.w);
      this->input.interact.y = platform_event.finger_y * static_cast<float>(ekg::dpi.viewport.h);

      this->set_input_state("finger-click", true);
      this->set_input_state("finger-click-double", !reach_double_interact);

      if (reach_double_interact) {
        ekg::reset(this->double_interact);
      }

      break;
    }

    case ekg::io::event_type::finger_up: {
      this->input.was_released = true;
      this->set_input_state("finger-hold", (this->finger_hold_event = ekg::reach(this->input.timing_last_interact, 750)));
      this->set_input_state("finger-click", false);
      this->set_input_state("finger-click-double", false);

      /**
       * Should stop the swipe event when there is no finger touching on screen.
       **/
      this->set_input_state("finger-swipe", false);
      this->set_input_state("finger-swipe-up", false);
      this->set_input_state("finger-swipe-down", false);

      this->input.interact.x = platform_event.finger_x * static_cast<float>(ekg::dpi.viewport.w);
      this->input.interact.y = platform_event.finger_y * static_cast<float>(ekg::dpi.viewport.h);

      this->input.interact.z = 0.0f;
      this->input.interact.w = 0.0f;
      break;
    }

    case ekg::io::event_type::finger_motion: {
      this->input.has_motion = true;
      this->input.interact.x = platform_event.finger_x * static_cast<float>(ekg::dpi.viewport.w);
      this->input.interact.y = platform_event.finger_y * static_cast<float>(ekg::dpi.viewport.h);

      this->input.interact.z = (platform_event.finger_dx * (static_cast<float>(ekg::dpi.viewport.w) / 9.0f));
      this->input.interact.w = (platform_event.finger_dy * static_cast<float>(ekg::dpi.viewport.h) / 9.0f);

      float swipe_factor = 0.01f;

      this->set_input_state(
        "finger-swipe",
        (this->input.interact.w > swipe_factor || this->input.interact.w < -swipe_factor) ||
        (this->input.interact.z > swipe_factor || this->input.interact.z < -swipe_factor)
      );

      this->set_input_state("finger-swipe-up", this->input.interact.w > swipe_factor);
      this->set_input_state("finger-swipe-down", this->input.interact.w < -swipe_factor);

      this->finger_swipe_event = true;
      ekg::reset(this->input.timing_last_interact);
      break;
    }
  }

  if (this->input.has_motion && !this->double_click_mouse_buttons_pressed.empty()) {
    for (const std::string &button: this->double_click_mouse_buttons_pressed) {
      this->set_input_state(button, false);
    }

    this->double_click_mouse_buttons_pressed.clear();
  }
}

void ekg::handler::input::update() {
  #if defined(EKG_INPUT_DEBUG)
    ekg::log()
      << "scroll_speed: " << this->input.scroll_speed           << "\n"
      << "interact: "     << (std::string) this->input.interact << "\n"
      << "was_pressed: "  << this->input.was_pressed            << "\n"
      << "was_released: " << this->input.was_released           << "\n"
      << "has_motion: "   << this->input.has_motion             << "\n"
      << "was_wheel: "    << this->input.was_wheel              << "\n"
      << "was_typed: "    << this->input.was_typed;
  #endif

  ekg::reset_if_reach(this->input.ui_timing, 1000);
  ekg::timing_t::second = this->input.ui_timing.current_ticks;
}

void ekg::handler::input::insert_input_bind(
  std::string_view tag,
  std::string_view input
) {
  std::vector<bool*> &bind_list {this->input_bindings_map[input.data()]};
  ekg::input_bind_t &input_bind {this->input_bind_map[tag.data()]};

  bool *p_address {&input_bind.state};
  bool must_bind {true};
  
  for (bool *p_binded_address : bind_list) {
    if (p_binded_address == p_address) {
      must_bind = false;
      break;
    }
  }

  if (must_bind) {
    bind_list.emplace_back(p_address);
    input_bind.registry.emplace_back(input.data());
  }
}

void ekg::handler::input::erase_input_bind(
  std::string_view tag,
  std::string_view input
) {
  std::vector<bool*> &bind_list {this->input_bindings_map[input.data()]};
  ekg::input_bind_t &input_bind {this->input_bind_map[tag.data()]};

  bool *p_address {&input_bind.state};
  bool was_erased {};

  for (size_t it {}; it < bind_list.size(); it++) {
    if (bind_list.at(it) == p_address) {
      bind_list.erase(bind_list.begin() + it);
      was_erased = true;
      break;
    }
  }

  if (!was_erased) {
    return;
  }

  for (size_t it {}; it < input_bind.registry.size(); it++) {
    if (input_bind.registry.at(it) == input) {
      input_bind.registry.erase(input_bind.registry.begin() + it);
      break;
    }
  }
}

void ekg::handler::input::erase_input_bind(
  std::string_view tag
) {
  ekg::input_bind_t &input_bind {this->input_bind_map[tag.data()]};
  bool *p_address {&input_bind.state};

  for (size_t it {}; it < input_bind.registry.size(); it++) {
    std::string &input {input_bind.registry.at(it)};
    std::vector<bool*> &bind_list {
      this->input_bindings_map[input]
    };

    for (size_t it_bind_list {}; it_bind_list < bind_list.size(); it_bind_list++) {
      if (bind_list.at(it_bind_list) == p_address) {
        bind_list.erase(bind_list.begin() + it_bind_list);
        break;
      }
    }
  }

  // input_bind.registry.clear();
  this->input_bind_map.erase(tag.data());
} 

void ekg::handler::input::set_input_state(
  std::string_view key,
  bool state
) {
  this->input_map[key.data()] = state;

  if (this->input_bind_listener_function) {
    this->input_bind_listener_function(key, state);
  }

  for (bool *p_address : this->input_bindings_map[key.data()]) {
    if (!p_address) {
      continue;
    }

    *p_address = state;
  }
}

void ekg::handler::input::complete_with_units(
  std::string &string_builder,
  std::string_view key_name
) {
  string_builder += this->special_keys[static_cast<uint64_t>(ekg::special_key::left_ctrl)];
  string_builder += this->special_keys[static_cast<uint64_t>(ekg::special_key::right_ctrl)];
  string_builder += this->special_keys[static_cast<uint64_t>(ekg::special_key::left_shift)];
  string_builder += this->special_keys[static_cast<uint64_t>(ekg::special_key::right_shift)];
  string_builder += this->special_keys[static_cast<uint64_t>(ekg::special_key::left_alt)];
  string_builder += this->special_keys[static_cast<uint64_t>(ekg::special_key::right_alt)];
  string_builder += this->special_keys[static_cast<uint64_t>(ekg::special_key::tab)];
  string_builder += key_name;
}

void ekg::handler::input::set_input_bind_state(
  std::string_view key,
  bool state
) {
  ekg::input_bind_t &input_bind {
    this->input_bind_map[key.data()]
  };

  if (!input_bind.p_address) {
    return;
  }

  *input_bind.p_address = state; 
  this->just_fired_input_bind.emplace_back(input_bind.p_address);
}

bool ekg::handler::input::contains_unit(
  std::string_view label
) {
  for (std::string &units : this->special_keys_unit_pressed) {
    if (units == label) {
      return true;
    }
  }

  return false;
}

bool ekg::handler::input::get_input_state(
  std::string_view input
) {
  return this->input_map[input.data()];
}

bool ekg::handler::input::get_input_bind_state(
  std::string_view tag
) {
  return this->input_bind_map[tag.data()].state;
}

void ekg::handler::input::set_input_bind_listener(
  ekg::input_bind_function_t input_bind_listener_function
) {
  this->input_bind_listener_function = input_bind_listener_function;
}
