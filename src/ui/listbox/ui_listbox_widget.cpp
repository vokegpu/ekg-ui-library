/*
 * MIT License
 *
 * Copyright (c) 2022-2023 Rina Wilk / vokegpu@gmail.com
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

void ekg::ui::listbox_widget::process_component_template(ekg::item &parent_item) {
}

void ekg::ui::listbox_widget::on_create() {
  auto p_ui {(ekg::ui::listbox *) this->p_data};
  auto &item {p_ui->item()};
}

void ekg::ui::listbox_widget::on_reload() {
  auto p_ui {(ekg::ui::listbox *) this->p_data};
  auto &ui_item {p_ui->item()};
  auto &rect {this->get_abs_rect()};

  auto &category_f_renderer {ekg::f_renderer(p_ui->get_category_font_size())};
  auto &item_f_renderer {ekg::f_renderer(p_ui->get_item_font_size())};

  this->category_font_metrics.y = category_f_renderer.get_text_height();
  this->category_font_metrics.x = this->category_font_metrics.y / 2.0f;

  this->item_font_metrics.y = item_f_renderer.get_text_height();
  this->item_font_metrics.x = this->item_font_metrics.y / 2.0f;

  this->rect_widget = {};
  this->component_category_last = {};
  this->loaded_item_list = {};
  this->process_component_template(ui_item);

  this->dimension.h = this->item_font_metrics.y * static_cast<float>(p_ui->get_scaled_height());
  ekg::dispatch(ekg::env::redraw);
}

void ekg::ui::listbox_widget::on_event(SDL_Event &sdl_event) {
  /*
  bool motion {ekg::input::motion()};
  bool pressed {ekg::input::pressed()};
  bool released {ekg::input::released()};

  if (motion || pressed || released) {
      ekg::vec4 &interact {ekg::input::interact()};
      ekg::rect &rect {this->get_abs_rect()};

      std::vector<ekg::item*> hovered_item_list {};
      ekg::item *p_item_hovered {};

      bool was_hovered {};
      bool should_redraw {};
      bool pressed_to_open {pressed && ekg::input::action("listbox-activy-open")};

      for (uint64_t it {}; it < this->loaded_item_list.size(); it++) {
          auto &p_item {this->loaded_item_list.at(it)};
          auto &component {p_item->component};

          if (p_item->p_item_parent != nullptr && !p_item->p_item_parent->component.is_open &&
              // the first item node does not have any parent, but it should render
              !(p_item->p_item_parent != nullptr &&
              p_item->p_item_parent != nullptr &&
              p_item->p_item_parent->p_item_parent == nullptr)) {
              continue;
          }

          if (ekg::bitwise::contains(p_item->attr, ekg::attr::unselectable)) {
              continue;
          }

          was_hovered = ekg::rect_collide_vec(component.rect_dimension_closed + rect, interact);
          if (hovered_item_list.empty() && was_hovered) {
              hovered_item_list.emplace_back() = p_item;
              p_item_hovered = p_item;
          }

          if (p_item->component.is_hovering && !p_item->empty() && !ekg::bitwise::contains(p_item->attr, ekg::attr::category) && pressed_to_open) {
              p_item->component.is_open = !p_item->component.is_open;
          }

          if (should_redraw == false && p_item->component.is_hovering != was_hovered) {
              ekg::dispatch(ekg::env::redraw);
              should_redraw = true;
          }

          p_item->component.is_hovering = was_hovered;
      }
      */

  /*
   * Row members items at the same index, interact togethers.
   * Category(s) and row(s) items attributes can not interact together.
   * If the mommy parent contains only one child, then it is not neceassary.
   */
  /*
      if (p_item_hovered != nullptr &&
          !ekg::bitwise::contains(p_item_hovered->attr, ekg::attr::category) &&
          !ekg::bitwise::contains(p_item_hovered->attr, ekg::attr::row) &&
          p_item_hovered->p_item_parent != nullptr &&
          ekg::bitwise::contains(p_item_hovered->p_item_parent->attr, ekg::attr::row_member) &&
          p_item_hovered->p_item_parent->p_item_parent != nullptr &&
          p_item_hovered->p_item_parent->p_item_parent->size() > 1) {

          ekg::item *&p_item_parent_mommy {p_item_hovered->p_item_parent->p_item_parent};

          uint64_t member_index {};
          bool found {};

          for (uint64_t it {}; it < p_item_hovered->p_item_parent->size(); it++) {
              auto &item {p_item_hovered->p_item_parent->at(it)};
              if (item.value == p_item_hovered->value) {
                  member_index = it;
                  found = true;
                  break;
              }
          }

          if (found) {
              for (ekg::item &items : *p_item_parent_mommy) {
                  if (ekg::bitwise::contains(items.attr, ekg::attr::row_member) && ekg::bitwise::contains(items.attr, ekg::attr::row)) {
                      if (items.size() > member_index) {
                          auto &item_target {items.at(member_index)};

                          // check hovering state
                          if (!ekg::bitwise::contains(item_target.attr, ekg::attr::row) && item_target.component.is_hovering != p_item_hovered->component.is_hovering) {
                              item_target.component.is_hovering = p_item_hovered->component.is_hovering;
                          }

                          // check open state
                          if (!ekg::bitwise::contains(item_target.attr, ekg::attr::row) && item_target.component.is_open != p_item_hovered->component.is_open) {
                              item_target.component.is_open = p_item_hovered->component.is_open;
                          }
                      }
                  }
              }
          }
      }
  }
  */
}

void ekg::ui::listbox_widget::on_update() {

}

void ekg::ui::listbox_widget::on_draw_refresh() {
  auto p_ui {(ekg::ui::listbox *) this->p_data};

  auto &rect {this->get_abs_rect()};
  auto &theme {ekg::theme()};
  auto &category_f_renderer {ekg::f_renderer(p_ui->get_category_font_size())};
  auto &item_f_renderer {ekg::f_renderer(p_ui->get_item_font_size())};

  /*

  ekg::draw::bind_scissor(p_ui->get_id());
  ekg::draw::sync_scissor(rect, p_ui->get_parent_id());

  // draw the outline frame
  ekg::draw::rect(rect, theme.listbox_outline, ekg::draw_mode::outline);

  ekg::rect rect_opened {};
  ekg::rect rect_opened_rectangle {0.0f, 0.0f, 0.0f, 0.0f};
  ekg::rect rect_opened_test {};

  bool should_reset_row {};
  bool next_row {};
  bool next_column {};
  bool check {};

  for (ekg::item *&p_item : this->loaded_item_list) {
      if (p_item->p_item_parent != nullptr && !p_item->p_item_parent->component.is_open &&
          // the first item node does not have any parent, but it should render
          !(p_item->p_item_parent != nullptr &&
            p_item->p_item_parent != nullptr &&
            p_item->p_item_parent->p_item_parent == nullptr)) {
          continue;
      }

      rect_opened_rectangle.h = rect_opened.h;

      switch (p_item->attr & ekg::attr::category) {
      default:
          ekg::draw::rect(p_item->component.rect_dimension_closed + rect + rect_opened_rectangle, theme.listbox_category_background);
          ekg::draw::rect(p_item->component.rect_dimension_closed + rect + rect_opened_rectangle, theme.listbox_category_outline, ekg::draw_mode::outline);

          if (p_item->component.is_hovering) {
              ekg::draw::rect(p_item->component.rect_dimension_closed + rect + rect_opened_rectangle, theme.listbox_highlight);
          }

          category_f_renderer.blit(p_item->value,
                                   p_item->component.rect_dimension_closed.x + p_item->component.rect_text.x + rect.x,
                                   p_item->component.rect_dimension_closed.y + p_item->component.rect_text.y + rect.y + rect_opened.h,
                                   theme.listbox_category_string);
          break;
      case 0:
          ekg::draw::rect(p_item->component.rect_dimension_closed + rect + rect_opened_rectangle, theme.listbox_item_background);
          ekg::draw::rect(p_item->component.rect_dimension_closed + rect + rect_opened_rectangle, theme.listbox_item_outline, ekg::draw_mode::outline);

          if (p_item->component.is_hovering) {
              ekg::draw::rect(p_item->component.rect_dimension_closed + rect + rect_opened_rectangle, theme.listbox_highlight);
          }

          item_f_renderer.blit(p_item->value,
                               p_item->component.rect_dimension_closed.x + p_item->component.rect_text.x + rect.x,
                               p_item->component.rect_dimension_closed.y + p_item->component.rect_text.y + rect.y + rect_opened.h,
                               theme.listbox_item_string);
          break;
      }

      if (ekg::bitwise::contains(p_item->attr, ekg::attr::separator)) {
          ekg::draw::rect(p_item->component.rect_dimension_closed.x + rect.x,
                          p_item->component.rect_dimension_closed.y + rect.y + p_item->component.rect_dimension_closed.h + rect_opened.h - ekg_pixel_div_2,
                          p_item->component.rect_dimension_closed.w,
                          1.0f,
                          theme.listbox_separator);
      }

      auto &dimension_closed {p_item->component.rect_dimension_closed};

      next_row = dimension_closed.x > rect_opened.w;
      next_column = dimension_closed.y > rect_opened_test.y;

      if (next_row) {
          rect_opened.h -= ekg::min((rect_opened.h + p_item->component.rect_dimension_opened.y) - (rect_opened_test.y + rect_opened.h), 0.0f);
          rect_opened_test.y = p_item->component.rect_dimension_closed.y;

          rect_opened.w = dimension_closed.x;
      }

      if (next_column) {
          rect_opened.h += p_item->p_item_parent != nullptr &&
                           p_item->p_item_parent->component.is_open &&
                           ( check =  (
                                  (
                                      p_item->component.rect_dimension_closed.y +
                                      p_item->component.rect_dimension_closed.h +
                                      1.0f // it is important to increase 1
                                  ) > (
                                      p_item->p_item_parent->component.rect_dimension_opened.y +
                                      p_item->p_item_parent->component.rect_dimension_opened.h
                                  )
                              )
                           )

                           ? p_item->p_item_parent->component.rect_dimension_opened.h : 0.0f;

          rect_opened_test.y = dimension_closed.y;

          if (!next_column) {
              rect_opened.w = 0.0f;
          }
      }
  }

  */

  ekg::draw::bind_off_scissor();
}