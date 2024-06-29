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

#include "ekg/ui/label/ui_label_widget.hpp"
#include "ekg/ekg.hpp"
#include "ekg/draw/draw.hpp"

void ekg::ui::label_widget::on_reload() {
  abstract_widget::on_reload();

  ekg::ui::label *p_ui {static_cast<ekg::ui::label*>(this->p_data)};
  ekg::rect &rect {this->get_abs_rect()};
  ekg::draw::font_renderer &f_renderer {ekg::f_renderer(p_ui->get_font_size())};
  int32_t scaled_height {p_ui->get_scaled_height()};

  float text_width {f_renderer.get_text_width(p_ui->get_value(), scaled_height)};
  float text_height {f_renderer.get_text_height()};

  float dimension_offset {text_height / 2};
  float offset {ekg::find_min_offset(text_width, dimension_offset)};

  this->dimension.w = ekg_min(this->dimension.w, text_width);
  this->dimension.h = (text_height + dimension_offset) * static_cast<float>(scaled_height);

  this->min_size.x = ekg_min(this->min_size.x, text_height);
  this->min_size.y = ekg_min(this->min_size.y, this->dimension.h);

  this->rect_text.w = text_width;
  this->rect_text.h = text_height * static_cast<float>(scaled_height);

  ekg::layout::mask &mask {ekg::core->mask};
  mask.preset({offset, offset, this->dimension.h}, ekg::axis::horizontal, this->dimension.w);
  mask.insert({&this->rect_text, p_ui->get_text_align()});
  mask.docknize();

  ekg::rect &layout_mask {mask.get_rect()};
  this->dimension.w = layout_mask.w;
}

void ekg::ui::label_widget::on_draw_refresh() {
  ekg::ui::label *p_ui {static_cast<ekg::ui::label*>(this->p_data)};
  ekg::rect &rect {this->get_abs_rect()};
  ekg::draw::font_renderer &f_renderer {ekg::f_renderer(p_ui->get_font_size())};
  ekg::service::theme &theme {ekg::theme()};

  if (p_ui->was_changed()) {
    ekg::reload(this);
  }

  ekg::draw::sync_scissor(this->scissor, rect, this->p_parent_scissor);
  ekg_draw_assert_scissor();

  ekg::draw::rect(
    rect,
    theme.label_background,
    ekg::draw_mode::filled, 
    ekg_layer(ekg::layer::background)
  );

  f_renderer.blit(
    p_ui->get_value(),
    rect.x + this->rect_text.x,
    rect.y + this->rect_text.y,
    theme.label_string
  );

  ekg::draw::rect(
    rect,
    theme.label_outline,
    ekg::draw_mode::outline
  );
}