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

#include <algorithm>

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
  uint64_t highest_arbitrary_index_pos {};
  uint64_t rendering_cache_arbitrary_index_pos {};

  int32_t item_scaled_height {p_ui->get_item_scaled_height()};
  int32_t column_header_scaled_height {p_ui->get_column_header_scaled_height()};
  ekg::flags column_header_dock_flags {p_ui->get_column_header_align()};

  float text_width {};
  int32_t text_lines {};
  uint64_t items_header_size {p_ui->p_value->size()};
  uint64_t old_cache_size {};

  text_height = f_renderer_column_header.get_text_height();
  dimension_offset = text_height / 2;

  ekg::mode mode {p_ui->get_mode()};
  bool is_multicolumn {mode == ekg::mode::multicolumn};
  bool opened {};

  float scaled_width {rect.w};
  this->header_relative_x = 0.0f;

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
    rendering_cache_arbitrary_index_pos = 0;

    if (
        ekg_bitwise_contains(column_header_dock_flags, ekg::dock::top) ||
        !ekg_bitwise_contains(column_header_dock_flags, ekg::dock::bottom)
       ) {
      this->rect_content_place.y = placement.rect.h + ekg_pixel;
      this->column_header_height = placement.rect.h + ekg_pixel;
    }

    if (this->must_update_items) {
      if (it >= this->item_rendering_cache.size()) {
        this->item_rendering_cache.emplace_back();
      }

      ekg::item &rendering_cache {this->item_rendering_cache.at(it)};
      ekg::ui::listbox_template_reload(
        rendering_cache,
        item,
        rect,
        placement.rect,
        item_font,
        relative_rect,
        item_scaled_height,
        it,
        arbitrary_index_pos,
        rendering_cache_arbitrary_index_pos,
        opened,
        mode,
        &this->must_update_items
      );
    
      if (arbitrary_index_pos < rendering_cache.size()) {
        rendering_cache.erase(
          rendering_cache.begin() + arbitrary_index_pos + 1,
          rendering_cache.end()
        );
      } else {
        rendering_cache.resize(arbitrary_index_pos);
      }

      rendering_cache.unsafe_set_visible_count(rendering_cache_arbitrary_index_pos);
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

    ekg::item &rendering_cache {this->item_rendering_cache.at(it)};
    rendering_cache.unsafe_get_placement() = item.unsafe_get_placement();
    rendering_cache.set_value(item.get_value());
    rendering_cache.set_attr(item.get_attr());
    rendering_cache.set_text_align(item.get_text_align());

    this->header_relative_x += placement.rect.w + ekg_pixel;
    relative_rect.x += relative_rect.w + ekg_pixel;

    if (relative_rect.x > relative_largest_rect.w) {
      relative_largest_rect.w = relative_rect.x;
    }

    /* get the largest visible size to scroll use as rect! */

    if (rendering_cache_arbitrary_index_pos > highest_arbitrary_index_pos) {
      highest_arbitrary_index_pos = rendering_cache_arbitrary_index_pos;
    }
  }

  if (this->must_update_items) {
    relative_largest_rect.h = (
      (highest_arbitrary_index_pos * relative_rect.h) + this->embedded_scroll.rect_horizontal_scroll_bar.h + ekg_pixel 
    );

    this->embedded_scroll.rect_child = relative_largest_rect;
    std::cout << highest_arbitrary_index_pos << std::endl;
  } else {
    this->embedded_scroll.rect_child.w = relative_largest_rect.w;
  }

  this->embedded_scroll.acceleration.y = (text_height * 3.0f) + (offset * 2.0f);
  this->embedded_scroll.p_rect_mother = &this->rect_content_abs;
  this->embedded_scroll.widget_id = this->p_data->get_id();
  this->embedded_scroll.on_reload();
  this->must_update_items = false;
}

void ekg::ui::listbox_widget::on_pre_event(ekg::os::io_event_serial &io_event_serial) {
  abstract_widget::on_pre_event(io_event_serial);

  this->embedded_scroll.on_pre_event(io_event_serial);

  /**
   * We do not want the scroll to interfere with some header target action.
   **/
  if (!this->flag.extra_state) {
    this->flag.absolute = (
      this->embedded_scroll.is_dragging_bar() ||
      this->embedded_scroll.flag.activity ||
      this->flag.state
    );
  }
}

void ekg::ui::listbox_widget::on_event(ekg::os::io_event_serial &io_event_serial) {
  bool pressed_open {ekg::input::action("listbox-activity-open")};
  bool pressed_select_many {ekg::input::action("listbox-activity-select-many")};
  bool pressed_select {ekg::input::action("listbox-activity-select")};
  bool released {ekg::input::released()};
  bool motion {ekg::input::motion()};

  bool is_some_header_targeted {
    this->flag.extra_state
  };

  this->embedded_scroll.on_event(io_event_serial);

  if ((this->flag.focused || this->flag.hovered || (this->flag.absolute && !is_some_header_targeted)) && !this->is_high_frequency) {
    ekg::update_high_frequency(this);
  }

  if (is_some_header_targeted && released) {
    this->latest_target_dragging = this->target_dragging;
    this->target_resizing = -1;
    this->target_dragging = -1;
    this->targeting_header_to_resize = -1;
    this->targeting_header_to_drag = -1;
    this->flag.extra_state = false;
    this->flag.absolute = false;
  }

  if (
      /**
       * This one checks if an unnecessary operation is going on,
       * if all input actions are not flagged, there is no reason to waste calls.
       **/
      (!motion && !released && !pressed_select && !pressed_open && !pressed_select_many) ||
      /**
       * If input interact position is not colliding with the listbox,
       * there is no reason to waste calls; but with exception of absolute flag,
       * because when a header is targeted the absolute flag is set to true,
       * but if the interact position is out of listbox boudings, it must keep
       * process the header targeted (drag or resize).
       **/
      (!this->flag.hovered && !this->was_hovered && !is_some_header_targeted && !this->was_selected) ||
      /**
       * Also stop the process if scroll bar is hovered or if there is an update
       * items. 
       **/
      (this->embedded_scroll.flag.hovered || this->must_update_items) ||
      /**
       * The absolute flag is also used to do scrolling (when dragging the scroll bar)
       * then we must stop the process instead of waste calls; but with exception of
       * header target, must not stop like previously said.
       **/
      (this->flag.absolute && !is_some_header_targeted)
    ) {
    return;
  }

  if (!this->flag.extra_state) {
    is_some_header_targeted = false;
  }

  this->was_hovered = this->flag.hovered;
  ekg::rect &rect {this->get_abs_rect()};
  this->was_selected = false;

  ekg::rect relative_largest_rect {};
  ekg::rect relative_rect {};
  ekg::rect scrollable_rect {this->rect_content_abs + this->embedded_scroll.scroll};
  ekg::vec2 ui_pos {rect.x, rect.y};
  ekg::rect delta_rect {};
  ekg::rect item_rect {};
  ekg::vec4 &interact {ekg::input::interact()};
  ekg::vec4 point {};

  ekg::ui::listbox *p_ui {static_cast<ekg::ui::listbox*>(this->p_data)};
  ekg::mode mode {p_ui->get_mode()};

  bool is_dragging_or_resizing {};
  bool is_multicolumn {mode == ekg::mode::multicolumn};
  bool must {};
  bool hovering {};
  bool contains_flag {};

  uint64_t arbitrary_index_pos {};
  uint64_t highest_arbitrary_index_pos {};
  ekg::flags flags {};
  uint64_t visible_begin_index {};
  uint64_t visible_count {};
  uint64_t size {};

  float bottom_place {this->rect_content_abs.y + this->rect_content_abs.h};

  is_some_header_targeted = (
    is_some_header_targeted || ((this->targeting_header_to_resize != -1 || this->targeting_header_to_drag != -1) && pressed_select)
  );

  this->header_relative_x = 0.0f;

  switch (pressed_open || is_some_header_targeted || this->latest_target_dragging != -1) {
  case true:
    size = p_ui->p_value->size();
    ekg::ui::redraw = true;

    if (this->latest_target_dragging != -1) {
      uint64_t set_header_new_index {static_cast<uint64_t>(this->latest_target_dragging)};

      point.x = ekg_clamp(interact.x, rect.x, rect.x + rect.w);

      for (uint64_t it {}; it < size; it++) {
        ekg::item &item_header {p_ui->p_value->at(it)};
        ekg::placement &placement_header {item_header.unsafe_get_placement()};

        item_rect = placement_header.rect + rect;
        item_rect.x = item_rect.x + this->embedded_scroll.scroll.x;

        point.y = ekg_clamp(interact.y, item_rect.y, item_rect.y + this->rect_original_dragging_targeted_header.h);
        if (ekg::rect_collide_vec(item_rect, point) && this->latest_target_dragging != it) {
          set_header_new_index = it;
          break;
        }
      }

      if (set_header_new_index != this->latest_target_dragging) {
        std::swap(
          p_ui->p_value->at(this->latest_target_dragging),
          p_ui->p_value->at(set_header_new_index)
        );
      }

      this->latest_target_dragging = -1;
    }

    for (uint64_t it {}; it < size; it++) {
      relative_rect.y = 0.0f;
      arbitrary_index_pos = 0;

      ekg::item &rendering_cache {this->item_rendering_cache.at(it)};
      ekg::item &item_header {p_ui->p_value->at(it)};
      ekg::placement &placement_header {item_header.unsafe_get_placement()};

      placement_header.rect.x = this->header_relative_x;

      if (this->targeting_header_to_resize == it && this->target_resizing == -1) {
        this->target_resizing = this->targeting_header_to_resize;
        this->rect_header_delta.x = (this->rect_targeting_header.x + this->rect_targeting_header.w) - interact.x;
      }

      if (this->target_resizing == static_cast<int32_t>(it)) {
        placement_header.rect.w = ekg_min((interact.x + this->rect_header_delta.x) - this->rect_targeting_header.x, placement_header.rect.h);
        ekg::cursor = ekg::system_cursor::size_we;
        ekg::ui::redraw = true;
        this->flag.absolute = true;
      }

      if (this->targeting_header_to_drag == it && this->target_dragging == -1 && this->target_resizing == -1) {
        this->target_dragging = this->targeting_header_to_drag;
        this->rect_header_delta.x = interact.x - this->rect_targeting_header.x;
        this->rect_original_dragging_targeted_header = this->rect_targeting_header;
      }

      if (this->target_dragging == it && this->target_resizing == -1) {
        placement_header.rect.x = interact.x - this->rect_header_delta.x;
        this->rect_current_dragging_targeted_header = placement_header.rect;
        ekg::cursor = ekg::system_cursor::size_all;
        ekg::ui::redraw = true;
        this->flag.absolute = true;
      }

      is_dragging_or_resizing = (
        this->flag.extra_state = this->target_resizing != -1 || this->target_dragging != -1
      );

      delta_rect.x = rect.x;
      delta_rect.w = rect.w;
      delta_rect.h = item_rect.y;
      delta_rect.h = item_rect.h;

      must = (
        ekg::rect_collide_vec(delta_rect, interact)
      );

      // there is no reason for calculate items if interact is hovering the header
      if (must) {
        continue;
      }

      /**
       * Do not copy all the content by simple one reason:
       * Copying the the major header from `p_value` to rendering cache element,
       * erase ALL reserved memory from reload, re-reserving is performanceless here.
       * 
       * Passing only necessary stuff (attributes, value, etc). 
       **/

      rendering_cache.unsafe_get_placement() = placement_header;
      rendering_cache.set_value(item_header.get_value());
      rendering_cache.set_attr(item_header.get_attr());
      rendering_cache.set_text_align(item_header.get_text_align());

      if (!is_dragging_or_resizing) {
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
      }

      relative_rect.w = placement_header.rect.w;
      this->header_relative_x += relative_rect.w + ekg_pixel;
      relative_rect.x += relative_rect.w + !is_multicolumn;

      if (relative_rect.x > relative_largest_rect.w) {
        relative_largest_rect.w = relative_rect.x;
      }

      /* get the largest visible size to scroll use as rect! */

      if (arbitrary_index_pos > highest_arbitrary_index_pos) {
        highest_arbitrary_index_pos = arbitrary_index_pos;
      }
    }

    if (is_dragging_or_resizing) {
      this->embedded_scroll.rect_child.w = relative_largest_rect.w;
    } else {
      relative_largest_rect.h = (
        (highest_arbitrary_index_pos * relative_rect.h) + this->embedded_scroll.rect_horizontal_scroll_bar.h + ekg_pixel
      );
  
      this->embedded_scroll.rect_child = relative_largest_rect;
    }

    break;
  case false:
    ekg::ui::redraw = true;

    float between_headers_target_resize {ekg_pixel * 4};
    float normalized_horizontal_scroll {this->embedded_scroll.get_normalized_vertical_scroll()};

    this->targeting_header_to_resize = -1;
    this->targeting_header_to_drag = -1;
    int32_t some_targeted_header {-1};
    uint64_t rendering_cache_size {this->item_rendering_cache.size()};

    for (uint64_t it_header {}; it_header < rendering_cache_size; it_header++) {
      ekg::item &item_header {this->item_rendering_cache.at(it_header)};
      ekg::placement &placement_header {item_header.unsafe_get_placement()};

      item_rect = placement_header.rect + rect;
      item_rect.x = item_rect.x + this->embedded_scroll.scroll.x;

      flags = item_header.get_attr();
      delta_rect = item_rect;
      delta_rect.x = delta_rect.x + delta_rect.w - between_headers_target_resize;
      delta_rect.w = between_headers_target_resize + between_headers_target_resize;

      must = (
        some_targeted_header == -1 && !this->flag.extra_state && ekg::rect_collide_vec(delta_rect, interact)
      );

      if (must) {
        some_targeted_header = static_cast<int32_t>(it_header);
        this->rect_targeting_header = item_rect;
        this->targeting_header_to_resize = static_cast<int32_t>(it_header);
        ekg::cursor = ekg::system_cursor::size_we;
      }

      delta_rect = item_rect;
      must = (
        !must && some_targeted_header == -1 && ekg::rect_collide_vec(delta_rect, interact) && rendering_cache_size > 1 
      ); // `!must &&` because we do not want to interfe in resize targeting. 

      if (must) {
        some_targeted_header = static_cast<int32_t>(it_header);
        this->rect_targeting_header = item_rect;
        this->targeting_header_to_drag = static_cast<int32_t>(it_header);
      }

      visible_count = item_header.get_visible_count();
      visible_begin_index = ekg::get_index_by_normalized_scroll(normalized_horizontal_scroll, visible_count);

      delta_rect.x = rect.x;
      delta_rect.w = rect.w;
      delta_rect.h = item_rect.y;
      delta_rect.h = item_rect.h;

      must = (
        ekg::rect_collide_vec(delta_rect, interact)
      );

      // there is no reason for calculate items if interact is hovering the header
      if (must) {
        continue;
      }

      for (uint64_t it_item {visible_begin_index}; it_item < visible_count; it_item++) {
        ekg::item &item {item_header.at(it_item)};
        ekg::placement &placement {item.unsafe_get_placement()};

        item_rect = placement.rect + scrollable_rect;
        flags = item.get_attr();

        switch (mode) {
        case ekg::mode::multicolumn:
          item_rect.w = rect.w;
          item_rect.x = rect.x;
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

        if (hovering && pressed_select_many && this->was_hovered) {
          contains_flag = static_cast<bool>(ekg_bitwise_contains(flags, ekg::attr::focused));
          ekg_bitwise_remove(flags, ekg::attr::focused * contains_flag);
          ekg_bitwise_add(flags, ekg::attr::focused * !contains_flag);
          item.set_attr(flags);
        } else if (pressed_select && hovering && !ekg_bitwise_contains(flags, ekg::attr::focused) && this->was_hovered) {
          ekg_bitwise_add(flags, ekg::attr::focused);
          item.set_attr(flags);
        } else if (!hovering && pressed_select && ekg_bitwise_contains(flags, ekg::attr::focused) && this->was_hovered) {
          ekg_bitwise_remove(flags, ekg::attr::focused);
          item.set_attr(flags);
        }

        if (ekg_bitwise_contains(flags, ekg::attr::focused)) {
          was_selected = true;
        }

        item.unsafe_get_addressed()->set_attr(item.get_attr());
        item.unsafe_get_addressed()->unsafe_get_placement() = item.unsafe_get_placement();

        if (item_rect.y + item_rect.h > bottom_place) {
          break;
        }
      }

      this->header_relative_x += placement_header.rect.w;
    }

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
  this->is_high_frequency = this->embedded_scroll.check_activity_state(this->flag.absolute || this->flag.focused || this->flag.hovered);
}

void ekg::ui::listbox_widget::on_draw_refresh() {
  ekg::rect &rect {this->get_abs_rect()};
  ekg::ui::listbox *p_ui {static_cast<ekg::ui::listbox*>(this->p_data)};
  ekg::service::theme_scheme_t &theme_scheme {ekg::current_theme_scheme()};
  ekg::draw::font_renderer &f_renderer {ekg::f_renderer(p_ui->get_item_font_size())};

  if (this->must_update_items) {
    ekg::reload(this);
  }

  /* The header offset transformed to content place! */

  this->rect_content_place.x = 0.0f;
  this->rect_content_place.h = rect.h - this->rect_content_place.y - this->embedded_scroll.rect_horizontal_scroll_bar.h;
  this->rect_content_place.w = rect.w - this->embedded_scroll.rect_vertical_scroll_bar.w;

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
    theme_scheme.listbox_background,
    ekg::draw_mode::filled,
    ekg_layer(ekg::layer::background)
  );

  ekg::rect scrollable_rect {this->rect_content_abs + this->embedded_scroll.scroll};
  ekg::rect item_rect {};
  ekg::rect content_scissor_bounding {};
  ekg::rect widget_absolute_rect_scissor {this->scissor};

  scrollable_rect.x = static_cast<int32_t>(scrollable_rect.x);
  scrollable_rect.y = static_cast<int32_t>(scrollable_rect.y);

  float bottom_place {this->rect_content_abs.y + this->rect_content_abs.h};

  ekg::flags flags {p_ui->get_column_header_align()};
  uint64_t rendering_cache_size {this->item_rendering_cache.size()};

  bool must_stop_rendering {};
  bool is_header_targeted {};

  bool is_column_header_top {
    ekg_bitwise_contains(flags, ekg::dock::top) ||
    !ekg_bitwise_contains(flags, ekg::dock::bottom)
  };

  float normalized_horizontal_scroll {this->embedded_scroll.get_normalized_vertical_scroll()};
  this->header_relative_x = 0.0f;

  for (uint64_t it_header {}; it_header < rendering_cache_size; it_header++) {
    ekg::item &item_header {this->item_rendering_cache.at(it_header)};
    ekg::placement &placement_header {item_header.unsafe_get_placement()};

    is_header_targeted = this->target_dragging == static_cast<int32_t>(it_header);

    if (is_header_targeted) {
      this->header_relative_x += placement_header.rect.w + ekg_pixel;
      continue;
    }

    this->render_item(
      item_header,
      placement_header,
      widget_absolute_rect_scissor,
      scrollable_rect,
      content_scissor_bounding,
      bottom_place,
      normalized_horizontal_scroll,
      is_header_targeted,
      is_column_header_top,
      f_renderer
    );

    this->header_relative_x += placement_header.rect.w + ekg_pixel;
  }

  ekg::draw::sync_scissor(
    this->scissor,
    rect,
    this->p_parent_scissor
  );

  /* instead of rendering the scroll content with a reduced borders, we re-clamp value to the widget abs-rect boundings */

  this->rect_content_abs.h = rect.h - this->rect_content_place.y;
  this->rect_content_abs.x = rect.x;
  this->rect_content_abs.w = rect.w;

  this->embedded_scroll.scissor = this->scissor;
  this->embedded_scroll.on_draw_refresh();

  ekg::draw::rect(
    rect,
    theme_scheme.listbox_outline,
    ekg::draw_mode::outline
  );

  if (this->target_dragging != -1 && this->target_dragging < rendering_cache_size) {
    ekg::item &item_header {this->item_rendering_cache.at(this->target_dragging)};
    ekg::placement &placement_header {item_header.unsafe_get_placement()};

    ekg::draw::enable_high_priority();

    is_header_targeted = true;
    this->render_item(
      item_header,
      placement_header,
      widget_absolute_rect_scissor,
      scrollable_rect,
      content_scissor_bounding,
      bottom_place,
      normalized_horizontal_scroll,
      is_header_targeted,
      is_column_header_top,
      f_renderer
    );

    ekg::draw::disable_high_priority();
  }
}

void ekg::ui::listbox_widget::render_item(
  ekg::item &item_header,
  ekg::placement &placement_header,
  ekg::rect widget_absolute_rect_scissor,
  ekg::rect scrollable_rect,
  ekg::rect content_scissor_bounding,
  float bottom_place,
  float normalized_horizontal_scroll,
  bool is_header_targeted,
  bool is_column_header_top,
  ekg::draw::font_renderer &f_renderer
) {
  ekg::service::theme_scheme_t &theme_scheme {ekg::current_theme_scheme()};
  ekg::rect &rect {this->get_abs_rect()};
  ekg::rect item_rect {};
  ekg::flags flags {item_header.get_attr()};
  bool must_stop_rendering {};
  ekg::rect targeted_rect {this->rect_current_dragging_targeted_header};
  targeted_rect.y += rect.y;

  if (is_header_targeted) {
    item_rect = targeted_rect;
    item_rect.h = rect.h;

    ekg::draw::rect(
      item_rect,
      theme_scheme.listbox_drag_background,
      ekg::draw_mode::filled,
      ekg_layer(ekg::layer::background)
    );

    ekg::draw::rect(
      item_rect,
      theme_scheme.listbox_drag_outline,
      ekg::draw_mode::outline
    );

    item_rect = targeted_rect;
  } else {
    item_rect = placement_header.rect + rect;
    item_rect.x = item_rect.x + this->embedded_scroll.scroll.x;
  }

  ekg::draw::sync_scissor(
    this->scissor,
    item_rect,
    &widget_absolute_rect_scissor
  );

  ekg::draw::rect(
    item_rect,
    theme_scheme.listbox_header_background,
    ekg::draw_mode::filled,
    ekg_layer(ekg::layer::background)
  );

  if (ekg_bitwise_contains(flags, ekg::attr::hovering)) {
    ekg::draw::rect(
      item_rect,
      theme_scheme.listbox_header_highlight,
      ekg::draw_mode::filled,
      ekg_layer(ekg::layer::highlight)
    );

    ekg::draw::rect(
      item_rect,
      theme_scheme.listbox_header_highlight_outline,
      ekg::draw_mode::outline,
      ekg_layer(ekg::layer::highlight)
    );
  }

  f_renderer.blit(
    item_header.get_value(),
    item_rect.x + placement_header.rect_text.x,
    item_rect.y + placement_header.rect_text.y,
    theme_scheme.listbox_header_string
  );

  ekg::draw::rect(
    item_rect,
    theme_scheme.listbox_header_outline,
    ekg::draw_mode::outline
  );

  ekg::draw::sync_scissor(
    this->scissor,
    this->rect_content_abs,
    this->p_parent_scissor
  );

  content_scissor_bounding = this->scissor;

  uint64_t visible_count {item_header.get_visible_count()};
  uint64_t visible_begin_index {ekg::get_index_by_normalized_scroll(normalized_horizontal_scroll, visible_count)};

  for (uint64_t it_item {visible_begin_index}; it_item < visible_count; it_item++) {
    ekg::item &item {item_header.at(it_item)};
    ekg::placement &placement {item.unsafe_get_placement()};

    placement.rect.w = placement_header.rect.w - placement.offset;
    flags = item.get_attr();

    if (is_header_targeted) {
      item_rect = placement.rect + scrollable_rect;
      item_rect.x = targeted_rect.x + placement.offset;
    } else {
      placement.rect.x = this->header_relative_x + placement.offset; // must update the size when resize a header
      item_rect = placement.rect + scrollable_rect;
    }

    /**
     * The point of adding one pixel on width, fix the rendering of
     * offset between headers space. The header is not the same
     * size of items for highlight & select, so the item must be
     * complete filled and not remains the offset header.
     **/
    item_rect.w += ekg_pixel;

    ekg::draw::sync_scissor(
      this->scissor,
      item_rect,
      &content_scissor_bounding
    );

    must_stop_rendering = item_rect.y + item_rect.h > bottom_place;

    ekg::draw::rect(
      item_rect,
      theme_scheme.listbox_item_background,
      ekg::draw_mode::filled,
      ekg_layer(ekg::layer::sub_background)
    );

    if (ekg_bitwise_contains(flags, ekg::attr::hovering)) {
      ekg::draw::rect(
        item_rect,
        theme_scheme.listbox_item_highlight,
        ekg::draw_mode::filled,
        ekg_layer(ekg::layer::highlight)
      );

      ekg::draw::rect(
        item_rect,
        theme_scheme.listbox_item_highlight_outline,
        ekg::draw_mode::outline
      );
    }

    if (ekg_bitwise_contains(flags, ekg::attr::focused)) {
      ekg::draw::rect(
        item_rect,
        theme_scheme.listbox_item_focused,
        ekg::draw_mode::filled,
        ekg_layer(ekg::layer::activity)
      );

      ekg::draw::rect(
        item_rect,
        theme_scheme.listbox_item_highlight_outline,
        ekg::draw_mode::outline
      );
    }

    f_renderer.blit(
      item.get_value(),
      item_rect.x + placement.rect_text.x,
      item_rect.y + placement.rect_text.y,
      theme_scheme.listbox_item_string
    );

    ekg::draw::rect(
      item_rect,
      theme_scheme.listbox_item_outline,
      ekg::draw_mode::outline
    );

    if (must_stop_rendering) {
      break;
    }
  }

  if (!is_header_targeted) {
    switch (is_column_header_top) {
      case true:
        item_rect = placement_header.rect + rect;
        item_rect.x = static_cast<int32_t>(item_rect.x + this->embedded_scroll.scroll.x) - ekg_pixel;
        item_rect.y = static_cast<int32_t>(item_rect.y + item_rect.h);
        item_rect.w = ekg_pixel;
        item_rect.h = rect.h;

        ekg::draw::sync_scissor(
          this->scissor,
          item_rect,
          &widget_absolute_rect_scissor
        );

        ekg::draw::rect(
          item_rect,
          theme_scheme.listbox_line_separator,
          ekg::draw_mode::filled
        );
      break;
    }
  }
}

void ekg::ui::listbox_extent_align(ekg::rect &rect) {
  rect.x -= ekg_pixel;
  rect.w += ekg_pixel;
}

void ekg::ui::listbox_template_reload(
  ekg::item &rendering_cache,
  ekg::item &parent,
  ekg::rect &ui_rect,
  ekg::rect &header_rect,
  ekg::font &item_font,
  ekg::rect &relative_rect,
  int32_t item_scaled_height,
  uint64_t header_index,
  uint64_t &arbitrary_index_pos,
  uint64_t &rendering_cache_arbitrary_index_pos,
  bool &opened,
  ekg::mode mode,
  bool *p_semaphore
) {
  ekg::layout::mask &mask {ekg::core->mask};
  ekg::draw::font_renderer &f_renderer {ekg::f_renderer(item_font)};
  ekg::rect item_rect {};
  ekg::service::theme_scheme_t &theme_scheme {ekg::current_theme_scheme()};

  uint64_t it {};
  int32_t text_lines {};
  ekg::flags flags {};

  bool is_empty {};
  bool is_major_column {arbitrary_index_pos == 0};
  bool is_opened_flagged {};

  float text_width {};
  float text_height {f_renderer.get_text_height()};

  float dimension_offset {text_height / 2};
  float offset {};

  bool should_apply_offset_by_column_based {
    (mode == ekg::mode::multicolumn && header_index == 0) || mode == ekg::mode::singlecolumn
  };

  float additional_offset_by_column_based {
    (theme_scheme.listbox_subitem_offset_space + ekg_pixel) * should_apply_offset_by_column_based
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

    placement.rect.w = relative_rect.w - placement.offset;
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

    relative_rect.h = placement.rect.h;

    if (is_major_column || opened) {
      if (rendering_cache_arbitrary_index_pos >= rendering_cache.size()) {
        rendering_cache.emplace_back();
      }

      rendering_cache[rendering_cache_arbitrary_index_pos].unsafe_get_placement() = placement;
      rendering_cache[rendering_cache_arbitrary_index_pos].set_value(item.get_value());
      rendering_cache[rendering_cache_arbitrary_index_pos].set_attr(item.get_attr());
      rendering_cache[rendering_cache_arbitrary_index_pos].set_text_align(item.get_text_align());
      rendering_cache[rendering_cache_arbitrary_index_pos].unsafe_set_addressed(&item);
      rendering_cache_arbitrary_index_pos++;
    }

    is_opened_flagged = ekg_bitwise_contains(flags, ekg::attr::opened);
    is_empty = item.empty();

    opened = is_opened_flagged;
    arbitrary_index_pos++;

    if (!is_empty) {
      relative_rect.x += additional_offset_by_column_based;
      //relative_rect.w -= additional_offset_by_column_based;

      ekg::ui::listbox_template_reload(
        rendering_cache,
        item,
        ui_rect,
        header_rect,
        item_font,
        relative_rect,
        item_scaled_height,
        header_index,
        arbitrary_index_pos,
        rendering_cache_arbitrary_index_pos,
        opened,
        mode,
        p_semaphore
      );

      relative_rect.x -= additional_offset_by_column_based;
      //relative_rect.w += additional_offset_by_column_based;
    }

    if (is_opened_flagged) {
      opened = 0;
    }

    relative_rect.y -= (relative_rect.y - placement.rect.y) * !is_opened_flagged;
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
  ekg::flags flags {};
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

    if (!item.empty() && hovering && pressed_open) {
      contains_flag = static_cast<bool>(ekg_bitwise_contains(flags, ekg::attr::opened));
      ekg_bitwise_remove(flags, ekg::attr::opened * contains_flag);
      ekg_bitwise_add(flags, ekg::attr::opened * !contains_flag);
      item.set_attr(flags);
    }

    if (ekg_bitwise_contains(flags, ekg::attr::focused)) {
      was_selected = true;
    }

    placement.rect.y = relative_rect.y;
    relative_rect.h = placement.rect.h;
    relative_rect.y += placement.rect.h;

    if (arbitrary_index_pos < rendering_cache_size) {
      rendering_cache[arbitrary_index_pos].unsafe_get_placement() = placement;
      rendering_cache[arbitrary_index_pos].set_value(item.get_value());
      rendering_cache[arbitrary_index_pos].set_attr(item.get_attr());
      rendering_cache[arbitrary_index_pos].set_text_align(item.get_text_align());
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