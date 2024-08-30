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

#ifndef EKG_UI_SLIDER_WIDGET_H
#define EKG_UI_SLIDER_WIDGET_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include "ekg/ui/slider/ui_slider.hpp"
#include "ekg/draw/font_renderer.hpp"

namespace ekg::ui {
  class slider_widget : public ekg::ui::abstract_widget {
  public:
    struct range {
    public:
      ekg::rect rect {};
      ekg::rect target {};
      ekg::rect rect_text {};
      std::string text {};
      ekg::font font_size {};
    };
  public:
    std::vector<ekg::ui::slider_widget::range> range_list {};
    uint64_t targetted_range_index {};
  public:
    void on_reload() override;

    void on_pre_event(ekg::os::io_event_serial &io_event_serial) override;

    void on_event(ekg::os::io_event_serial &io_event_serial) override;

    void on_draw_refresh() override;
  };
}

namespace ekg::ui {
  /**
   * Calculate the target (value) position from widget dimension.
   * Based on all supported number formats.
   **/
  float slider_widget_calculate_target_pos(
    ekg::ui::slider *&p_ui,
    ekg::number number,
    float dimension,
    uint64_t index
  );

  /**
   * Calculate the value by drag factor.
   * Based on all supported number formats.
   **/
  void slider_widget_calculate_value(
    ekg::ui::slider *&p_ui,
    ekg::number number,
    float factor,
    float dimension,
    uint64_t index
  );

  /**
   * Returns the valeu label in string.
   * Based on all supported number formats and precision (`ekg::float32`, `ekg::float64`).
   **/
  void slider_widget_get_value_label(
    ekg::ui::slider *&p_ui,
    ekg::number number,
    uint64_t index,
    std::string &content
  );

  /**
   * Returns both (min, max) text width metrics.
   * Based on all supported number formats.
   **/
  void slider_widget_get_metrics(
    ekg::ui::slider *&p_ui,
    ekg::number number,
    uint64_t index,
    ekg::draw::font_renderer &f_renderer,
    float *p_min_text_width,
    float *p_max_text_width
  );
}

#endif