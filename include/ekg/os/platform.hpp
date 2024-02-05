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

#include "ekg_sdl.hpp"

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

  namespace os {
    class platform {
    public:
      int32_t monitor_resolution[2] {};
    public:
      virtual void init() {};
      virtual void quit() {};
      virtual void update_monitor_resolution() {};
      virtual void update_cursor(ekg::system_cursor system_cursor) {};
    };
  };

  extern ekg::system_cursor cursor;
}

#endif