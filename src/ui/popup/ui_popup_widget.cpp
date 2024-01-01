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

#include "ekg/ui/popup/ui_popup_widget.hpp"
#include "ekg/ui/popup/ui_popup.hpp"
#include "ekg/ekg.hpp"
#include "ekg/draw/draw.hpp"

void ekg::ui::popup_widget::get_popup_path(std::string &path) {
  if (this->hovered_element == -1) {
    return;
  }

  auto p_ui {(ekg::ui::popup *) this->p_data};
  auto &item {p_ui->get_item_list().at(this->hovered_element)};

  ekg::ui::popup_widget *p_popup {};
  path += item.name;

  if (this->popup_opened != -1 &&
      (p_popup = (ekg::ui::popup_widget *) ekg::core->get_fast_widget_by_id(item.linked_id)) != nullptr) {
    path += " | ";
    p_popup->get_popup_path(path);
  }
}

bool ekg::ui::popup_widget::is_hovering_any_popup(int32_t top_level) {
  auto p_ui {(ekg::ui::popup *) this->p_data};
  bool is_hovering {ekg::rect_collide_vec(this->get_abs_rect(), ekg::input::interact())};

  ekg::ui::item item {};
  ekg::ui::popup_widget *p_popup {};
  int32_t item_it {this->popup_opened};

  if (top_level != this->top_level_popup &&
      (p_popup = (ekg::ui::popup_widget *) ekg::core->get_fast_widget_by_id(top_level)) != nullptr) {
    item_it = p_popup->popup_opened;
  }

  if (item_it != -1 && (item = p_ui->get_item_list().at(item_it)).linked_id != 0 &&
      (p_popup = (ekg::ui::popup_widget *) ekg::core->get_fast_widget_by_id(item.linked_id)) != nullptr) {
    is_hovering = is_hovering || p_popup->is_hovering_any_popup(p_popup->p_data->get_id());
  }

  return is_hovering;
}

void ekg::ui::popup_widget::unset_visible_all_sub_popup() {
  if (this->popup_opened == -1) {
    return;
  }

  auto p_ui {(ekg::ui::popup *) this->p_data};
  auto &item {p_ui->get_item_list().at(this->popup_opened)};
  auto p_popup {(ekg::ui::popup_widget *) ekg::core->get_fast_widget_by_id(item.linked_id)};

  if (p_popup->p_data->get_state() == ekg::state::visible) {
    p_popup->p_data->set_state(ekg::state::invisible);
    ekg::reload(item.linked_id);
    ekg::dispatch(ekg::env::redraw);
  }

  p_popup->unset_visible_all_sub_popup();
  this->popup_opened = -1;
}

void ekg::ui::popup_widget::on_reload() {
  ekg::ui::abstract_widget::on_reload();

  auto p_ui {(ekg::ui::popup *) this->p_data};
  auto &f_renderer {ekg::f_renderer(p_ui->get_font_size())};
  auto &item_list {p_ui->get_item_list()};
  auto &layout {ekg::core->service_layout};

  float max_width {};
  float text_width {};
  float text_height {f_renderer.get_text_height()};
  float dimension_offset {text_height / 2};

  /* First phase: get text metrics and prevent multi-iterations. */
  for (ekg::ui::item &items: item_list) {
    text_width = f_renderer.get_text_width(items.name);
    items.rect_content.w = text_width;
    items.rect_content.h = text_height;

    if (text_width > max_width) {
      max_width = text_width;
    }
  }

  /* Second phase: compute bounds and layout mask. */

  auto text_dock_flags {p_ui->get_text_align()};
  float offset {ekg::find_min_offset(max_width, dimension_offset)};
  float size {(text_height + dimension_offset) * static_cast<float>(p_ui->get_scaled_height())};

  ekg::vec3 layout_offset {offset, offset, text_height + dimension_offset};
  auto &layout_mask {layout.get_layout_mask()};

  this->dimension.w = ekg_min(this->dimension.w, max_width + dimension_offset);
  this->dimension.h = offset;
  this->separator_offset = offset;

  for (ekg::ui::item &items: item_list) {
    items.rect_dimension.x = 0.0f;
    items.rect_dimension.y = this->dimension.h;
    items.rect_dimension.w = this->dimension.w;
    items.rect_dimension.h = size;

    layout_offset.z = items.rect_dimension.h;

    layout.set_preset_mask(layout_offset, ekg::axis::horizontal, this->dimension.w);
    layout.insert_into_mask({&items.rect_content, text_dock_flags});
    layout.process_layout_mask();

    // Process the popup height based in layout mask.
    this->dimension.h += layout_mask.h;
  }

  // @TODO fix the final size of popup which is a bit more higher than the initial offset.

  /* Extra offset to final rect. */
  this->dimension.h += offset;

  /* Reset scissor height if current state is invisible. */
  auto *gpu_scissor {ekg::core->gpu_allocator.get_scissor_by_id(p_ui->get_id())};
  if (p_ui->get_state() == ekg::state::invisible && gpu_scissor != nullptr) {
    gpu_scissor->rect[3] = this->scissor_opened_height = 0;
    this->is_high_frequency = false;
  } else if (p_ui->get_state() == ekg::state::visible && !this->is_high_frequency) {
    ekg::update_high_frequency(this);
    this->elapsed_animation_ticks = SDL_GetTicks64();
  }

  /* Fix screen position at space. */
  if (!p_ui->has_parent()) {
    const float display_w {static_cast<float>(ekg::display::width)};
    const float display_h {static_cast<float>(ekg::display::height)};

    if (this->p_parent->x < 0) {
      this->p_parent->x = 0;
    }

    if (this->p_parent->y < 0) {
      this->p_parent->y = 0;
    }

    if (this->p_parent->x + this->dimension.w > display_w) {
      this->p_parent->x = display_w - this->dimension.w;
    }

    if (this->p_parent->y + this->dimension.h > display_h) {
      this->p_parent->y = display_h - this->dimension.h;
    }
  }

  this->min_size.x = ekg_min(this->min_size.x, this->dimension.w);
  this->min_size.y = ekg_min(this->min_size.y, this->dimension.h);
}

void ekg::ui::popup_widget::on_pre_event(SDL_Event &sdl_event) {
  if (this->p_data->get_state() == ekg::state::invisible) {
    return;
  }

  abstract_widget::on_pre_event(sdl_event);
}

void ekg::ui::popup_widget::on_event(SDL_Event &sdl_event) {
  if (this->p_data->get_state() == ekg::state::invisible) {
    return;
  }

  bool check_hovered {};
  bool should_destroy {};
  bool pressed {ekg::input::pressed() && ekg::input::action("popup-activy")};
  bool released {ekg::input::released()};

  auto p_ui {(ekg::ui::popup *) this->p_data};
  auto &item_list {p_ui->get_item_list()};
  auto &rect {this->get_abs_rect()};

  ekg::ui::popup_widget *p_popup {};

  /* Auto-check is this popup is the top-level popup. */
  this->parent_id_popup_opened = !p_ui->has_parent() || this->parent_id_popup_opened;
  this->top_level_popup = !p_ui->has_parent() ? p_ui->get_id() : this->top_level_popup;

  int32_t hovered {-1};
  if (ekg::input::motion() || pressed || released) {
    check_hovered = this->flag.hovered;
  }

  if (check_hovered) {
    ekg::vec4 &interact {ekg::input::interact()};
    ekg::rect &rect {this->get_abs_rect()};

    float ypos {};
    int32_t hover {};
    float popup_offset {2.0f};

    for (int32_t it {}; it < item_list.size(); it++) {
      auto &item {item_list.at(it)};
      hover = -1;

      if (ekg::rect_collide_vec(item.rect_dimension + rect, interact)) {
        hovered = it;
        hover = it;
      }

      switch (hover) {
        case -1:
          if (this->popup_opened == it && item.linked_id != 0 &&
              (p_popup = (ekg::ui::popup_widget *) ekg::core->get_fast_widget_by_id(item.linked_id)) != nullptr) {
            p_popup->p_data->set_state(ekg::state::invisible);
            p_popup->top_level_popup = this->top_level_popup;

            p_popup->parent_id_popup_opened = false;
            p_popup->unset_visible_all_sub_popup();

            ekg::reload(item.linked_id);
            ekg::dispatch(ekg::env::redraw);
            this->popup_opened = -1;
          }

          break;
        default:
          /* When the popup is linked with another some other popup widget, this should showcase the popup linked. */
          if (this->popup_opened != hover && item.linked_id != 0 &&
              (p_popup = (ekg::ui::popup_widget *) ekg::core->get_fast_widget_by_id(item.linked_id)) != nullptr) {
            p_popup->p_data->set_state(ekg::state::visible);
            p_popup->top_level_popup = this->top_level_popup;

            auto &popup_rect {p_popup->dimension};
            const float display_w {static_cast<float>(ekg::display::width)};
            const float display_h {static_cast<float>(ekg::display::height)};

            if (rect.x + rect.w + (popup_rect.w - popup_offset) > display_w) {
              popup_rect.x = -(popup_rect.w - popup_offset);
            } else {
              popup_rect.x = rect.w - popup_offset;
            }

            if (rect.y + ypos + popup_rect.h > display_h) {
              popup_rect.y = ypos - (rect.y + ypos + popup_rect.h - display_h);
            } else {
              popup_rect.y = ypos;
            }

            ekg::reload(p_popup->p_data->get_id());
            ekg::dispatch(ekg::env::redraw);

            p_popup->parent_id_popup_opened = true;
            this->popup_opened = hover;
          }

          /* And if there hovered is not linked with any and there is a shown popup, must close the shown popup. */
          if (this->popup_opened != -1 && item.linked_id == 0) {
            auto &popup_opened_item {item_list.at(this->popup_opened)};
            if (popup_opened_item.linked_id != 0 &&
                (p_popup = (ekg::ui::popup_widget *) ekg::core->get_fast_widget_by_id(popup_opened_item.linked_id)) !=
                nullptr) {
              p_popup->p_data->set_state(ekg::state::invisible);
              p_popup->top_level_popup = this->top_level_popup;

              p_popup->parent_id_popup_opened = false;
              p_popup->unset_visible_all_sub_popup();

              ekg::reload(item.linked_id);
              ekg::dispatch(ekg::env::redraw);
            }

            this->popup_opened = -1;
          }

          break;
      }

      ypos += item.rect_dimension.h;
    }

    ekg::set(this->hovered_element, hovered);
  }

  bool is_hovering_any_popup_flag {this->is_hovering_any_popup(this->top_level_popup)};

  /* This should be destroy when a click happens out of any sub-popup or top-level popup rect. */
  if (!check_hovered && !is_hovering_any_popup_flag && !p_ui->has_parent() && (pressed || released)) {
    should_destroy = true;
    this->popup_opened = -1;
  }

  /* Process the final input. */
  if (((pressed || released) && this->popup_opened == -1 && is_hovering_any_popup_flag)) {
    p_popup = (ekg::ui::popup_widget *) ekg::core->get_fast_widget_by_id(this->top_level_popup);
    p_popup->p_data->destroy();

    std::string path_result {};
    p_popup->get_popup_path(path_result);
    ekg::dispatch_ui_event(p_popup->p_data->get_tag().empty() ? "Unknown Popup UI" : p_popup->p_data->get_tag(),
                           path_result, (uint16_t) p_popup->p_data->get_type());
    return;
  }

  if (should_destroy) {
    this->p_data->destroy();
  }
}

void ekg::ui::popup_widget::on_update() {
  auto &rect {this->get_abs_rect()};
  auto *gpu_scissor {ekg::core->gpu_allocator.get_scissor_by_id(this->p_data->get_id())};

  float animation {
      ekg::smooth(ekg::theme().popup_drop_animation_delay, SDL_GetTicks64() - this->elapsed_animation_ticks)};
  this->scissor_opened_height = animation * this->dimension.h;

  if (ekg_equals_float(this->scissor_opened_height, this->dimension.h)) {
    scissor_opened_height = this->dimension.h;
    this->is_high_frequency = false;
  }

  gpu_scissor->rect[0] = rect.x;
  gpu_scissor->rect[1] = rect.y;
  gpu_scissor->rect[2] = rect.w;
  gpu_scissor->rect[3] = this->scissor_opened_height;
}

void ekg::ui::popup_widget::on_draw_refresh() {
  auto p_ui {(ekg::ui::popup *) this->p_data};
  auto &rect {this->get_abs_rect()};
  auto &theme {ekg::theme()};
  auto &f_renderer {ekg::f_renderer(p_ui->get_font_size())};
  auto &item_list {p_ui->get_item_list()};

  ekg::draw::bind_scissor(p_ui->get_id());

  ekg::draw::rect(rect, theme.popup_background);
  ekg::draw::rect(rect, theme.popup_outline, ekg::draw_mode::outline);

  ekg::rect button_rect {};

  for (int32_t it {}; it < item_list.size(); it++) {
    auto &item {item_list.at(it)};

    // Draw the separator.
    if (ekg::bitwise::contains(item.attributes, ekg::attr::separator)) {
      ekg::draw::rect(rect.x + item.rect_dimension.x,
                      rect.y + item.rect_dimension.y + item.rect_dimension.h - ekg_pixel_div_2,
                      item.rect_dimension.w,
                      1.0f,
                      theme.popup_separator);
    }

    if (this->hovered_element == it) {
      ekg::draw::rect(item.rect_dimension + rect, theme.popup_highlight);
    }

    f_renderer.blit(item.name,
                    rect.x + item.rect_dimension.x + item.rect_content.x,
                    rect.y + item.rect_dimension.y + item.rect_content.y,
                    theme.popup_string);
  }

  ekg::draw::bind_off_scissor();
}