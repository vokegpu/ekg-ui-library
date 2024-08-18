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

#include "ekg/ui/slider/ui_slider_widget.hpp"
#include "ekg/ui/slider/ui_slider.hpp"
#include "ekg/ekg.hpp"
#include "ekg/draw/draw.hpp"

void ekg::ui::slider_widget::on_reload() {
  abstract_widget::on_reload();

  ekg::ui::slider *p_ui {static_cast<ekg::ui::slider*>(this->p_data)};
  ekg::axis axis {p_ui->get_axis()};
  ekg::number number {p_ui->get_number()};
  ekg::draw::font_renderer &f_renderer {ekg::f_renderer(p_ui->get_font_size())};

  ekg::rect &rect {this->get_abs_rect()};
  ekg::rect relative_rect {};

  float base_text_height {f_renderer.get_text_height()};
  float dimension_offset {static_cast<float>((int32_t) (base_text_height / 2.0f))};
  float offset {ekg::find_min_offset(base_text_height, dimension_offset)};

  switch (axis) {
    case ekg::axis::horizontal: {
      this->dimension.w = ekg_min(this->dimension.w, base_text_height);
      this->dimension.h = (base_text_height + dimension_offset) * static_cast<float>(p_ui->get_scaled_height());

      uint64_t value_list_size {
        ekg::ui::slider_widget_get_range_count(p_ui->get_value(), number)
      };

      this->range_list.resize(value_list_size);
      float range_rect_width {this->dimension.w / static_cast<float>(value_list_size)};

      for (uint64_t it {}; it < value_list_size; it++) {
        ekg::ui::slider_widget::range &range {this->range_list.at(it)};
        
        range.rect.x = relative_rect.x;
        range.rect.y = relative_rect.y;

        range.rect.w = range_rect_width;
        range.rect.h = this->dimension.h;

        relative_rect.w += range_rect_width;
      }

      break;
    }

    case ekg::axis::vertical: {
      break;
    }
  }
}

void ekg::ui::slider_widget::on_pre_event(ekg::os::io_event_serial &io_event_serial) {
  abstract_widget::on_pre_event(io_event_serial);

  this->flag.state = (
    this->flag.hovered && ekg::input::action("slider-bar-modifier") &&
    (ekg::input::action("slider-bar-increase") || ekg::input::action("slider-bar-decrease"))
  );

  this->flag.absolute = this->flag.state;
}

void ekg::ui::slider_widget::on_event(ekg::os::io_event_serial &io_event_serial) {

}

void ekg::ui::slider_widget::on_draw_refresh() {
  ekg::rect &rect {this->get_abs_rect()};
  ekg::service::theme_scheme_t &theme_scheme {ekg::current_theme_scheme()};

  ekg::draw::sync_scissor(this->scissor, rect, this->p_parent_scissor);
  ekg_draw_assert_scissor();

  for (ekg::ui::slider_widget::range &range : this->range_list) {
    ekg::draw::rect(
      range.rect + rect,
      theme_scheme.slider_background,
      ekg::draw_mode::outline,
      ekg_layer(ekg::layer::background)
    );
  }

  ekg::draw::rect(
    rect,
    theme_scheme.slider_outline,
    ekg::draw_mode::outline
  );
}

uint64_t ekg::ui::slider_widget_get_range_count(ekg::feature *&p_feature, ekg::number number) {
  switch (number) {
  case ekg::number::float64:
    return static_cast<ekg::ui::slider::serializer_t<double>*>(p_feature)->value_list.size();
  case ekg::number::float32:
    return static_cast<ekg::ui::slider::serializer_t<float>*>(p_feature)->value_list.size();
  case ekg::number::int64:
    return static_cast<ekg::ui::slider::serializer_t<int64_t>*>(p_feature)->value_list.size();
  case ekg::number::uint64:
    return static_cast<ekg::ui::slider::serializer_t<uint64_t>*>(p_feature)->value_list.size();
  case ekg::number::int32:
    return static_cast<ekg::ui::slider::serializer_t<int32_t>*>(p_feature)->value_list.size();
  case ekg::number::uint32:
    return static_cast<ekg::ui::slider::serializer_t<uint32_t>*>(p_feature)->value_list.size();
  case ekg::number::int16:
    return static_cast<ekg::ui::slider::serializer_t<int16_t>*>(p_feature)->value_list.size();
  case ekg::number::uint16:
    return static_cast<ekg::ui::slider::serializer_t<uint16_t>*>(p_feature)->value_list.size();
  case ekg::number::int8:
    return static_cast<ekg::ui::slider::serializer_t<int8_t>*>(p_feature)->value_list.size();
  case ekg::number::uint8:
    return static_cast<ekg::ui::slider::serializer_t<uint8_t>*>(p_feature)->value_list.size();
  }

  return 0;
}