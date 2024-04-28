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

#include "ekg/ui/listbox/ui_listbox_widget.hpp"
#include "ekg/ekg.hpp"
#include "ekg/draw/draw.hpp"

void ekg::ui::listbox_widget::process_component_template() {
   
}

void ekg::ui::listbox_widget::on_create() {
  ekg::ui::listbox *p_ui {static_cast<ekg::ui::listbox*>(this->p_data)};
  this->p_item_list = static_cast<std::vector<ekg::item>*>(p_ui);
}

void ekg::ui::listbox_widget::on_reload() {
  auto *p_ui {(ekg::ui::listbox *) this->p_data};
  auto &rect {this->get_abs_rect()};

  ekg::draw::font_renderer &f_renderer {ekg::f_renderer(p_ui->get_item_font_size())};

  float text_height {f_renderer.get_text_height()};
  float text_width {text_height + text_height}; // temp

  float dimension_offset = text_height / 2;
  float offset {ekg::find_min_offset(text_width, dimension_offset)};
  float dimension_height {
    (text_height + dimension_offset) * static_cast<float>(p_ui->get_scaled_height())
  };

  this->dimension.w = ekg_min(this->dimension.w, text_width);
  this->dimension.h = dimension_height;

  this->min_size.x = ekg_min(this->min_size.x, text_height);
  this->min_size.y = ekg_min(this->min_size.y, dimension_height);
}

void ekg::ui::listbox_widget::on_event(ekg::os::io_event_serial &io_event_serial) {
}

void ekg::ui::listbox_widget::on_update() {

}

void ekg::ui::listbox_widget::on_draw_refresh() {
  auto &rect {this->get_abs_rect()};
  auto &theme {ekg::theme()};
  auto *p_ui {static_cast<ekg::ui::listbox*>(this->p_data)};

  ekg::draw::sync_scissor(this->scissor, rect, this->p_parent_scissor);
  ekg_draw_assert_scissor();

  ekg::draw::rect(rect, theme.listbox_background);

  ekg::font item_font {p_ui->get_item_font_size()};

  for (uint64_t it {}; it < this->p_item_list->size(); it++) {
    ekg::item &item {this->p_item_list->at(it)};   
    ekg::ui::listbox_template_rendering(item, rect, item_font);
  }

  ekg::draw::rect(rect, theme.listbox_outline, ekg::draw_mode::outline);  
}

void ekg::ui::listbox_template_rendering(ekg::item &parent, ekg::rect &ui_rect, ekg::font &item_font) {
  ekg::service::theme &theme {ekg::theme()};
  ekg::draw::font_renderer &f_renderer {ekg::f_renderer(item_font)};
  ekg::rect item_rect {};

  for (ekg::item &item : parent) {
    ekg::placement &placement {item.unsafe_get_placement()};
    item_rect = ui_rect + placement.rect;

    ekg::draw::rect(
      item_rect,
      theme.listbox_item_background
    );

    if (ekg_bitwise_contains(item.get_attr(), ekg::attr::hovering)) {
      ekg::draw::rect(
        item_rect,
        theme.listbox_item_highlight
      );
    }

    f_renderer.blit(
      item.get_value(),
      item_rect.x + 10.0f, item_rect.y + 10.0f,
      theme.listbox_item_string
    );
  }
}