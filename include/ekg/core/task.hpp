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

#ifndef EKG_CORE_EVENT_H
#define EKG_CORE_EVENT_H

#include <iostream>
#include <functional>
#include "ekg/os/ekg_sdl.hpp"

namespace ekg {
  enum event_type {
    unique = 0,
    alloc = 2,
    allocated = 4,
    shared = 8
  };

  extern uint32_t listener;

  void dispatch_ui_event(std::string_view tag, std::string_view value, uint16_t type);

  struct event {
  public:
    std::string tag {};
    std::string value {};
    uint16_t type {};
  };

  struct task {
  public:
    const char *p_tag {};
    void *p_callback {};
    std::function<void(void *)> function {};
  };
}

#endif