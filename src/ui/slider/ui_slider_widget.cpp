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
  ekg::font base_font_size {p_ui->get_font_size()};
  ekg::draw::font_renderer &f_renderer {ekg::f_renderer(base_font_size)};
  ekg::draw::font_renderer &small_f_renderer {ekg::f_renderer(ekg::font::small)};
  ekg::service::theme_scheme_t &theme_scheme {ekg::current_theme_scheme()};

  ekg::rect &rect {this->get_abs_rect()};
  ekg::rect relative_rect {};
  ekg::flags text_align_flags {p_ui->get_text_align()};

  float base_text_height {f_renderer.get_text_height()};
  float dimension_offset {static_cast<float>((int32_t) (base_text_height / 2.0f))};
  float offset {ekg::find_min_offset(base_text_height, dimension_offset)};

  float min_text_width {};
  float max_text_width {};
  float text_width {};
  float small_font_text_height {small_f_renderer.get_text_height()};

  float bar_thickness {
    static_cast<float>(theme_scheme.slider_bar_thickness) / 100
  };

  ekg::layout::mask &mask {ekg::core->mask};
  std::string text {};

  switch (axis) {
    case ekg::axis::horizontal: {
      this->dimension.w = ekg_min(this->dimension.w, base_text_height);
      this->dimension.h = (base_text_height + dimension_offset) * static_cast<float>(p_ui->get_scaled_height());

      uint64_t range_list_size {
        p_ui->get_range_count()
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
        mask.preset({4.0f, 0.0f, this->dimension.h}, axis, this->dimension.w);

        for (uint64_t it {}; it < range_list_size; it++) {
          ekg::ui::slider_widget::range &range {this->range_list.at(it)};

          ekg::ui::slider_widget_get_value_label(
            p_ui,
            number,
            it,
            range.text
          );

          /**
           * Note: EKG allocator contains an issue on rendering where the next
           * UV coords is jittering due some stupid text size (unknown wsize).
           * May you want know more: https://github.com/vokegpu/ekg-ui-library/issues/22
           **/
          range.text += " ";

          ekg::ui::slider_widget_get_metrics(
            p_ui,
            number,
            it,
            f_renderer,
            &min_text_width,
            &max_text_width
          );

          if (min_text_width > max_text_width) {
            max_text_width = min_text_width;
          }

          range.rect_text.w = max_text_width;
          range.rect_text.h = base_text_height;
          range.font_size = base_font_size;

          range.rect.h = this->dimension.h * bar_thickness;

          mask.insert({&range.rect_text, text_align_flags});
          mask.insert({&range.rect, text_align_flags | ekg::dock::fill});
        }

        mask.docknize();
      }
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
      factor = ekg_clamp(interact.x - bar.x, 0.0f, range.rect.w);
      dimension = range.rect.w;
      break;
    case ekg::axis::vertical:
      factor = ekg_clamp(interact.y - bar.y, 0.0f, range.rect.h);
      dimension = range.rect.h;
      break;
    }

    ekg::ui::slider_widget_calculate_value(
      p_ui,
      p_ui->get_number(),
      factor,
      dimension,
      this->targetted_range_index
    );

    ekg::reload(this);
  }
}

void ekg::ui::slider_widget::on_draw_refresh() {
  ekg::rect &rect {this->get_abs_rect()};
  ekg::service::theme_scheme_t &theme_scheme {ekg::current_theme_scheme()};
  ekg::ui::slider *p_ui {static_cast<ekg::ui::slider*>(this->p_data)};
  ekg::number number {p_ui->get_number()};

  ekg::draw::sync_scissor(this->scissor, rect, this->p_parent_scissor);
  ekg_draw_assert_scissor();

  switch (p_ui->get_axis()) {
  case ekg::axis::horizontal:
    for (uint64_t it {}; it < this->range_list.size(); it++) {
      ekg::ui::slider_widget::range &range {this->range_list.at(it)};

      range.target = range.rect;
      range.target.w = ekg::ui::slider_widget_calculate_target_pos(
        p_ui,
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

      ekg::f_renderer(range.font_size).blit(
        range.text,
        range.rect_text.x + rect.x,
        range.rect_text.y + rect.y,
        theme_scheme.slider_string
      );
    }
    break;
  case ekg::axis::vertical:
    for (uint64_t it {}; it < this->range_list.size(); it++) {
      ekg::ui::slider_widget::range &range {this->range_list.at(it)};

      range.target.h = ekg::ui::slider_widget_calculate_target_pos(
        p_ui,
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
  ekg::ui::slider *&p_ui,
  ekg::number number,
  float dimension,
  uint64_t index
) {
  switch (number) {
    case ekg::number::float64: {
      ekg::ui::slider::range_t &range {p_ui->range<double>(index)};
      range.f64.align_ownership_mem_if_necessary();
      return (dimension * (range.f64.get_value() - range.f64_min) / (range.f64_max - range.f64_min));
    }

    case ekg::number::float32: {
      ekg::ui::slider::range_t &range {p_ui->range<float>(index)};
      range.f32.align_ownership_mem_if_necessary();
      return (dimension * (range.f32.get_value() - range.f32_min) / (range.f32_max - range.f32_min));
    }
    
    case ekg::number::int64: {
      ekg::ui::slider::range_t &range {p_ui->range<int64_t>(index)};
      range.i64.align_ownership_mem_if_necessary();
      return (dimension * (range.i64.get_value() - range.i64_min) / (range.i64_max - range.i64_min));
    }
    
    case ekg::number::uint64: {
      ekg::ui::slider::range_t &range {p_ui->range<uint64_t>(index)};
      range.u64.align_ownership_mem_if_necessary();
      return (dimension * (range.u64.get_value() - range.u64_min) / (range.u64_max - range.u64_min));
    }
    
    case ekg::number::int32: {
      ekg::ui::slider::range_t &range {p_ui->range<int32_t>(index)};
      range.i32.align_ownership_mem_if_necessary();
      return (dimension * (range.i32.get_value() - range.i32_min) / (range.i32_max - range.i32_min));
    }
    
    case ekg::number::uint32: {
      ekg::ui::slider::range_t &range {p_ui->range<uint32_t>(index)};
      range.u32.align_ownership_mem_if_necessary();
      return (dimension * (range.u32.get_value() - range.u32_min) / (range.u32_max - range.u32_min));
    }
    
    case ekg::number::int16: {
      ekg::ui::slider::range_t &range {p_ui->range<int16_t>(index)};
      range.i16.align_ownership_mem_if_necessary();
      return (dimension * (range.i16.get_value() - range.i16_min) / (range.i16_max - range.i16_min));
    }
    
    case ekg::number::uint16: {
      ekg::ui::slider::range_t &range {p_ui->range<uint16_t>(index)};
      range.u16.align_ownership_mem_if_necessary();
      return (dimension * (range.u16.get_value() - range.u16_min) / (range.u16_max - range.u16_min));
    }
    
    case ekg::number::int8: {
      ekg::ui::slider::range_t &range {p_ui->range<int8_t>(index)};
      range.i8.align_ownership_mem_if_necessary();
      return (dimension * (range.i8.get_value() - range.i8_min) / (range.i8_max - range.i8_min));
    }
    
    case ekg::number::uint8: {
      ekg::ui::slider::range_t &range {p_ui->range<uint8_t>(index)};
      range.u8.align_ownership_mem_if_necessary();
      return (dimension * (range.u8.get_value() - range.u8_min) / (range.u8_max - range.u8_min));
    }
  }

  return 0.0f;  
}

void ekg::ui::slider_widget_calculate_value(
  ekg::ui::slider *&p_ui,
  ekg::number number,
  float factor,
  float dimension,
  uint64_t index
) {
  switch (number) {
    case ekg::number::float64: {
      ekg::ui::slider::range_t &range {p_ui->range<double>(index)};
      range.f64.align_ownership_mem_if_necessary();

      if (factor == 0) {
        range.f64.set_value(range.f64_min);
      } else {
        range.f64.set_value(static_cast<double>((factor / dimension) * (range.f64_max - range.f64_min) + range.f64_min));
      }
      break;
    }
    
    case ekg::number::float32: {
      ekg::ui::slider::range_t &range {p_ui->range<float>(index)};
      range.f32.align_ownership_mem_if_necessary();

      if (factor == 0) {
        range.f32.set_value(range.f32_min);
      } else {
        range.f32.set_value(static_cast<float>((factor / dimension) * (range.f32_max - range.f32_min) + range.f32_min));
      }

      break;
    }
    
    case ekg::number::int64: {
      ekg::ui::slider::range_t &range {p_ui->range<int64_t>(index)};
      range.i64.align_ownership_mem_if_necessary();

      if (factor == 0) {
        range.i64.set_value(range.i64_min);
      } else {
        range.i64.set_value(static_cast<int64_t>((factor / dimension) * (range.i64_max - range.i64_min) + range.i64_min));
      }
      break;
    }
    
    case ekg::number::uint64: {
      ekg::ui::slider::range_t &range {p_ui->range<uint64_t>(index)};
      range.u64.align_ownership_mem_if_necessary();

      if (factor == 0) {
        range.u64.set_value(range.u64_min);
      } else {
        range.u64.set_value(static_cast<uint64_t>((factor / dimension) * (range.u64_max - range.u64_min) + range.u64_min));
      }
      break;
    }
    
    case ekg::number::int32: {
      ekg::ui::slider::range_t &range {p_ui->range<int32_t>(index)};
      range.i32.align_ownership_mem_if_necessary();

      if (factor == 0) {
        range.i32.set_value(range.i32_min);
      } else {
        range.i32.set_value(static_cast<int32_t>((factor / dimension) * (range.i32_max - range.i32_min) + range.i32_min));
      }
      break;
    }
    
    case ekg::number::uint32: {
      ekg::ui::slider::range_t &range {p_ui->range<uint32_t>(index)};
      range.u32.align_ownership_mem_if_necessary();

      if (factor == 0) {
        range.u32.set_value(range.u32_min);
      } else {
        range.u32.set_value(static_cast<uint32_t>((factor / dimension) * (range.u32_max - range.u32_min) + range.u32_min));
      }
      break;
    }
    
    case ekg::number::int16: {
      ekg::ui::slider::range_t &range {p_ui->range<int16_t>(index)};
      range.i16.align_ownership_mem_if_necessary();

      if (factor == 0) {
        range.i16.set_value(range.i16_min);
      } else {
        range.i16.set_value(static_cast<int16_t>((factor / dimension) * (range.i16_max - range.i16_min) + range.i16_min));
      }
      break;
    }
    
    case ekg::number::uint16: {
      ekg::ui::slider::range_t &range {p_ui->range<uint16_t>(index)};
      range.u16.align_ownership_mem_if_necessary();

      if (factor == 0) {
        range.u16.set_value(range.u16_min);
      } else {
        range.u16.set_value(static_cast<uint16_t>((factor / dimension) * (range.u16_max - range.u16_min) + range.u16_min));
      }
      break;
    }
    
    case ekg::number::int8: {
      ekg::ui::slider::range_t &range {p_ui->range<int8_t>(index)};
      range.i8.align_ownership_mem_if_necessary();

      if (factor == 0) {
        range.i8.set_value(range.i8_min);
      } else {
        range.i8.set_value(static_cast<int8_t>((factor / dimension) * (range.i8_max - range.i8_min) + range.i8_min));
      }
      break;
    }
    
    case ekg::number::uint8: {
      ekg::ui::slider::range_t &range {p_ui->range<uint8_t>(index)};
      range.u8.align_ownership_mem_if_necessary();

      if (factor == 0) {
        range.u8.set_value(range.u8_min);
      } else {
        range.u8.set_value(static_cast<uint8_t>((factor / dimension) * (range.u8_max - range.u8_min) + range.u8_min));
      }
      break;
    }
  }
}

void ekg::ui::slider_widget_get_value_label(
  ekg::ui::slider *&p_ui,
  ekg::number number,
  uint64_t index,
  std::string &content
) {
  switch (number) {
    case ekg::number::float64: {
      ekg::ui::slider::range_t &range {p_ui->range<double>(index)};
      range.f64.align_ownership_mem_if_necessary();
      content = ekg::string_float64_precision(range.f64.get_value(), range.display_precision);
      break;
    }

    case ekg::number::float32: {
      ekg::ui::slider::range_t &range {p_ui->range<float>(index)};
      range.f32.align_ownership_mem_if_necessary();
      content = ekg::string_float_precision(range.f32.get_value(), range.display_precision);
      break;
    }
    
    case ekg::number::int64: {
      ekg::ui::slider::range_t &range {p_ui->range<int64_t>(index)};
      range.i64.align_ownership_mem_if_necessary();
      content = std::to_string(range.i64.get_value());
      break;
    }
    
    case ekg::number::uint64: {
      ekg::ui::slider::range_t &range {p_ui->range<uint64_t>(index)};
      range.u64.align_ownership_mem_if_necessary();
      content = std::to_string(range.u64.get_value());
      break;
    }

    case ekg::number::int32: {
      ekg::ui::slider::range_t &range {p_ui->range<int32_t>(index)};
      range.i32.align_ownership_mem_if_necessary();
      content = std::to_string(range.i32.get_value());
      break;
    }
    
    case ekg::number::uint32: {
      ekg::ui::slider::range_t &range {p_ui->range<uint32_t>(index)};
      range.u32.align_ownership_mem_if_necessary();
      content = std::to_string(range.u32.get_value());
      break;
    }
    
    case ekg::number::int16: {
      ekg::ui::slider::range_t &range {p_ui->range<int16_t>(index)};
      range.i16.align_ownership_mem_if_necessary();
      content = std::to_string(range.i16.get_value());
      break;
    }
    
    case ekg::number::uint16: {
      ekg::ui::slider::range_t &range {p_ui->range<uint16_t>(index)};
      range.u16.align_ownership_mem_if_necessary();
      content = std::to_string(range.u16.get_value());
      break;
    }
    
    case ekg::number::int8: {
      ekg::ui::slider::range_t &range {p_ui->range<int8_t>(index)};
      range.i8.align_ownership_mem_if_necessary();
      content = std::to_string(range.i8.get_value());
      break;
    }
    
    case ekg::number::uint8: {
      ekg::ui::slider::range_t &range {p_ui->range<uint8_t>(index)};
      range.u8.align_ownership_mem_if_necessary();
      content = std::to_string(range.u8.get_value());
      break;
    }
  }
}

void ekg::ui::slider_widget_get_metrics(
  ekg::ui::slider *&p_ui,
  ekg::number number,
  uint64_t index,
  ekg::draw::font_renderer &f_renderer,
  float *p_min_text_width,
  float *p_max_text_width
) {
  switch (number) {
    case ekg::number::float64: {
      ekg::ui::slider::range_t &range {p_ui->range<double>(index)};
      range.f64.align_ownership_mem_if_necessary();

      *p_min_text_width = f_renderer.get_text_width(ekg::string_float64_precision(range.f64_min, range.display_precision));
      *p_max_text_width = f_renderer.get_text_width(ekg::string_float64_precision(range.f64_min, range.display_precision));

      break;
    }

    case ekg::number::float32: {
      ekg::ui::slider::range_t &range {p_ui->range<float>(index)};
      range.f32.align_ownership_mem_if_necessary();

      *p_min_text_width = f_renderer.get_text_width(ekg::string_float_precision(range.f32_min, range.display_precision));
      *p_max_text_width = f_renderer.get_text_width(ekg::string_float_precision(range.f32_max, range.display_precision));

      break;
    }
    
    case ekg::number::int64: {
      ekg::ui::slider::range_t &range {p_ui->range<int64_t>(index)};
      range.i64.align_ownership_mem_if_necessary();

      *p_min_text_width = f_renderer.get_text_width(std::to_string(range.i64_min));
      *p_max_text_width = f_renderer.get_text_width(std::to_string(range.i64_max));

      break;
    }
    
    case ekg::number::uint64: {
      ekg::ui::slider::range_t &range {p_ui->range<uint64_t>(index)};
      range.u64.align_ownership_mem_if_necessary();

      *p_min_text_width = f_renderer.get_text_width(std::to_string(range.u64_min));
      *p_max_text_width = f_renderer.get_text_width(std::to_string(range.u64_max));

      break;
    }
    
    case ekg::number::int32: {
      ekg::ui::slider::range_t &range {p_ui->range<int32_t>(index)};
      range.i32.align_ownership_mem_if_necessary();

      *p_min_text_width = f_renderer.get_text_width(std::to_string(range.i32_min));
      *p_max_text_width = f_renderer.get_text_width(std::to_string(range.i32_max));

      break;
    }
    
    case ekg::number::uint32: {
      ekg::ui::slider::range_t &range {p_ui->range<uint32_t>(index)};
      range.u32.align_ownership_mem_if_necessary();

      *p_min_text_width = f_renderer.get_text_width(std::to_string(range.u32_min));
      *p_max_text_width = f_renderer.get_text_width(std::to_string(range.u32_max));

      break;
    }
    
    case ekg::number::int16: {
      ekg::ui::slider::range_t &range {p_ui->range<int16_t>(index)};
      range.i16.align_ownership_mem_if_necessary();

      *p_min_text_width = f_renderer.get_text_width(std::to_string(range.i16_min));
      *p_max_text_width = f_renderer.get_text_width(std::to_string(range.i16_max));

      break;
    }
    
    case ekg::number::uint16: {
      ekg::ui::slider::range_t &range {p_ui->range<uint16_t>(index)};
      range.u16.align_ownership_mem_if_necessary();

      *p_min_text_width = f_renderer.get_text_width(std::to_string(range.u16_min));
      *p_max_text_width = f_renderer.get_text_width(std::to_string(range.u16_max));

      break;
    }
    
    case ekg::number::int8: {
      ekg::ui::slider::range_t &range {p_ui->range<int8_t>(index)};
      range.i8.align_ownership_mem_if_necessary();

      *p_min_text_width = f_renderer.get_text_width(std::to_string(range.i8_min));
      *p_max_text_width = f_renderer.get_text_width(std::to_string(range.i8_max));

      break;
    }
    
    case ekg::number::uint8: {
      ekg::ui::slider::range_t &range {p_ui->range<uint8_t>(index)};
      range.u8.align_ownership_mem_if_necessary();

      *p_min_text_width = f_renderer.get_text_width(std::to_string(range.u8_min));
      *p_max_text_width = f_renderer.get_text_width(std::to_string(range.u8_max));

      break;
    }
  }
}