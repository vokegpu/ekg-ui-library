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

namespace ekg {
  enum event_type {
    unique = 0,
    alloc = 2,
    allocated = 4,
    shared = 8
  };

  struct task {
  public:
    struct info {
    public:
      std::string_view tag {};
      int32_t id {};
      int32_t type {};
      bool button {};
      bool checkbox {};
      float slider {};
      std::string_view popup {};
      std::string_view checkbox {};
      std::string_view combobox {};
      void *p_data {};
    };
  public:
    std::string_view tag {};
    void *p_data {};
    std::function<void(void*)> function {};
  };
}

#endif