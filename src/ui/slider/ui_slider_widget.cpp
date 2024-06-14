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

void ekg::ui::slider_widget::update_bar(float x, float y) {
  auto p_ui {(ekg::ui::slider *) this->p_data};
  auto &rect {this->get_abs_rect()};
  auto bar {this->rect_bar + rect};
  auto orientation {p_ui->get_bar_align()};

  float factor {};
  float dimension_factor {};
  float min {p_ui->get_value_min()};
  float max {p_ui->get_value_max()};

  switch (orientation) {
    case ekg::dock::left: {
      break;
    }

    default: {
      factor = ekg_min(x - bar.x, 0.0f);
      dimension_factor = bar.w;
      break;
    }
  }

  if (factor == 0) {
    factor = p_ui->get_value_min();
  } else {
    factor = (factor / dimension_factor) * (max - min) + min;
  }

  if (p_ui->set_value(factor)) {
    ekg::reload(this);
  }
}

void ekg::ui::slider_widget::on_reload() {
  abstract_widget::on_reload();

  auto p_ui {(ekg::ui::slider *) this->p_data};
  auto &rect {this->get_abs_rect()};
  auto &f_renderer {ekg::f_renderer(p_ui->get_font_size())};
  auto text_dock_flags {p_ui->get_text_align()};
  auto bar_dock_flags {p_ui->get_bar_align()};
  auto bar_axis {p_ui->get_bar_axis()};
  auto &theme {ekg::theme()};
  auto value_precision {p_ui->get_precision()};

  float text_height {f_renderer.get_text_height()};
  float value {p_ui->get_value()}, min {p_ui->get_value_min()}, max {p_ui->get_value_max()};

  std::string max_string {ekg::string_float_precision(max, value_precision)};
  std::string min_string {ekg::string_float_precision(min, value_precision)};

  float min_text_width {f_renderer.get_text_width(min_string)};
  float max_text_width {f_renderer.get_text_width(max_string)};
  float largest_text_width {max_text_width > min_text_width ? max_text_width : min_text_width};

  float text_width {largest_text_width};
  this->string_value = ekg::string_float_precision(value, value_precision);

  float dimension_offset {text_height / 2};
  float offset {ekg::find_min_offset(text_width, dimension_offset)};
  float dimension_height {(text_height + dimension_offset) * static_cast<float>(p_ui->get_scaled_height())};

  float normalised_bar_thickness {static_cast<float>(theme.slider_bar_thickness) / 100};
  float normalised_target_thickness {static_cast<float>(theme.slider_target_thickness) / 100};
  bool centered_text {text_dock_flags == ekg::dock::center};

  ekg::service::layout &layout {ekg::core->service_layout};

  this->dimension.w = ekg_min(this->dimension.w, text_width);
  this->dimension.h = dimension_height;

  this->min_size.x = ekg_min(this->min_size.x, text_height);
  this->min_size.y = ekg_min(this->min_size.y, dimension_height);

  if (centered_text) {
    auto &f_renderer_small {ekg::f_renderer(this->font_render_size)};
    text_width = f_renderer_small.get_text_width(this->string_value);
    text_height = f_renderer_small.get_text_height();
  }

  this->rect_text.w = largest_text_width;
  this->rect_text.h = text_height;

  float bar_difference_size {};

  /**
   * @TODO
   * The slider has a little unknown size-difference from layout align,
   * not sure why this small layout difference happen, I think its the
   * text width subtracted or something, but it is necessary check
   * and out-print the values.
   * 
   * Some day I will rewrite this, bu! not now. 
   **/

  switch (bar_axis) {
    case ekg::axis::horizontal: {
      /* we need to subtract the bar with the text size if text align is not center (center follows the target) */
      float text_rendering_not_center {((text_width + offset * 2) * (!centered_text))};
      this->rect_bar.w = this->dimension.w - (offset * 2) - text_rendering_not_center;
      this->rect_bar.h = dimension_height * normalised_bar_thickness;

      this->rect_bar_value.w = this->rect_bar.w * (value - min) / (max - min);
      this->rect_bar_value.h = this->rect_bar.h;

      // Radius are equals to the both dimension (w, h)
      this->rect_target.w = dimension_height * normalised_target_thickness;
      this->rect_target.h = this->rect_target.w;

      bar_difference_size = this->rect_bar.h;
      this->rect_bar.h = ekg_min(this->rect_bar.h, this->rect_target.h);
      layout.set_preset_mask({offset, offset, dimension_height}, bar_axis, this->dimension.w);

      break;
    }

    case ekg::axis::vertical: {
      // todo axis height.
      break;
    }
  }

  this->font_render_size = ekg::font::small;
  if (!centered_text) {
    layout.insert_into_mask({&this->rect_text, text_dock_flags});
    this->font_render_size = ekg::font::normal;
  }

  layout.insert_into_mask({&this->rect_bar, bar_dock_flags});
  layout.process_layout_mask();

  auto &layout_mask {layout.get_layout_mask()};
  this->dimension.h = ekg_min(layout_mask.h, layout_mask.h);

  this->rect_bar.h = bar_difference_size;
  this->rect_target.x = this->rect_bar.x + this->rect_bar_value.w - (this->rect_target.w / 2);
  this->rect_target.y = this->rect_bar.y + (this->rect_bar.h / 2) - (this->rect_target.h / 2);

  if (centered_text && bar_axis == ekg::axis::horizontal &&
      ekg_bitwise_contains(bar_dock_flags, ekg::dock::top | ekg::dock::bottom | ekg::dock::center)) {
    float middle_size {this->dimension.h - (this->rect_target.y + this->rect_target.h)};
    middle_size /= 2;

    this->rect_text.x = this->rect_target.x + (this->rect_target.w / 2) - (this->rect_text.w / 2);
    this->rect_text.y = (
      ekg_bitwise_contains(bar_dock_flags, ekg::dock::top | ekg::dock::center) ?
        this->rect_target.y + this->rect_target.h + (middle_size - (this->rect_text.h / 2)) :
        offset + (middle_size - (this->rect_text.h / 2))
    );

    bool text_out_dimension_left {this->rect_text.x < offset};
    this->rect_text.x = text_out_dimension_left ? offset : this->rect_text.x;

    bool text_out_dimension_right {this->rect_text.x + this->rect_text.w > this->dimension.w - offset};
    this->rect_text.x = text_out_dimension_right ? (this->dimension.w - offset - this->rect_text.w) : this->rect_text.x;
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
  auto p_ui {(ekg::ui::slider *) this->p_data};
  auto &rect {this->get_abs_rect()};
  auto &interact {ekg::input::interact()};

  bool pressed {ekg::input::pressed()};
  bool released {ekg::input::released()};
  bool motion {ekg::input::motion()};

  if (motion || pressed || released) {
    ekg::set(
      this->flag.highlight,
      this->flag.hovered && ekg::rect_collide_vec(this->rect_bar + rect, interact)
    );

    ekg_action_dispatch(
      motion && this->flag.hovered && ekg::timing::second > ekg::ui::latency,
      ekg::action::hover
    );
  }

  this->flag.absolute = this->flag.absolute || this->flag.activity;
  this->flag.hovered = this->flag.hovered && this->flag.highlight;

  if (this->flag.state) {
    if (p_ui->set_value(p_ui->get_value() + interact.w)) {
      ekg::reload(this);
    }
  } else if (this->flag.hovered && pressed && ekg::input::action("slider-activity")) {
    ekg_action_dispatch(
      !this->flag.activity,
      ekg::action::press
    );

    this->flag.activity = true;
    this->update_bar(interact.x, interact.y);
    this->flag.absolute = true;
    p_ui->set_dragging(true);
  } else if (released) {
    if (this->flag.activity) {
      ekg_action_dispatch(
        true,
        ekg::action::release
      );
    }

    this->flag.absolute = false;
    this->flag.activity = false;
    p_ui->set_dragging(false);
  } else if (this->flag.activity && motion) {
    ekg_action_dispatch(
      ekg::timing::second > ekg::ui::latency,
      ekg::action::drag
    );

    this->update_bar(interact.x, interact.y);
  }
}

void ekg::ui::slider_widget::on_draw_refresh() {
  auto p_ui {(ekg::ui::slider *) this->p_data};
  auto &rect {this->get_abs_rect()};
  auto &theme {ekg::theme()};
  auto &f_renderer {ekg::f_renderer(this->font_render_size)};
  auto bar {this->rect_bar + rect}, bar_value {this->rect_bar_value + rect};;

  if (p_ui->was_changed()) {
    ekg::reload(this);
  }

  ekg::draw::sync_scissor(this->scissor, rect, this->p_parent_scissor);
  ekg_draw_assert_scissor();

  ekg::draw::rect(
    bar,
    theme.slider_background,
    ekg::draw_mode::filled,
    ekg_layer(ekg::layer::background)
  );

  if (this->flag.highlight) {
    ekg::draw::rect(
      bar,
      theme.slider_highlight,
      ekg::draw_mode::filled,
      ekg_layer(ekg::layer::highlight)
    );
  }

  ekg::draw::rect(
    this->rect_target + rect,
    theme.slider_activity,
    ekg::draw_mode::circle,
    ekg_layer(ekg::layer::activity)
  );

  ekg::draw::rect(
    bar.x,
    bar.y,
    bar_value.w,
    bar_value.h,
    theme.slider_activity_bar,
    ekg::draw_mode::filled,
    ekg_layer(ekg::layer::activity)
  );

  f_renderer.blit(
    this->string_value,
    rect.x + this->rect_text.x,
    rect.y + this->rect_text.y,
    theme.slider_string
  );
}