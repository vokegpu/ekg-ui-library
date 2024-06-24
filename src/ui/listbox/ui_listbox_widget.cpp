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

void ekg::ui::listbox_widget::on_create() {
  ekg::ui::listbox *p_ui {static_cast<ekg::ui::listbox*>(this->p_data)};
  this->p_item_list = static_cast<std::vector<ekg::item>*>(p_ui);
}

void ekg::ui::listbox_widget::on_reload() {
  auto *p_ui {(ekg::ui::listbox *) this->p_data};
  auto &rect {this->get_abs_rect()};

  ekg::draw::font_renderer &f_renderer_item {ekg::f_renderer(p_ui->get_item_font_size())};
  ekg::draw::font_renderer &f_renderer_column_header {ekg::f_renderer(p_ui->get_column_header_font_size())};
  ekg::layout::mask &mask {ekg::core->mask};

  float text_height {f_renderer_item.get_text_height()};
  float dimension_offset {text_height / 2};
  float offset {ekg::find_min_offset(text_height, dimension_offset)};

  this->dimension.h = (text_height + dimension_offset) * static_cast<float>(p_ui->get_scaled_height());

  this->min_size.x = ekg_min(this->min_size.x, text_height);
  this->min_size.y = ekg_min(this->min_size.y, text_height);

  ekg::font item_font {p_ui->get_item_font_size()};
  ekg::rect relative_rect {};
  ekg::rect relative_largest_rect {};

  uint64_t arbitrary_index_pos {};
  int32_t item_scaled_height {p_ui->get_item_scaled_height()};
  uint16_t column_header_dock_flags {p_ui->get_column_header_align()};

  float text_width {};
  int32_t text_lines {};
  uint64_t items_header_size {this->p_item_list->size()};

  text_height = f_renderer_column_header.get_text_height();
  bool opened {};

  for (uint64_t it {}; it < items_header_size; it++) {
    relative_rect.y = 0.0f;
    arbitrary_index_pos = 0;

    ekg::item &item {this->p_item_list->at(it)};
    ekg::placement &placement {item.unsafe_get_placement()};

    text_lines = 0;
    text_width = f_renderer_column_header.get_text_width(item.get_value(), text_lines);

    placement.rect.w = ekg_min(placement.rect.w, text_width);
    placement.rect.h = (text_height + dimension_offset) * static_cast<float>(item_scaled_height);

    if (ekg_bitwise_contains(column_header_dock_flags, ekg::dock::fill)) {
      placement.rect.w = this->dimension.w / items_header_size;
    }

    relative_rect.w = placement.rect.w;

    if (
        ekg_bitwise_contains(column_header_dock_flags, ekg::dock::top) ||
        !ekg_bitwise_contains(column_header_dock_flags, ekg::dock::bottom)
       ) {
      ekg::ui::listbox_template_reload(
        item,
        rect,
        item_font,
        relative_rect,
        item_scaled_height,
        arbitrary_index_pos,
        opened
      );
    } else {
      ekg::ui::listbox_template_reload(
        item,
        rect,
        item_font,
        relative_rect,
        item_scaled_height,
        arbitrary_index_pos,
        opened
      );

      // do here <bla>
    }

    placement.rect_text.w = text_width;
    placement.rect_text.h = text_height * static_cast<float>(text_lines);

    mask.preset(
      {
        offset, offset, placement.rect.h
      },
      ekg::axis::horizontal,
      placement.rect.w
    );
    
    mask.insert({&placement.rect_text, placement.text_dock_flags});
    mask.docknize();

    relative_rect.x += relative_rect.w;

    if (relative_rect.x > relative_largest_rect.w) {
      relative_largest_rect.w = relative_rect.x;
    }

    if (relative_rect.y > relative_largest_rect.h) {
      relative_largest_rect.h = relative_rect.y;
    }
  }

  this->embedded_scroll.acceleration.y = (text_height * 3.0f) + (offset * 2.0f);
  this->embedded_scroll.p_rect_mother = &rect;
  this->embedded_scroll.widget_id = this->p_data->get_id();
  this->embedded_scroll.rect_child = relative_largest_rect;
  this->embedded_scroll.on_reload();
}

void ekg::ui::listbox_widget::on_pre_event(ekg::os::io_event_serial &io_event_serial) {
  abstract_widget::on_pre_event(io_event_serial);

  this->embedded_scroll.on_pre_event(io_event_serial);
  this->flag.absolute = (
    this->embedded_scroll.is_dragging_bar() ||
    this->embedded_scroll.flag.activity ||
    this->flag.state
  );
}

void ekg::ui::listbox_widget::on_event(ekg::os::io_event_serial &io_event_serial) {
  bool pressed_open {ekg::input::action("listbox-activity-open")};
  bool pressed_select_many {ekg::input::action("listbox-activity-select-many")};
  bool pressed_select {ekg::input::action("listbox-activity-select")};
  bool released {ekg::input::released()};
  bool motion {ekg::input::motion() && this->flag.hovered};

  this->embedded_scroll.on_event(io_event_serial);

  if ((this->flag.focused || this->flag.hovered || this->flag.absolute) && !this->is_high_frequency) {
    ekg::update_high_frequency(this);
  }

  if ((!motion && !released && !pressed_select && !pressed_open && !pressed_select_many) ||
      (!this->flag.hovered && !this->was_selected)) {
    return;    
  }

  ekg::rect &rect {this->get_abs_rect()};
  this->was_selected = false;

  ekg::rect relative_largest_rect {};
  ekg::rect relative_rect {};
  ekg::rect scrollable_rect {rect + this->embedded_scroll.scroll};
  ekg::vec2 ui_pos {rect.x, rect.y};

  ekg::ui::listbox *p_ui {static_cast<ekg::ui::listbox*>(this->p_data)};
  ekg::mode mode {p_ui->get_mode()};

  for (uint64_t it {}; it < this->p_item_list->size(); it++) {
    relative_rect.y = 0.0f;

    ekg::item &item {this->p_item_list->at(it)};
    ekg::ui::listbox_template_on_event(
      io_event_serial,
      motion,
      released,
      pressed_select_many,
      pressed_select,
      pressed_open,
      this->was_selected,
      item,
      ui_pos,
      scrollable_rect,
      relative_rect,
      mode
    );

    relative_rect.x += relative_rect.w;

    if (relative_rect.x > relative_largest_rect.w) {
      relative_largest_rect.w = relative_rect.x;
    }

    if (relative_rect.y > relative_largest_rect.h) {
      relative_largest_rect.h = relative_rect.y;
    }
  }

  this->embedded_scroll.rect_child = relative_largest_rect;
}

void ekg::ui::listbox_widget::on_post_event(ekg::os::io_event_serial &io_event_serial) {
  abstract_widget::on_post_event(io_event_serial);
  this->embedded_scroll.flag.hovered = false;
  this->embedded_scroll.flag.activity = false;
}

void ekg::ui::listbox_widget::on_update() {
  this->embedded_scroll.on_update();
  this->is_high_frequency = this->embedded_scroll.check_activity_state(this->flag.focused || this->flag.hovered);
}

void ekg::ui::listbox_widget::on_draw_refresh() {
  auto &rect {this->get_abs_rect()};
  auto &theme {ekg::theme()};
  auto *p_ui {static_cast<ekg::ui::listbox*>(this->p_data)};

  this->embedded_scroll.clamp_scroll();

  ekg::draw::sync_scissor(this->scissor, rect, this->p_parent_scissor);
  ekg_draw_assert_scissor();

  ekg::draw::rect(rect, theme.listbox_background, ekg::draw_mode::filled, ekg_layer(ekg::layer::background));

  ekg::rect relative_rect {};
  ekg::font item_font {p_ui->get_item_font_size()};

  uint64_t arbitrary_index_pos {};
  ekg::rect scrollable_rect {rect + this->embedded_scroll.scroll};
  bool stop_rendering_items {};
  ekg::vec2 ui_pos {rect.x, rect.y};

  for (uint64_t it {}; it < this->p_item_list->size(); it++) {
    arbitrary_index_pos = 0;

    ekg::item &item {this->p_item_list->at(it)};
    ekg::placement &placement {item.unsafe_get_placement()};

    ekg::ui::listbox_template_render(
      p_ui,
      item,
      ui_pos,
      scrollable_rect,
      item_font,
      relative_rect,
      this->embedded_scroll.scroll.y,
      arbitrary_index_pos,
      stop_rendering_items
    );

    if (stop_rendering_items) {
      break;
    }
  }

  this->embedded_scroll.scissor = this->scissor;
  this->embedded_scroll.on_draw_refresh();

  ekg::draw::rect(rect, theme.listbox_outline, ekg::draw_mode::outline);  
}

void ekg::ui::listbox_template_reload(
  ekg::item &parent,
  ekg::rect &ui_rect,
  ekg::font &item_font,
  ekg::rect &relative_rect,
  int32_t item_scaled_height,
  uint64_t pos,
  bool &opened
) {
  ekg::layout::mask &mask {ekg::core->mask};
  ekg::draw::font_renderer &f_renderer {ekg::f_renderer(item_font)};
  ekg::rect item_rect {};
  ekg::service::theme &theme {ekg::theme()};

  uint64_t it {};
  int32_t text_lines {};
  uint16_t flags {};
  bool just_flagged_cursive_opened {}; 

  float text_width {};
  float text_height {};

  float dimension_offset {};
  float offset {};

  for (it = it; it < parent.size(); it++) {
    ekg::item &item {parent.at(it)};
    ekg::placement &placement {item.unsafe_get_placement()};
    
    item_rect = ui_rect + placement.rect;
    flags = item.get_attr();

    text_lines = 0;
    text_width = f_renderer.get_text_width(item.get_value(), text_lines);
    text_height = f_renderer.get_text_height();

    dimension_offset = text_height / 2;
    offset = ekg::find_min_offset(text_width, dimension_offset);

    placement.rect.x = relative_rect.x;
    placement.rect.y = relative_rect.y;

    placement.rect.w = ekg_min(relative_rect.w, text_width);
    placement.rect.h = (text_height + dimension_offset) * static_cast<float>(item_scaled_height);

    placement.rect_text.w = text_width;
    placement.rect_text.h = text_height * static_cast<float>(text_lines);

    mask.preset(
      {
        offset, offset, placement.rect.h
      },
      ekg::axis::horizontal,
      placement.rect.w
    );
    
    mask.insert({&placement.rect_text, placement.text_dock_flags});
    mask.docknize();

    placement.rect_extent.y = placement.rect.h;
    just_flagged_cursive_opened = opened;

    if (!opened && ekg_bitwise_contains(flags, ekg::attr::opened)) {
      just_flagged_cursive_opened = true;
      opened = true;
    }

    if (!item.empty()) {
      relative_rect.x += theme.listbox_subitem_offset_space;
      relative_rect.w -= theme.listbox_subitem_offset_space;

      ekg::ui::listbox_template_reload(
        item,
        ui_rect,
        item_font,
        relative_rect,
        item_scaled_height,
        pos,
        opened
      );

      relative_rect.x -= theme.listbox_subitem_offset_space;
      relative_rect.w += theme.listbox_subitem_offset_space;
    }

    if (just_flagged_cursive_opened) {
      opened = false;
    }

    relative_rect.y -= (relative_rect.y - placement.rect.y) * !opened;
    relative_rect.y += placement.rect.h + ekg_pixel;
  }
}

void ekg::ui::listbox_template_on_event(
  ekg::os::io_event_serial &io_event_serial,
  bool motion,
  bool released,
  bool pressed_select_many,
  bool pressed_select,
  bool pressed_open,
  bool &was_selected,
  ekg::item &parent,
  ekg::vec2 &ui_pos,
  ekg::rect &ui_rect,
  ekg::rect &relative_rect,
  ekg::mode mode
) {
  ekg::vec4 &interact {ekg::input::interact()};
  ekg::rect item_rect {};

  bool hovering {};
  uint16_t flags {};
  bool contains_flag {};
  bool multicolumn {mode == ekg::mode::multicolumn};

  for (uint64_t it {}; it < parent.size(); it++) {
    ekg::item &item {parent.at(it)};
    ekg::placement &placement {item.unsafe_get_placement()};

    item_rect = placement.rect + ui_rect;

    switch (mode) {
    case ekg::mode::multicolumn:
      item_rect.w = ui_rect.w;
      item_rect.x = ui_rect.x;
      break;
    }

    flags = item.get_attr();
    hovering = ekg::rect_collide_vec(item_rect, interact);

    if (!ekg_bitwise_contains(flags, ekg::attr::hovering) && hovering) {
      ekg_bitwise_add(flags, ekg::attr::hovering);
      item.set_attr(flags);
    } else if (ekg_bitwise_contains(flags, ekg::attr::hovering) && !hovering) {
      ekg_bitwise_remove(flags, ekg::attr::hovering);
      item.set_attr(flags);
    }

    if (!item.empty() && hovering && pressed_open) {
      contains_flag = static_cast<bool>(ekg_bitwise_contains(flags, ekg::attr::opened));
      ekg_bitwise_remove(flags, ekg::attr::opened * contains_flag);
      ekg_bitwise_add(flags, ekg::attr::opened * !contains_flag);
      item.set_attr(flags);
    }

    if (hovering && pressed_select_many) {
      contains_flag = static_cast<bool>(ekg_bitwise_contains(flags, ekg::attr::focused));
      ekg_bitwise_remove(flags, ekg::attr::focused * contains_flag);
      ekg_bitwise_add(flags, ekg::attr::focused * !contains_flag);
      item.set_attr(flags);
    } else if (pressed_select && hovering && !ekg_bitwise_contains(flags, ekg::attr::focused)) {
      ekg_bitwise_add(flags, ekg::attr::focused);
      item.set_attr(flags);
    } else if (!hovering && pressed_select && ekg_bitwise_contains(flags, ekg::attr::focused)) {
      ekg_bitwise_remove(flags, ekg::attr::focused);
      item.set_attr(flags);
    }

    if (ekg_bitwise_contains(flags, ekg::attr::focused)) {
      was_selected = true;
    }

    placement.rect.y = relative_rect.y;
    relative_rect.y += placement.rect.h + ekg_pixel;

    if (!item.empty() && ekg_bitwise_contains(item.get_attr(), ekg::attr::opened)) {
      ekg::ui::listbox_template_on_event(
        io_event_serial,
        motion,                                                                    
        released,
        pressed_select_many,
        pressed_select,
        pressed_open,
        was_selected,
        item,
        ui_pos,
        ui_rect,
        relative_rect,
        mode
      );
    }
  }
}

void ekg::ui::listbox_template_render(
  ekg::ui::listbox *&p_data,
  ekg::item &parent,
  ekg::vec2 &ui_pos,
  ekg::rect &ui_rect,
  ekg::font &item_font,
  ekg::rect &relative_rect,
  float y_scroll,
  uint64_t pos,
  bool stop_rendering_items
) {
  ekg::service::theme &theme {ekg::theme()};
  ekg::draw::font_renderer &f_renderer {ekg::f_renderer(item_font)};
  ekg::rect item_rect {};

  float bottom_place {ui_pos.y + ui_rect.h};
  uint16_t flags {};

  uint64_t it {};
  if (!pos) {
    // do dynamic index calc based on y-scroll
  }

  pos++;

  for (it = it; it < parent.size(); it++) {
    ekg::item &item {parent.at(it)};
    ekg::placement &placement {item.unsafe_get_placement()};

    item_rect = placement.rect + ui_rect;
    if (item_rect.y > ui_pos.y - item_rect.h) {
      flags = item.get_attr();

      ekg::draw::rect(
        item_rect,
        theme.listbox_item_background,
        ekg::draw_mode::filled,
        ekg_layer(ekg::layer::sub_background)
      );

      if (ekg_bitwise_contains(flags, ekg::attr::hovering)) {
        ekg::draw::rect(
          item_rect,
          theme.listbox_item_highlight,
          ekg::draw_mode::filled,
          ekg_layer(ekg::layer::highlight)
        );

        ekg::draw::rect(
          item_rect,
          theme.listbox_item_highlight_outline,
          ekg::draw_mode::outline
        );
      }

      if (ekg_bitwise_contains(flags, ekg::attr::focused)) {
        ekg::draw::rect(
          item_rect,
          theme.listbox_item_focused,
          ekg::draw_mode::filled,
          ekg_layer(ekg::layer::activity)
        );

        ekg::draw::rect(
          item_rect,
          theme.listbox_item_highlight_outline,
          ekg::draw_mode::outline
        );
      }

      f_renderer.blit(
        item.get_value(),
        item_rect.x + placement.rect_text.x,
        item_rect.y + placement.rect_text.y,
        theme.listbox_item_string
      );

      ekg::draw::rect(
        item_rect,
        theme.listbox_item_outline,
        ekg::draw_mode::outline
      );
    }

    if (!item.empty() && ekg_bitwise_contains(flags, ekg::attr::opened)) {
      ekg::ui::listbox_template_render(
        p_data,
        item,
        ui_pos,
        ui_rect,
        item_font,
        relative_rect,
        y_scroll,
        pos,
        stop_rendering_items
      );
    }

    if (item_rect.y + item_rect.h > bottom_place) {
      stop_rendering_items = true;
      break;
    }
  }
}