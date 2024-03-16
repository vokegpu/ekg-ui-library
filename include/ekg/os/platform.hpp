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

#ifndef EKG_OS_PLATFORM_H
#define EKG_OS_PLATFORM_H

#include <cstdint>  
#include <string_view>

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

  extern ekg::system_cursor cursor;
}

namespace ekg::os {
  struct io_event_serial {
  public:
    bool is_text_input {};
    std::string_view text_input {};

    bool is_mouse_button_up {};
    bool is_mouse_button_down {};
    uint8_t mouse_button {};

    bool is_mouse_motion {};
    bool is_mouse_wheel {};

    int32_t mouse_motion_x {};
    int32_t mouse_motion_y {};

    int32_t mouse_wheel_x {};
    int32_t mouse_wheel_y {};

    float mouse_wheel_precise_x {};
    float mouse_wheel_precise_y {};

    bool is_finger_up {};
    bool is_finger_down {};
    bool is_finger_motion {};

    bool is_key_down {};
    bool is_key_up {};
    int32_t key {};

    float finger_x {};
    float finger_y {};

    float finger_dx {};
    float finger_dy {};
  };

  class platform {
  public:
    int32_t monitor_resolution[2] {};
  public:
    virtual void init() {}
    virtual void quit() {}
    virtual void update_monitor_resolution() {}
    virtual void update_cursor(ekg::system_cursor system_cursor) {}
    virtual void get_key_name(int32_t key, std::string &name) {}
    virtual void get_special_key(int32_t key, ekg::special_key &special_key) {}
    virtual const char *get_clipboard_text() { return nullptr; };
    virtual void set_clipboard_text(const char *p_text) {};
    virtual bool has_clipboard_text() { return false; }
  };
}

#endif