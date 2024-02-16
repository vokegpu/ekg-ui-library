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

#ifndef EKG_DRAW_H
#define EKG_DRAW_H

#include <vector>
#include <iostream>

#include "ekg/util/geometry.hpp"

#define ekg_draw_assert_scissor() if (ekg::gpu::allocator::is_out_of_scissor) { ekg::core->gpu_allocator.bind_off_scissor();  return;  }

namespace ekg {
  enum draw_mode {
    filled = 0,
    circle = -1,
    outline = 1
  };

  namespace draw {
    void rect(const ekg::rect &rect, const ekg::vec4 &color, int32_t draw_mode = 0);

    void rect(float x, float y, float w, float h, const ekg::vec4 &color, int32_t draw_mode = 0);

    void sync_scissor(ekg::rect &rect, int32_t id);

    void bind_scissor(int32_t id);

    void bind_off_scissor();

    bool is_visible(int32_t id, ekg::vec4 &interact);

    void get_visible(int32_t id, ekg::rect &rect);
  }
}

#endif