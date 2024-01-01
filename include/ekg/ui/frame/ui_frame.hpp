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

#ifndef EKG_UI_FRAME_H
#define EKG_UI_FRAME_H

#include "ekg/ui/abstract/ui_abstract.hpp"
#include "ekg/util/geometry.hpp"
#include <vector>

namespace ekg::ui {
  class frame : public ekg::ui::abstract {
  protected:
    uint16_t dock_resize {};
    uint16_t dock_drag {};

    ekg::rect rect_initial {};
    std::string tag {};
    ekg::vec2 scale_factor {};
  public:
    ekg::ui::frame *set_place(uint16_t dock);

    ekg::ui::frame *set_scale_factor(float x, float y);

    ekg::vec2 get_scale_factor();

    ekg::ui::frame *set_drag(uint16_t dock);

    uint16_t get_drag_dock();

    ekg::ui::frame *set_resize(uint16_t dock);

    uint16_t get_resize_dock();

    ekg::ui::frame *set_pos_initial(float x, float y);

    ekg::vec2 get_pos_initial();

    ekg::ui::frame *set_size_initial(float x, float y);

    ekg::vec2 get_size_initial();

    ekg::ui::frame *set_initial_width(float w);

    float get_initial_width();

    ekg::ui::frame *set_initial_height(float h);

    float get_initial_height();

    ekg::ui::frame *set_size(float w, float h);

    ekg::vec2 get_size();

    ekg::ui::frame *set_pos(float x, float y);

    ekg::vec2 get_pos();

    ekg::ui::frame *set_width(float w);

    float get_width();

    ekg::ui::frame *set_height(float h);

    float get_height();
  };
}

#endif