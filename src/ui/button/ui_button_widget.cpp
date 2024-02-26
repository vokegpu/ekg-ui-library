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

#include "ekg/ui/button/ui_button_widget.hpp"
#include "ekg/ui/button/ui_button.hpp"
#include "ekg/ekg.hpp"
#include "ekg/draw/draw.hpp"

void ekg::ui::button_widget::on_reload() {
  abstract_widget::on_reload();

  auto p_ui {(ekg::ui::button *) this->p_data};
  auto &rect {this->get_abs_rect()};
  auto &f_renderer {ekg::f_renderer(p_ui->get_font_size())};

  float text_width {f_renderer.get_text_width(p_ui->get_text())};
  float text_height {f_renderer.get_text_height()};

  float dimension_offset {static_cast<float>((int32_t) (text_height / 2.0f))};
  float offset {ekg::find_min_offset(text_width, dimension_offset)};

  this->dimension.w = ekg_min(this->dimension.w, text_width);
  this->dimension.h = (text_height + dimension_offset) * static_cast<float>(p_ui->get_scaled_height());

  this->min_size.x = ekg_min(this->min_size.x, text_height);
  this->min_size.y = ekg_min(this->min_size.y, this->dimension.h);

  this->rect_text.w = text_width;
  this->rect_text.h = text_height;

  auto &layout {ekg::core->service_layout};
  layout.set_preset_mask({offset, offset, this->dimension.h}, ekg::axis::horizontal, this->dimension.w);
  layout.insert_into_mask({&this->rect_text, p_ui->get_text_align()});
  layout.process_layout_mask();

  auto &layout_mask {layout.get_layout_mask()};
  this->dimension.w = (int32_t) (this->dimension.w <= text_width ? layout_mask.w : this->dimension.w);
  this->dimension.h = ekg_min(this->dimension.h, layout_mask.h);
}

void ekg::ui::button_widget::on_event(ekg::os::io_event_serial &io_event_serial) {
  bool pressed {ekg::input::pressed()};
  bool released {ekg::input::released()};
  bool motion {ekg::input::motion()};

  if (motion || pressed || released) {
    ekg_action_dispatch(
      motion && this->flag.hovered && ekg::timing::second > ekg::display::latency,
      ekg::action::hover
    );

    ekg::set(this->flag.highlight, this->flag.hovered);
  }

  if (
      pressed &&
      !this->flag.activity &&
      this->flag.hovered &&
      ekg::input::action("button-activity")
  ) {
    ekg_action_dispatch(
      true,
      ekg::action::press
    );

    ekg::set(this->flag.activity, true);
  } else if (released && this->flag.activity) {
    ekg_action_dispatch(
      this->flag.hovered,
      ekg::action::release
    );

    ekg_action_dispatch(
      this->flag.hovered,
      ekg::action::activity
    );

    auto p_ui {(ekg::ui::button*) this->p_data};
    p_ui->set_value(this->flag.hovered);
    ekg::set(this->flag.activity, false);
  }
}

void ekg::ui::button_widget::on_draw_refresh() {
  auto p_ui {(ekg::ui::button *) this->p_data};
  auto &rect {this->get_abs_rect()};
  auto &theme {ekg::theme()};
  auto &f_renderer {ekg::f_renderer(p_ui->get_font_size())};

  ekg::draw::sync_scissor(this->scissor, rect, this->p_parent_scissor);
  ekg_draw_assert_scissor();

  ekg::draw::rect(rect, theme.button_background);
  ekg::draw::rect(rect, theme.button_outline, ekg::draw_mode::outline);

  if (this->flag.highlight) {
    ekg::draw::rect(rect, theme.button_highlight);
  }

  if (this->flag.activity) {
    ekg::draw::rect(rect, theme.button_activity);
    ekg::draw::rect(rect, {theme.button_activity, theme.button_outline.w}, ekg::draw_mode::outline);
  }

  f_renderer.blit(p_ui->get_text(), rect.x + this->rect_text.x, rect.y + this->rect_text.y, theme.button_string);
}