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
  this->must_update_items = true;
}

void ekg::ui::listbox_widget::on_reload() {
  ekg::ui::listbox *p_ui {static_cast<ekg::ui::listbox*>(this->p_data)};
  ekg::rect &rect {this->get_abs_rect()};

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
  int32_t column_header_scaled_height {p_ui->get_column_header_scaled_height()};
  uint16_t column_header_dock_flags {p_ui->get_column_header_align()};

  float text_width {};
  int32_t text_lines {};
  uint64_t items_header_size {p_ui->p_value->size()};
  uint64_t old_cache_size {};

  text_height = f_renderer_column_header.get_text_height();
  dimension_offset = text_height / 2;

  bool opened {};
  ekg::mode mode {p_ui->get_mode()};
  bool is_multicolumn {mode == ekg::mode::multicolumn};
  float scaled_width {rect.w};
  float header_relative_x {ekg_pixel};

  for (uint64_t it {}; it < items_header_size; it++) {
    relative_rect.y = 0.0f;

    ekg::item &item {p_ui->p_value->at(it)};
    ekg::placement &placement {item.unsafe_get_placement()};

    text_lines = 0;
    text_width = f_renderer_column_header.get_text_width(item.get_value(), text_lines);

    if (ekg_bitwise_contains(column_header_dock_flags, ekg::dock::fill)) {
      /**
       * Check for the note in `ekg/layout/docknize.hpp`::docknize widgets function,
       * for understand the reason of integer 32bits casting here.
       **/
      placement.rect.w = static_cast<int32_t>(
        (scaled_width) / static_cast<int32_t>(items_header_size)
      );
    } else {
      placement.rect.w = ekg_min(placement.rect.w, text_width);
    }

    relative_rect.w = placement.rect.w;
    placement.rect.h = (text_height + dimension_offset) * static_cast<float>(column_header_scaled_height);

    placement.rect.x = relative_rect.x;
    placement.rect.y = relative_rect.y;

    arbitrary_index_pos = 0;

    if (
        ekg_bitwise_contains(column_header_dock_flags, ekg::dock::top) ||
        !ekg_bitwise_contains(column_header_dock_flags, ekg::dock::bottom)
       ) {
      this->rect_content_place.y = placement.rect.h + ekg_pixel;
      this->column_header_height = placement.rect.h + ekg_pixel;
    }

    if (this->must_update_items) {
      ekg::ui::listbox_template_reload(
        item,
        rect,
        placement.rect,
        item_font,
        relative_rect,
        item_scaled_height,
        it,
        arbitrary_index_pos,
        opened,
        mode,
        &this->must_update_items
      );
    
      if (it >= this->item_rendering_cache.size()) {
        this->item_rendering_cache.emplace_back().resize(arbitrary_index_pos);
      } else {
        ekg::item &rendering_cache {this->item_rendering_cache.at(it)};

        if (arbitrary_index_pos < rendering_cache.size()) {
          rendering_cache.erase(
            rendering_cache.begin() + arbitrary_index_pos + 1,
            rendering_cache.end()
          );
        } else {
          rendering_cache.resize(arbitrary_index_pos);
        }
      }
    }

    ekg::item &rendering_cache {this->item_rendering_cache.at(it)};
    rendering_cache.unsafe_get_placement() = item.unsafe_get_placement();
    rendering_cache.set_value(item.get_value());
    rendering_cache.set_attr(item.get_attr());
    rendering_cache.set_text_align(item.get_text_align());

    placement.rect.x = header_relative_x; // rect was used to get the right aligned offset
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

    header_relative_x += placement.rect.w + ekg_pixel;
    relative_rect.x += relative_rect.w + !is_multicolumn;

    if (relative_rect.x > relative_largest_rect.w) {
      relative_largest_rect.w = relative_rect.x;
    }

    if (relative_rect.y > relative_largest_rect.h) {
      relative_largest_rect.h = relative_rect.y;
    }
  }

  std::cout << "rendering-cache amount: " << arbitrary_index_pos << std::endl;

  this->embedded_scroll.acceleration.y = (text_height * 3.0f) + (offset * 2.0f);
  this->embedded_scroll.p_rect_mother = &this->rect_content_abs;
  this->embedded_scroll.widget_id = this->p_data->get_id();
  this->embedded_scroll.on_reload();
  this->must_update_items = false;
}

void ekg::ui::listbox_widget::on_pre_event(ekg::os::io_event_serial &io_event_serial) {
  abstract_widget::on_pre_event(io_event_serial);

  this->embedded_scroll.on_pre_event(io_event_serial);
  this->flag.absolute = (
    this->embedded_scroll.is_dragging_bar() ||
    this->embedded_scroll.flag.activity || this->flag.state
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
  ekg::rect scrollable_rect {this->rect_content_abs + this->embedded_scroll.scroll};
  ekg::vec2 ui_pos {rect.x, rect.y};

  ekg::ui::listbox *p_ui {static_cast<ekg::ui::listbox*>(this->p_data)};
  ekg::mode mode {p_ui->get_mode()};
  uint64_t arbitrary_index_pos {};
  uint64_t largest_rendering_cache_size {};

  switch (pressed_open) {
  case true:
    for (uint64_t it {}; it < p_ui->p_value->size(); it++) {
      relative_rect.y = 0.0f;
      arbitrary_index_pos = 0;

      ekg::item &rendering_cache {this->item_rendering_cache.at(it)};
      ekg::item &item_header {p_ui->p_value->at(it)};

      /**
      * Do not copy all the content by simple one reason:
      * Copying the the major header from `p_value` to rendering cache element,
      * erase ALL reserved memory from reload, re-reserving is performanceless here.
      * 
      * Passing only necessary stuff (attributes, value, etc). 
      **/

      rendering_cache.unsafe_get_placement() = item_header.unsafe_get_placement();
      rendering_cache.set_value(item_header.get_value());
      rendering_cache.set_attr(item_header.get_attr());
      rendering_cache.set_text_align(item_header.get_text_align());

      ekg::placement &placement_header {item_header.unsafe_get_placement()};

      ekg::ui::listbox_template_on_event(
        io_event_serial,
        arbitrary_index_pos,
        rendering_cache,
        motion,
        released,
        pressed_select_many,
        pressed_select,
        pressed_open,
        this->was_selected,
        item_header,
        ui_pos,
        scrollable_rect,
        relative_rect,
        mode
      );

      rendering_cache.unsafe_set_visible_count(arbitrary_index_pos);

      relative_rect.w = placement_header.rect.w;
      relative_rect.x += relative_rect.w + ekg_pixel;

      if (relative_rect.x > relative_largest_rect.w) {
        relative_largest_rect.w = relative_rect.x;
      }

      /* get the largest visible size to scroll use as rect! */

      this->rect_content_place.h = arbitrary_index_pos * relative_rect.h; 
      if (this->rect_content_place.h > relative_largest_rect.h) {
        relative_largest_rect.h = this->rect_content_place.h;
        largest_rendering_cache_size = arbitrary_index_pos;
      }
    }

    relative_largest_rect.h;
    this->embedded_scroll.rect_child = relative_largest_rect;
    break;
  case false:
    break;
  }
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
  ekg::rect &rect {this->get_abs_rect()};
  ekg::ui::listbox *p_ui {static_cast<ekg::ui::listbox*>(this->p_data)};
  ekg::service::theme &theme {ekg::theme()};
  ekg::draw::font_renderer &f_renderer {ekg::f_renderer(p_ui->get_item_font_size())};

  /* The header offset transformed to content place! */

  this->rect_content_place.x = 0.0f;
  this->rect_content_place.h = rect.h - this->rect_content_place.y;
  this->rect_content_place.w = rect.w;

  /* Insert the absolute rect position to the place, the content abs is addressed to scrollbar! */

  this->rect_content_abs = this->rect_content_place + rect;
  this->embedded_scroll.clamp_scroll();

  ekg::draw::sync_scissor(
    this->scissor,
    rect,
    this->p_parent_scissor
  );
  ekg_draw_assert_scissor();

  ekg::draw::rect(
    rect,
    theme.listbox_background,
    ekg::draw_mode::filled,
    ekg_layer(ekg::layer::background)
  );

  ekg::rect scrollable_rect {this->rect_content_abs + this->embedded_scroll.scroll};
  ekg::rect item_rect {};
  ekg::rect scissor_store {};

  scrollable_rect.x = static_cast<int32_t>(scrollable_rect.x);
  scrollable_rect.y = static_cast<int32_t>(scrollable_rect.y);

  float top_place {this->rect_content_abs.y};
  float bottom_place {this->rect_content_abs.y + this->rect_content_abs.h};
  float header_relative_x {};

  uint16_t flags {};
  uint64_t visible_begin_index {};
  uint64_t visible_count {};

  bool scroll_on_top {this->embedded_scroll.scroll.y == 0.0f};
  float scrolling_cropy {
    ((-this->embedded_scroll.scroll.y) / this->embedded_scroll.rect_child.h)
  };

  for (uint64_t it_header {}; it_header < this->item_rendering_cache.size(); it_header++) {
    ekg::item &item_header {this->item_rendering_cache.at(it_header)};
    ekg::placement &placement_header {item_header.unsafe_get_placement()};

    item_rect = placement_header.rect + rect;
    flags = item_header.get_attr();

    item_rect.x = static_cast<int32_t>(item_rect.x);
    item_rect.y = static_cast<int32_t>(item_rect.y);

    /* fix scissor to draw the header */

    ekg::draw::sync_scissor(
      this->scissor,
      rect,
      this->p_parent_scissor
    );

    ekg::draw::rect(
      item_rect,
      theme.listbox_header_background,
      ekg::draw_mode::filled,
      ekg_layer(ekg::layer::background)
    );

    if (ekg_bitwise_contains(flags, ekg::attr::hovering)) {
      ekg::draw::rect(
        item_rect,
        theme.listbox_header_highlight,
        ekg::draw_mode::filled,
        ekg_layer(ekg::layer::highlight)
      );

      ekg::draw::rect(
        item_rect,
        theme.listbox_header_highlight_outline,
        ekg::draw_mode::outline,
        ekg_layer(ekg::layer::highlight)
      );
    }

    f_renderer.blit(
      item_header.get_value(),
      item_rect.x + placement_header.rect_text.x,
      item_rect.y + placement_header.rect_text.y,
      theme.listbox_header_string
    );

    ekg::draw::rect(
      item_rect,
      theme.listbox_header_outline,
      ekg::draw_mode::outline
    );

    visible_count = item_header.get_visible_count();
    visible_begin_index = (
      scroll_on_top ?
        0 :
        static_cast<uint64_t>(scrolling_cropy * visible_count)
    );

    /* fix scissor to draw items */

    ekg::draw::sync_scissor(
      this->scissor,
      this->rect_content_abs,
      this->p_parent_scissor
    );

    scissor_store = this->scissor;

    for (uint64_t it_item {visible_begin_index}; it_item < visible_count; it_item++) {
      ekg::item &item {item_header.at(it_item)};
      ekg::placement &placement {item.unsafe_get_placement()};

      placement.rect.w = placement_header.rect.w - (placement.offset * 2.0f);
      placement.rect.x = header_relative_x + placement.offset;

      item_rect = placement.rect + scrollable_rect;
      flags = item.get_attr();

      ekg::draw::sync_scissor(
        this->scissor,
        item_rect,
        &scissor_store
      );

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

      if (item_rect.y + item_rect.h > bottom_place) {
        break;
      }
    }

    header_relative_x += placement_header.rect.w;
  }

  ekg::draw::sync_scissor(
    this->scissor,
    rect,
    this->p_parent_scissor
  );

  this->embedded_scroll.scissor = this->scissor;
  this->embedded_scroll.on_draw_refresh();

  ekg::draw::rect(rect, theme.listbox_outline, ekg::draw_mode::outline);  
}

void ekg::ui::listbox_template_reload(
  ekg::item &parent,
  ekg::rect &ui_rect,
  ekg::rect &header_rect,
  ekg::font &item_font,
  ekg::rect &relative_rect,
  int32_t item_scaled_height,
  uint64_t header_index,
  uint64_t &arbitrary_index_pos,
  bool &opened,
  ekg::mode mode,
  bool *p_semaphore
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
  float text_height {f_renderer.get_text_height()};

  float dimension_offset {text_height / 2};
  float offset {};

  bool should_apply_offset_by_column_based {
    (mode == ekg::mode::multicolumn && header_index == 0) || mode == ekg::mode::singlecolumn
  };

  float additional_offset_by_column_based {
    (theme.listbox_subitem_offset_space + ekg_pixel) * should_apply_offset_by_column_based
  };

  for (it = it; it < parent.size(); it++) {
    ekg::item &item {parent.at(it)};
    ekg::placement &placement {item.unsafe_get_placement()};

    item.unsafe_set_semaphore(p_semaphore);
    item_rect = ui_rect + placement.rect;
    flags = item.get_attr();

    text_lines = 0;
    text_width = f_renderer.get_text_width(item.get_value(), text_lines);

    offset = ekg::find_min_offset(text_width, dimension_offset);
    placement.offset = ekg_min(relative_rect.x - header_rect.x, 0.0f);

    placement.rect.x = relative_rect.x;
    placement.rect.y = relative_rect.y;

    placement.rect.w = relative_rect.w;
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

    just_flagged_cursive_opened = opened;

    if (!opened && ekg_bitwise_contains(flags, ekg::attr::opened)) {
      just_flagged_cursive_opened = true;
      opened = true;
    }

    arbitrary_index_pos++;

    if (!item.empty()) {
      relative_rect.x += additional_offset_by_column_based;
      relative_rect.w -= additional_offset_by_column_based;

      ekg::ui::listbox_template_reload(
        item,
        ui_rect,
        header_rect,
        item_font,
        relative_rect,
        item_scaled_height,
        header_index,
        arbitrary_index_pos,
        opened,
        mode,
        p_semaphore
      );

      relative_rect.x -= additional_offset_by_column_based;
      relative_rect.w += additional_offset_by_column_based;
    }

    if (just_flagged_cursive_opened) {
      opened = false;
    }

    relative_rect.y -= (relative_rect.y - placement.rect.y) * !opened;
    relative_rect.y += placement.rect.h;
  }
}

void ekg::ui::listbox_template_on_event(
  ekg::os::io_event_serial &io_event_serial,
  uint64_t &arbitrary_index_pos,
  ekg::item &rendering_cache,
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
  uint64_t rendering_cache_size {rendering_cache.size()};

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
    relative_rect.h = placement.rect.h;
    relative_rect.y += placement.rect.h;

    if (arbitrary_index_pos < rendering_cache_size) {
      rendering_cache[arbitrary_index_pos] = item;
      rendering_cache[arbitrary_index_pos].unsafe_set_addressed(&item);
      arbitrary_index_pos++;
    }

    if (!item.empty() && ekg_bitwise_contains(item.get_attr(), ekg::attr::opened)) {
      ekg::ui::listbox_template_on_event(
        io_event_serial,
        arbitrary_index_pos,
        rendering_cache,
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