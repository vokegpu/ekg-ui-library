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
#ifndef EKG_PLATFORM_HPP
#define EKG_PLATFORM_HPP

#include "ekg/io/event.hpp"
#include <string_view>

namespace ekg::platform {
  class base {
  public:
    ekg::rect_t<int32_t> display_size {};
    ekg::system_cursor system_cursor {};
    ekg::io::event_t event {};
    ekg::flags_t modes {};
  public:
    virtual void init() {};
    virtual void quit() {};
    virtual void update_display_size() {};
    virtual void update() {};
    virtual void get_key_name(ekg::input_key_t &key, std::string &name) {};
    virtual void get_special_key(ekg::input_key_t &key, ekg::special_key &espcial_key) {};
    virtual std::string_view get_clipboard_text() { return ""; };
    virtual void set_clipboard_text(std::string_view text) {};
    virtual bool has_clipboard_text() { return false; };
  };
}

#endif
