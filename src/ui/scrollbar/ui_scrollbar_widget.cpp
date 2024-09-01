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

#include "ekg/ui/scrollbar/ui_scrollbar_widget.hpp"
#include "ekg/draw/draw.hpp"
#include "ekg/ekg.hpp"

void ekg::ui::scrollbar_widget::on_reload() {
  this->scroll.p_rect_mother = this->p_parent;
  this->scroll.mother_id = this->p_data->get_parent_id();
  this->scroll.widget_id = this->p_data->get_id();
  this->dimension.w = this->p_parent->w;
  this->dimension.h = this->p_parent->h;
  this->scroll.on_reload();
}

void ekg::ui::scrollbar_widget::on_pre_event(ekg::os::io_event_serial &io_event_serial) {
  this->scroll.on_pre_event(io_event_serial);
  this->flag.hovered = this->scroll.flag.hovered && !this->scroll.flag.activity;
  this->flag.absolute = this->scroll.is_dragging_bar() || this->scroll.flag.activity;
}

void ekg::ui::scrollbar_widget::on_event(ekg::os::io_event_serial &io_event_serial) {
  this->flag.hovered = this->scroll.flag.hovered;
  this->flag.scrolling = this->scroll.flag.scrolling;
  this->scroll.on_event(io_event_serial);

  this->flag.was_hovered = this->flag.hovered;
  if (!this->flag.was_hovered && this->is_high_frequency) {
    this->flag.was_hovered = false;
  }

  if ((this->flag.absolute || this->scroll.flag.activity) && !this->is_high_frequency) {
    ekg::update_high_frequency(this);
  }

  if (this->scroll.flag.state || this->scroll.flag.extra_state || this->flag.absolute) {
    ekg::dispatch(ekg::env::redraw);
  }
}

void ekg::ui::scrollbar_widget::on_post_event(ekg::os::io_event_serial &io_event_serial) {
  ekg::ui::abstract_widget::on_post_event(io_event_serial);
  this->scroll.flag.hovered = false;
  this->scroll.flag.activity = false;
}

void ekg::ui::scrollbar_widget::on_update() {
  this->scroll.on_update();
  this->is_high_frequency = this->scroll.check_activity_state(this->flag.was_hovered || this->scroll.flag.activity);
}

void ekg::ui::scrollbar_widget::on_draw_refresh() {
  this->dimension.w = this->p_parent->w;
  this->dimension.h = this->p_parent->h;

  ekg::draw::sync_scissor(this->scissor, this->get_abs_rect(), this->p_parent_scissor);
  ekg_draw_assert_scissor();

  this->scroll.on_draw_refresh();
}