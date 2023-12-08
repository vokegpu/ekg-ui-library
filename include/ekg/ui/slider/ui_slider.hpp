/*
* MIT License
* 
* Copyright (c) 2022-2023 Rina Wilk / vokegpu@gmail.com
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

#ifndef EKG_UI_SLIDER_H
#define EKG_UI_SLIDER_H

#include "ekg/ui/abstract/ui_abstract.hpp"

namespace ekg::ui {
  class slider : public ekg::ui::abstract {
  protected:
    float value {};
    float minimum_value {};
    float maximum_value {};

    int32_t scaled_size {};
    int32_t float_precision {};
    uint16_t bar_flags {};
    uint16_t text_flags {};

    ekg::font font_size {};
    ekg::axis bar_axis {};
    bool dragging {};
  public:
    ekg::ui::slider *set_dragging(bool state);

    bool is_dragging();

    ekg::ui::slider *set_place(uint16_t dock);

    ekg::ui::slider *set_bar_axis(ekg::axis axis);

    ekg::axis get_bar_axis();

    ekg::ui::slider *set_text_align(uint16_t dock);

    uint16_t get_text_align();

    ekg::ui::slider *set_bar_align(uint16_t dock);

    uint16_t get_bar_align();

    ekg::ui::slider *set_font_size(ekg::font font);

    ekg::font get_font_size();

    ekg::ui::slider *set_width(float w);

    float get_width();

    ekg::ui::slider *set_scaled_height(int32_t h);

    int32_t get_scaled_height();

    float get_height();

    ekg::ui::slider *set_value(float val);

    float get_value();

    ekg::ui::slider *set_value_min(float min);

    float get_value_min();

    ekg::ui::slider *set_value_max(float max);

    float get_value_max();

    ekg::ui::slider *set_precision(int32_t precision);

    int32_t get_precision();
  };
}

#endif