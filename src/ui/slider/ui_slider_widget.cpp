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
  ekg::draw::font_renderer &small_f_renderer {ekg::f_renderer(ekg::font::small)};
  ekg::service::theme_scheme_t &theme_scheme {ekg::current_theme_scheme()};
  ekg::feature *&p_feature {p_ui->get_value()};

  ekg::rect &rect {this->get_abs_rect()};
  ekg::rect relative_rect {};
  ekg::flags text_align_flags {p_ui->get_text_align()};

  float base_text_height {f_renderer.get_text_height()};
  float dimension_offset {static_cast<float>((int32_t) (base_text_height / 2.0f))};
  float offset {ekg::find_min_offset(base_text_height, dimension_offset)};

  float text_width {};
  float small_font_text_height {small_f_renderer.get_text_height()};

  float bar_thickness {
    static_cast<float>(theme_scheme.slider_bar_thickness) / 100
  };

  ekg::layout::mask &mask {ekg::core->mask};

  switch (axis) {
    case ekg::axis::horizontal: {
      this->dimension.w = ekg_min(this->dimension.w, base_text_height);
      this->dimension.h = (base_text_height + dimension_offset) * static_cast<float>(p_ui->get_scaled_height());

      uint64_t range_list_size {
        ekg::ui::slider_widget_get_range_count(p_feature, number)
      };

      this->range_list.resize(range_list_size);
      float range_rect_width {this->dimension.w / static_cast<float>(range_list_size)};

      // left or right
      // docknize all

      // left top/bottom or right top/bottom or center top/bottom
      // docknize with small font height

      // center
      // docknize with no small font (normal) height

      // target
      // no-docknize, target the drag cursor but with no small font (normal) height

      // target top/bottom
      // no-docknize, target the drag cursor with small font height

      if (text_align_flags == ekg::dock::left || text_align_flags == ekg::dock::right) {
        mask.preset({0.0f, offset, this->dimension.h}, axis, this->dimension.w);

        for (uint64_t it {}; it < range_list_size; it++) {
          ekg::ui::slider_widget::range &range {this->range_list.at(it)};

          range.text = ekg::ui::slider_widget_get_value_label(
            p_feature,
            number,
            it
          );

          mask.insert({&range.rect, text_align_flags});
        }
      } else {

      }

      for (uint64_t it {}; it < range_list_size; it++) {
        ekg::ui::slider_widget::range &range {this->range_list.at(it)};

        range.rect.x = relative_rect.x;
        range.rect.y = relative_rect.y;
        range.rect.w = range_rect_width;

        range.rect.h = this->dimension.h * bar_thickness;
        range.rect.y = range.rect.y + ((this->dimension.h / 2) - (range.rect.h / 2));

        range.target.x = range.rect.x;
        range.target.y = range.rect.y;
        range.target.h = range.rect.h;
        range.target.w = range_rect_width;

        relative_rect.x += range_rect_width;
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
  bool pressed {ekg::input::pressed()};

  if (!this->flag.activity && pressed && ekg::input::action("slider-drag-activity")) {
    ekg::rect &rect {this->get_abs_rect()};
    ekg::vec4 &interact {ekg::input::interact()};

    for (uint64_t it {}; it < this->range_list.size(); it++) {
      ekg::ui::slider_widget::range &range {this->range_list.at(it)};
      if (ekg::rect_collide_vec(range.rect + rect, interact)) {
        this->flag.activity = true;
        this->targetted_range_index = it;
        break;
      }
    }
  } else if (this->flag.activity && ekg::input::released()) {
    this->flag.activity = false;
    this->targetted_range_index = UINT64_MAX;
  } else if (this->flag.activity && this->targetted_range_index != UINT64_MAX && ekg::input::motion()) {
    ekg::ui::slider_widget::range &range {this->range_list.at(this->targetted_range_index)};
    ekg::ui::slider *p_ui {static_cast<ekg::ui::slider*>(this->p_data)};

    ekg::vec4 &interact {ekg::input::interact()};
    ekg::rect &rect {this->get_abs_rect()};

    ekg::rect bar {range.rect + rect};
    float factor {};
    float dimension {};

    switch (p_ui->get_axis()) {
    case ekg::axis::horizontal:
      factor = ekg_clamp(interact.x - range.rect.x, 0.0f, range.rect.w);
      dimension = range.rect.w;
      break;
    case ekg::axis::vertical:
      factor = ekg_clamp(interact.y - range.rect.y, 0.0f, range.rect.h);
      dimension = range.rect.h;
      break;
    }

    ekg::ui::slider_widget_calculate_value(
      p_ui->get_value(),
      p_ui->get_number(),
      factor,
      dimension,
      this->targetted_range_index
    );
  }
}

void ekg::ui::slider_widget::on_draw_refresh() {
  ekg::rect &rect {this->get_abs_rect()};
  ekg::service::theme_scheme_t &theme_scheme {ekg::current_theme_scheme()};
  ekg::ui::slider *p_ui {static_cast<ekg::ui::slider*>(this->p_data)};
  ekg::feature *&p_feature {p_ui->get_value()};
  ekg::number number {p_ui->get_number()};

  ekg::draw::sync_scissor(this->scissor, rect, this->p_parent_scissor);
  ekg_draw_assert_scissor();

  switch (p_ui->get_axis()) {
  case ekg::axis::horizontal:
    for (uint64_t it {}; it < this->range_list.size(); it++) {
      ekg::ui::slider_widget::range &range {this->range_list.at(it)};

      range.target.w = ekg::ui::slider_widget_calculate_target_pos(
        p_feature,
        number,
        range.rect.w,
        it
      );

      ekg::draw::rect(
        range.rect + rect,
        theme_scheme.slider_background,
        ekg::draw_mode::filled,
        ekg_layer(ekg::layer::background)
      );

      ekg::draw::rect(
        range.target + rect,
        theme_scheme.slider_activity_bar,
        ekg::draw_mode::filled,
        ekg_layer(ekg::layer::activity)
      );

      ekg::draw::rect(
        range.rect + rect,
        theme_scheme.slider_outline_bar,
        ekg::draw_mode::outline
      );
    }
    break;
  case ekg::axis::vertical:
    for (uint64_t it {}; it < this->range_list.size(); it++) {
      ekg::ui::slider_widget::range &range {this->range_list.at(it)};

      range.target.h = ekg::ui::slider_widget_calculate_target_pos(
        p_feature,
        number,
        range.rect.h,
        it
      );

      ekg::draw::rect(
        range.rect + rect,
        theme_scheme.slider_background,
        ekg::draw_mode::filled,
        ekg_layer(ekg::layer::background)
      );

      ekg::draw::rect(
        range.target + rect,
        theme_scheme.slider_activity_bar,
        ekg::draw_mode::filled,
        ekg_layer(ekg::layer::activity)
      );
    }
    break;
  }

  ekg::draw::rect(
    rect,
    theme_scheme.slider_outline,
    ekg::draw_mode::outline
  );
}

float ekg::ui::slider_widget_calculate_target_pos(
  ekg::feature *&p_feature,
  ekg::number number,
  float dimension,
  uint64_t index
) {
  switch (number) {
    case ekg::number::float64: {
      ekg::ui::slider::serializer_t<double> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<double>*>(p_feature)
      };

      ekg::ui::slider::range_t<double> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();
      return (dimension * (range.value.get_value() - range.min) / (range.max - range.min));
    }

    case ekg::number::float32: {
      ekg::ui::slider::serializer_t<float> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<float>*>(p_feature)
      };

      ekg::ui::slider::range_t<float> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();
      return (dimension * (range.value.get_value() - range.min) / (range.max - range.min));
    }
    
    case ekg::number::int64: {
      ekg::ui::slider::serializer_t<int64_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<int64_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<int64_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();
      return (dimension * (range.value.get_value() - range.min) / (range.max - range.min));
    }
    
    case ekg::number::uint64: {
      ekg::ui::slider::serializer_t<uint64_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<uint64_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<uint64_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();
      return (dimension * (range.value.get_value() - range.min) / (range.max - range.min));
    }
    
    case ekg::number::int32: {
      ekg::ui::slider::serializer_t<int32_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<int32_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<int32_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();
      return (dimension * (range.value.get_value() - range.min) / (range.max - range.min));
    }
    
    case ekg::number::uint32: {
      ekg::ui::slider::serializer_t<uint32_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<uint32_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<uint32_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();
      return (dimension * (range.value.get_value() - range.min) / (range.max - range.min));
    }
    
    case ekg::number::int16: {
      ekg::ui::slider::serializer_t<int16_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<int16_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<int16_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();
      return (dimension * (range.value.get_value() - range.min) / (range.max - range.min));
    }
    
    case ekg::number::uint16: {
      ekg::ui::slider::serializer_t<uint16_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<uint16_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<uint16_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();
      return (dimension * (range.value.get_value() - range.min) / (range.max - range.min));
    }
    
    case ekg::number::int8: {
      ekg::ui::slider::serializer_t<int8_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<int8_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<int8_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();
      return (dimension * (range.value.get_value() - range.min) / (range.max - range.min));
    }
    
    case ekg::number::uint8: {
      ekg::ui::slider::serializer_t<uint8_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<uint8_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<uint8_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();
      return (dimension * (range.value.get_value() - range.min) / (range.max - range.min));
    }
  }

  return 0.0f;  
}

void ekg::ui::slider_widget_calculate_value(ekg::feature *&p_feature, ekg::number number, float factor, float dimension, uint64_t index) {
  switch (number) {
    case ekg::number::float64: {
      ekg::ui::slider::serializer_t<double> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<double>*>(p_feature)
      };

      ekg::ui::slider::range_t<double> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();

      if (factor == 0) {
        range.value.set_value(range.min);
      } else {
        range.value.set_value(static_cast<double>((factor / dimension) * (range.max - range.min) + range.min));
      }
      break;
    }
    
    case ekg::number::float32: {
      ekg::ui::slider::serializer_t<float> *p_serializer {
        (ekg::ui::slider::serializer_t<float>*) p_feature
      };

      ekg::ui::slider::range_t<float> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();

      if (factor == 0) {
        range.value.set_value(range.min);
      } else {
        range.value.set_value(static_cast<float>((factor / dimension) * (range.max - range.min) + range.min));
      }

      break;
    }
    
    case ekg::number::int64: {
      ekg::ui::slider::serializer_t<int64_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<int64_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<int64_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();

      if (factor == 0) {
        range.value.set_value(range.min);
      } else {
        range.value.set_value(static_cast<int64_t>((factor / dimension) * (range.max - range.min) + range.min));
      }
      break;
    }
    
    case ekg::number::uint64: {
      ekg::ui::slider::serializer_t<uint64_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<uint64_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<uint64_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();

      if (factor == 0) {
        range.value.set_value(range.min);
      } else {
        range.value.set_value(static_cast<uint64_t>((factor / dimension) * (range.max - range.min) + range.min));
      }
      break;
    }
    
    case ekg::number::int32: {
      ekg::ui::slider::serializer_t<int32_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<int32_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<int32_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();

      if (factor == 0) {
        range.value.set_value(range.min);
      } else {
        range.value.set_value(static_cast<int32_t>((factor / dimension) * (range.max - range.min) + range.min));
      }
      break;
    }
    
    case ekg::number::uint32: {
      ekg::ui::slider::serializer_t<uint32_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<uint32_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<uint32_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();

      if (factor == 0) {
        range.value.set_value(range.min);
      } else {
        range.value.set_value(static_cast<uint32_t>((factor / dimension) * (range.max - range.min) + range.min));
      }
      break;
    }
    
    case ekg::number::int16: {
      ekg::ui::slider::serializer_t<int16_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<int16_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<int16_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();

      if (factor == 0) {
        range.value.set_value(range.min);
      } else {
        range.value.set_value(static_cast<int16_t>((factor / dimension) * (range.max - range.min) + range.min));
      }
      break;
    }
    
    case ekg::number::uint16: {
      ekg::ui::slider::serializer_t<uint16_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<uint16_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<uint16_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();

      if (factor == 0) {
        range.value.set_value(range.min);
      } else {
        range.value.set_value(static_cast<uint16_t>((factor / dimension) * (range.max - range.min) + range.min));
      }
      break;
    }
    
    case ekg::number::int8: {
      ekg::ui::slider::serializer_t<int8_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<int8_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<int8_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();

      if (factor == 0) {
        range.value.set_value(range.min);
      } else {
        range.value.set_value(static_cast<int8_t>((factor / dimension) * (range.max - range.min) + range.min));
      }
      break;
    }
    
    case ekg::number::uint8: {
      ekg::ui::slider::serializer_t<uint8_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<uint8_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<uint8_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();

      if (factor == 0) {
        range.value.set_value(range.min);
      } else {
        range.value.set_value(static_cast<uint8_t>((factor / dimension) * (range.max - range.min) + range.min));
      }
      break;
    }
  }
}

uint64_t ekg::ui::slider_widget_get_range_count(ekg::feature *&p_feature, ekg::number number) {
  switch (number) {
  case ekg::number::float64:
    return static_cast<ekg::ui::slider::serializer_t<double>*>(p_feature)->range_list.size();
  case ekg::number::float32:
    return static_cast<ekg::ui::slider::serializer_t<float>*>(p_feature)->range_list.size();
  case ekg::number::int64:
    return static_cast<ekg::ui::slider::serializer_t<int64_t>*>(p_feature)->range_list.size();
  case ekg::number::uint64:
    return static_cast<ekg::ui::slider::serializer_t<uint64_t>*>(p_feature)->range_list.size();
  case ekg::number::int32:
    return static_cast<ekg::ui::slider::serializer_t<int32_t>*>(p_feature)->range_list.size();
  case ekg::number::uint32:
    return static_cast<ekg::ui::slider::serializer_t<uint32_t>*>(p_feature)->range_list.size();
  case ekg::number::int16:
    return static_cast<ekg::ui::slider::serializer_t<int16_t>*>(p_feature)->range_list.size();
  case ekg::number::uint16:
    return static_cast<ekg::ui::slider::serializer_t<uint16_t>*>(p_feature)->range_list.size();
  case ekg::number::int8:
    return static_cast<ekg::ui::slider::serializer_t<int8_t>*>(p_feature)->range_list.size();
  case ekg::number::uint8:
    return static_cast<ekg::ui::slider::serializer_t<uint8_t>*>(p_feature)->range_list.size();
  }

  return 0;
}

std::string_view ekg::ui::slider_widget_get_value_label(
  ekg::feature *&p_feature,
  ekg::number number,
  uint64_t index
) {
  switch (number) {
    case ekg::number::float64: {
      ekg::ui::slider::serializer_t<double> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<double>*>(p_feature)
      };

      ekg::ui::slider::range_t<double> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();
      return ekg::string_float64_precision(range.value.get_value(), range.display_precision);
    }

    case ekg::number::float32: {
      ekg::ui::slider::serializer_t<float> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<float>*>(p_feature)
      };

      ekg::ui::slider::range_t<float> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();
      return ekg::string_float_precision(range.value.get_value(), range.display_precision);
    }
    
    case ekg::number::int64: {
      ekg::ui::slider::serializer_t<int64_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<int64_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<int64_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();
      return std::to_string(range.value.get_value());
    }
    
    case ekg::number::uint64: {
      ekg::ui::slider::serializer_t<uint64_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<uint64_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<uint64_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();
      return std::to_string(range.value.get_value());
    }
    
    case ekg::number::int32: {
      ekg::ui::slider::serializer_t<int32_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<int32_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<int32_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();
      return std::to_string(range.value.get_value());
    }
    
    case ekg::number::uint32: {
      ekg::ui::slider::serializer_t<uint32_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<uint32_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<uint32_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();
      return std::to_string(range.value.get_value());
    }
    
    case ekg::number::int16: {
      ekg::ui::slider::serializer_t<int16_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<int16_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<int16_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();
      return std::to_string(range.value.get_value());
    }
    
    case ekg::number::uint16: {
      ekg::ui::slider::serializer_t<uint16_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<uint16_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<uint16_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();
      return std::to_string(range.value.get_value());
    }
    
    case ekg::number::int8: {
      ekg::ui::slider::serializer_t<int8_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<int8_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<int8_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();
      return std::to_string(range.value.get_value());
    }
    
    case ekg::number::uint8: {
      ekg::ui::slider::serializer_t<uint8_t> *p_serializer {
        static_cast<ekg::ui::slider::serializer_t<uint8_t>*>(p_feature)
      };

      ekg::ui::slider::range_t<uint8_t> &range {p_serializer->range_list.at(index)};
      range.value.align_ownership_mem_if_necessary();
      return std::to_string(range.value.get_value());
    }
  }

  return "NaN";
}
