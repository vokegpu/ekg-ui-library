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

#ifndef EKG_LAYOUT_H
#define EKG_LAYOUT_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include <map>

namespace ekg::service {
  class layout {
  protected:
    float min_offset {};
    float min_fill_width {};
    float min_height {};
    float scaled_width_divided {};
    float scaled_height_divided {};
    float respective_mask_all {};
    float respective_mask_center {};
    float extent_data[4] {};

    int32_t min_factor_height {};
    uint16_t enum_docks_flag {};
    uint16_t layout_mask_flags {};

    ekg::axis dock_axis_mask {};
    ekg::vec3 offset_mask {};
    std::vector<ekg::dock_rect> dock_rect_list {};
    ekg::rect layout_mask {};

    ekg::vec2 viewport_scale {0.0f, 0.0f};
    float scale_factor {};
  public:
    void init();

    void quit();

    void set_preset_mask(const ekg::vec3 &, ekg::axis, float = 0.0f);

    void insert_into_mask(const ekg::dock_rect &);

    void process_layout_mask();

    ekg::rect &get_layout_mask();

    float get_min_offset();

    float get_respective_mask_size();

    void update_scale_factor();

    float get_scale_factor();

    float get_dimensional_extent(
      ekg::ui::abstract_widget *widget,
      uint16_t flag_ok,
      uint16_t flag_stop,
      int64_t &begin_and_count,
      ekg::axis axis
    );

    void process_scaled(ekg::ui::abstract_widget *widget_parent);
  };
}

#endif