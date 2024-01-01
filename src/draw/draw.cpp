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

#include "ekg/draw/draw.hpp"
#include "ekg/ekg.hpp"

void ekg::draw::rect(const ekg::rect &rect, const ekg::vec4 &color, int32_t line_thickness) {
  ekg::draw::rect(rect.x, rect.y, rect.w, rect.h, color, line_thickness);
}

void ekg::draw::rect(float x, float y, float w, float h, const ekg::vec4 &color, int32_t line_thickness) {
  if (color.w < 0.1f) {
    return;
  }

  ekg::gpu::data &data {ekg::core->gpu_allocator.bind_current_data()};

  data.shape_rect[0] = x;
  data.shape_rect[1] = y;
  data.shape_rect[2] = w;
  data.shape_rect[3] = h;

  data.material_color[0] = color.x;
  data.material_color[1] = color.y;
  data.material_color[2] = color.z;
  data.material_color[3] = color.w;
  data.line_thickness = line_thickness;
  data.factor = 1;

  ekg::core->gpu_allocator.dispatch();
}

void ekg::draw::sync_scissor(ekg::rect &child_rect, int32_t mother_parent_id) {
  ekg::core->gpu_allocator.sync_scissor(child_rect, mother_parent_id);
}

void ekg::draw::bind_scissor(int32_t id) {
  ekg::core->gpu_allocator.bind_scissor(id);
}

void ekg::draw::bind_off_scissor() {
  ekg::core->gpu_allocator.bind_off_scissor();
}

bool ekg::draw::is_visible(int32_t scissor_id, ekg::vec4 &interact) {
  auto scissor {ekg::core->gpu_allocator.get_scissor_by_id(scissor_id)};
  return scissor != nullptr &&
         ekg::rect_collide_vec({scissor->rect[0], scissor->rect[1], scissor->rect[2], scissor->rect[3]}, interact);
}

void ekg::draw::get_visible(int32_t scissor_id, ekg::rect &rect) {
  auto scissor {ekg::core->gpu_allocator.get_scissor_by_id(scissor_id)};
  if (scissor != nullptr) {
    rect.x = scissor->rect[0];
    rect.y = scissor->rect[1];
    rect.w = scissor->rect[2];
    rect.h = scissor->rect[3];
  }
}