/**
 * MIT License
 * 
 * Copyright (c) 2022-2025 Rina Wilk / vokegpu@gmail.com
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
#include "ekg/ui/scrollbar/widget.hpp"
#include "ekg/ui/textbox/widget.hpp"
#include "ekg/layout/docknize.hpp"
#include "ekg/io/log.hpp"
#include "ekg/ui/abstract.hpp"
#include "ekg/draw/shape/shape.hpp"
#include "ekg/draw/typography/font.hpp"
#include "ekg/core/context.hpp"
#include "ekg/core/runtime.hpp"
#include "ekg/core/pools.hpp"
#include "ekg/math/floating_point.hpp"

void ekg::ui::refresh_cursors_pos(
  ekg::textbox_t &textbox,
  ekg::textbox_t::cursor_t &origin,
  const ekg::vec2_t<size_t> &displacement_a,
  const ekg::vec2_t<size_t> &displacement_b,
  const ekg::ui::textbox_operation &operation
) {
  for (ekg::textbox_t::cursor_t &cursor : textbox.widget.cursors) {
    if (cursor.is_ignored || cursor.is_deleted || cursor.a.y < origin.a.y) continue;

    switch (operation) {
    case ekg::ui::textbox_operation::insert_line:
      if (cursor.a.y > origin.b.y) {
        cursor.a.y += displacement_a.y;
        cursor.b.y += displacement_b.y;
      }

      if (cursor.a.y == origin.b.y && cursor.a.x > origin.b.x) {
        cursor.a.x += displacement_a.x;
        cursor.a.y += displacement_a.y;
      }

      if (cursor.b.y == origin.b.y && cursor.b.x > origin.b.x) {
        cursor.b.x += displacement_b.x;
        cursor.b.y += displacement_b.y;
      }
      break;
    case ekg::ui::textbox_operation::insert_inline:
      if (cursor.a.y == origin.a.y && cursor.a.x > origin.a.x) {
        cursor.a.x += displacement_a.x;
      }

      if (cursor.b.y == origin.a.y && cursor.b.x > origin.a.x) {
        cursor.b.x += displacement_b.x;
      }
      break;
    case ekg::ui::textbox_operation::insert_multiline:
      if (cursor.a.y > origin.a.y) {
        cursor.a.y += displacement_a.y;
        cursor.b.y += displacement_b.y;
      }

      if (cursor.a.y == origin.a.y && cursor.a.x > origin.a.x) {
        cursor.a.x = (origin.b.x > origin.a.x)
          ? cursor.a.x + (origin.b.x - origin.a.x) 
          : cursor.a.x - (origin.a.x - origin.b.x);
        cursor.a.y += displacement_a.y;
      }

      if (cursor.b.y == origin.a.y && cursor.b.x > origin.a.x) {
        cursor.b.x = (origin.b.x > origin.a.x)
          ? cursor.b.x + (origin.b.x - origin.a.x)
          : cursor.b.x - (origin.a.x - origin.b.x);
        cursor.b.y += displacement_b.y;
      }

      break;
    case ekg::ui::textbox_operation::erase_inline:
      if (cursor.a.y > origin.b.y) {
        cursor.a.y -= displacement_a.y;
        cursor.b.y -= displacement_b.y;
      }

      if (
        cursor.a.y == origin.a.y
        &&
        cursor.a.x > origin.a.x
      ) {
        cursor.a.x = cursor.a.x - displacement_a.x;
        cursor.a.y -= displacement_a.y;
      }

      if (
        cursor.b.y == origin.b.y
        &&
        cursor.b.x > origin.b.x
      ) { 
        cursor.b.x = cursor.b.x - displacement_b.x;
        cursor.b.y -= displacement_b.y;
      }
      break;
    case ekg::ui::textbox_operation::erase_multiline:
      if (cursor.a.y > origin.b.y) {
        cursor.a.y -= displacement_a.y;
        cursor.b.y -= displacement_b.y;
      }

      if (
        cursor.a.y == origin.b.y
        &&
        cursor.a.x > origin.b.x
      ) {
        cursor.a.x = (origin.a.x < origin.b.x) ?
          cursor.a.x - (origin.b.x - origin.a.x)
          :
          cursor.a.x + (origin.a.x - origin.b.x);
        cursor.a.y -= displacement_a.y;
      }

      if (
        cursor.b.y == origin.b.y
        &&
        cursor.b.x > origin.b.x
      ) {
        cursor.b.x = (origin.a.x < origin.b.x) ?
          cursor.b.x - (origin.b.x - origin.a.x)
          :
          cursor.b.x + (origin.a.x - origin.b.x);
        cursor.b.y -= displacement_b.y;
      }
      break;
    }
  }
}

void ekg::ui::refresh_scroll_positions(
  ekg::textbox_t &textbox
) {
  size_t highest_size {};
  for (ekg::io::chunk_t &chunk : textbox.text.chunks_data()) {
    for (std::string &line : chunk) {
      highest_size = ekg::max(line.size(), highest_size);
    }
  };

  textbox.widget.scrollbar.rect.w = highest_size * (textbox.widget.rect_text_size.h * 0.6f);
  textbox.widget.scrollbar.rect.h = textbox.text.length_of_lines() * textbox.widget.rect_text_size.h;
}

bool ekg::ui::find_cursor(
  ekg::textbox_t &textbox,
  ekg::vec2_t<size_t> &index,
  ekg::textbox_t::cursor_t &cursor_out
) {
  if (cursor_out != ekg::vec2_t<size_t>(0, 0) && cursor_out >= index && cursor_out <= index) {
    return true;
  }

  for (ekg::textbox_t::cursor_t &cursor : textbox.widget.cursors) {
    if (cursor >= index && cursor <= index) {
      cursor_out = cursor;
      return true;
    }
  }

  return false;
}

bool ekg::ui::find_index_by_interact(
  ekg::property_t &property,
  ekg::textbox_t &textbox,
  ekg::vec2_t<float> &interact,
  ekg::vec2_t<size_t> &index
) {
  ekg::rect_t<float> &rect_abs {
    ekg::ui::get_abs_rect(property, textbox.rect)
  };

  ekg::draw::font &draw_font {
    ekg::draw::get_font_renderer(textbox.font_size)
  };

  ekg::io::font_face_t &text_font_face {draw_font.faces[ekg::io::font_face_type::text]};
  ekg::io::font_face_t &emojis_font_face {draw_font.faces[ekg::io::font_face_type::emojis]};
  ekg::io::font_face_t &kanjis_font_face {draw_font.faces[ekg::io::font_face_type::kanjis]};

  FT_Face ft_face {};
  FT_Vector ft_vector_previous_char {};
  char32_t ft_uint_previous {};

  size_t chunk_size {};
  size_t il {};
  size_t sum_chunk_sizes {};
  size_t text_total_lines {textbox.text.length_of_lines()};

  ekg::vec2_t<float> rendered {};
  ekg::rect_t<float> rect {};

  size_t text_len {};
  size_t utf8_text_len {};
  uint8_t uc8 {};
  char32_t c32 {};

  std::list<ekg::io::chunk_t> &chunks {textbox.text.chunks_data()};
  size_t chunks_size {chunks.size()};

  index.y += textbox.widget.view_line_index;
  textbox.widget.scrollbar.rect.h = text_total_lines * textbox.widget.rect_text_size.h;

  ekg::vec2_t<float> pos {
    0.0f,
    0.0f
  };

  float visible_text_height {static_cast<float>(textbox.widget.view_line_index * textbox.widget.rect_text_size.h)};
  pos.y = textbox.widget.scrollbar_property.scroll.position.y + visible_text_height;
  pos.y = static_cast<float>(static_cast<int32_t>(floorf(pos.y)));

  std::string empty {"\n"};
  bool is_empty {};

  il = textbox.widget.view_chunk_line_index;
  for (std::list<ekg::io::chunk_t>::iterator ic {textbox.widget.view_chunk_it}; ic != chunks.end(); ic++) {
    ekg::io::chunk_t &chunk {*ic};

    chunk_size = chunk.size();
    for (;il < chunk_size; il++) {
      std::string &chunk_line {chunk.at(il)};
      std::string &line {(is_empty = chunk_line.empty()) ? empty : chunk_line};

      text_len = line.size();
      for (size_t it {}; it < text_len; it++) {
        uc8 = static_cast<uint8_t>(line.at(it));

        ekg::utf8_sequence(
          uc8,
          c32,
          line,
          it
        );

        if (draw_font.ft_bool_kerning && ft_uint_previous) {
          switch (c32 < 256 || !emojis_font_face.was_loaded) {
            case true: {
              ft_face = text_font_face.ft_face;
              break;
            }

            default: {
              ft_face = emojis_font_face.ft_face;
              break;
            }
          }

          FT_Get_Kerning(ft_face, ft_uint_previous, c32, 0, &ft_vector_previous_char);
          pos.x += static_cast<float>(ft_vector_previous_char.x >> 6);
        }

        ekg::io::glyph_t &glyph {draw_font.mapped_glyph[c32]};

        rect.x = pos.x + textbox.widget.scrollbar_property.scroll.position.x + rect_abs.x;
        rect.y = pos.y + rect_abs.y;
        rect.w = glyph.wsize / 2;
        rect.h = textbox.widget.rect_text_size.h;

        if (ekg::rect_collide_vec2(rect, interact)) {
          return true;
        }

        rect.w = glyph.wsize;
        if (!is_empty && ekg::rect_collide_vec2(rect, interact)) {
          index.x++;
          return true;
        }
 
        index.x++;
        pos.x += rect.w;

        rect.w = rect_abs.w;
        if (it+1 == text_len && ekg::rect_collide_vec2(rect, interact)) {
          index.x *= !is_empty; // if line is empty then last utf8 index is 0
          return true;
        }
      }

      index.x = 0;
      pos.x = textbox.color_scheme.gutter_margin;

      index.y++;
      pos.y += textbox.widget.rect_text_size.h;

      rendered.y += textbox.widget.rect_text_size.h;
      if (rendered.y >= rect_abs.h + textbox.widget.rect_text_size.h) {
        return false;
      }
    }

    il = 0;
  }

  return false;
} 

void ekg::ui::handle_cursor_interact(
  ekg::property_t &property,
  ekg::textbox_t &textbox,
  bool picked_index,
  ekg::vec2_t<size_t> &pick_index,
  ekg::input_info_t &input
) {
  if (input.was_released && !input.was_typed) {
    if (
      textbox.widget.current_cursor_index != UINT64_MAX
      &&
      textbox.widget.current_cursor_index < textbox.widget.cursors.size()
    ) {
      ekg::textbox_t::cursor_t cursor {
        textbox.widget.cursors[textbox.widget.current_cursor_index]
      };

      std::vector<ekg::textbox_t::cursor_t> new_cursors {};
      for (size_t i {}; i < textbox.widget.cursors.size(); i++) {
        ekg::textbox_t::cursor_t cur {textbox.widget.cursors[i]};
        if (cursor == cur) {
          new_cursors.push_back(cur);
          continue;
        }

        if (cursor >= cur.a && cursor <= cur.b) {
          continue;
        }

        new_cursors.push_back(cur);
      }

      textbox.widget.cursors = new_cursors;
    }

    textbox.widget.current_cursor_index = UINT64_MAX;
  }

  if (!picked_index) {
    return;
  }

  bool should_create_a_cursor {};
  ekg::vec2_t<size_t> first_pick_pos {};

  if (input.was_pressed && ekg::fired("textbox-action-cursor")) {
    textbox.widget.picked_left = pick_index;
    textbox.widget.picked_right = pick_index;
    first_pick_pos = pick_index;
    textbox.widget.set_cursor_static = true;
    should_create_a_cursor = true;
  }

  if (
    textbox.widget.current_cursor_index != UINT64_MAX &&
    textbox.widget.current_cursor_index >= textbox.widget.cursors.size()
  ) {
    textbox.widget.current_cursor_index = UINT64_MAX;
  }

  if (textbox.widget.current_cursor_index != UINT64_MAX) {
    ekg::textbox_t::cursor_t &cursor {
      textbox.widget.cursors[textbox.widget.current_cursor_index]
    };

    if (ekg::textbox_t::cursor_t {.a = pick_index, .b = pick_index} <= cursor.delta) {
      cursor.direction = ekg::dock::left;
      textbox.widget.picked_left = cursor.delta;
      textbox.widget.picked_right = pick_index;
    } else {
      cursor.direction = ekg::dock::right;
      textbox.widget.picked_left = pick_index;
      textbox.widget.picked_right = cursor.delta;
    }

    cursor.a = textbox.widget.picked_left;
    cursor.b = textbox.widget.picked_right;

    ekg::gui.ui.redraw = true;
    property.widget.should_buffering = true;
  }

  ekg::textbox_t::cursor_t cursor {};
  if (
    should_create_a_cursor
    &&
    !ekg::ui::find_cursor(textbox, pick_index, cursor)
    &&
    ekg::fired("textbox-action-multicursor")
  ) {
    textbox.widget.current_cursor_index = textbox.widget.cursors.size();
    textbox.widget.cursors.push_back(
      {
        .highest_char_index = textbox.widget.picked_left.x,
        .a = textbox.widget.picked_left, 
        .b = textbox.widget.picked_right,
        .delta = first_pick_pos
      }
    );
    should_create_a_cursor = false;
  }

  if (should_create_a_cursor) {
    textbox.widget.current_cursor_index = 0;
    textbox.widget.cursors.clear();
    textbox.widget.cursors.push_back(
      {
        .highest_char_index = textbox.widget.picked_left.x,
        .a = textbox.widget.picked_left, 
        .b = textbox.widget.picked_right,
        .delta = first_pick_pos
      }
    );
  }
}

void ekg::ui::handle_erase(
  ekg::textbox_t &textbox,
  ekg::textbox_t::cursor_t &cursor
) {
  if (cursor.a == cursor.b) {
    return;
  }

  ekg_log_low_level("begin")

  if (cursor.a.y == cursor.b.y) {
    std::string line {textbox.text.at(cursor.a.y)};
    std::string concated {};

    ekg::textbox_t::cursor_t origin {cursor};
    origin.a = origin.delta;
    origin.b = origin.delta;

    cursor.is_ignored = true;
    ekg::ui::refresh_cursors_pos(
      textbox,
      origin,
      {cursor.b.x - cursor.a.x, 0},
      {cursor.b.x - cursor.a.x, 0},
      ekg::ui::textbox_operation::erase_inline
    );
    cursor.is_ignored = false;

    ekg::utf8_concat(
      line,
      {0, cursor.a.x, cursor.b.x, line.size()},
      concated
    );

    textbox.text.set(
      cursor.a.y,
      concated
    );

    cursor.b = cursor.a;
    cursor.delta = cursor.a;
    cursor.highest_char_index = cursor.a.x;

    return;
  }

  ekg::textbox_t::cursor_t origin {cursor};

  cursor.is_ignored = true;
  ekg::ui::refresh_cursors_pos(
    textbox,
    origin,
    {cursor.b.x, cursor.b.y - cursor.a.y},
    {cursor.b.x, cursor.b.y - cursor.a.y},
    ekg::ui::textbox_operation::erase_multiline
  );

  cursor.is_ignored = false;
  textbox.text.set(
    cursor.a.y,
    ekg::utf8_substr(
      textbox.text.at(cursor.a.y),
      0, cursor.a.x
    )
    +
    ekg::utf8_substr(
      textbox.text.at(cursor.b.y),
      cursor.b.x, UINT32_MAX
    )
  );

  textbox.text.erase(
    cursor.a.y + 1,
    cursor.b.y + 1
  );

  textbox.text.sanitize();

  cursor.b = cursor.a;
  cursor.delta = cursor.a;
  cursor.highest_char_index = cursor.a.x;

  ekg_log_low_level("ended")
}

void ekg::ui::handle_insert(
  ekg::textbox_t &textbox,
  ekg::textbox_t::cursor_t &cursor,
  std::string_view typed
) {
  if (cursor.a != cursor.b) {
    ekg::ui::handle_erase(textbox, cursor);
  }

  if (typed == EKG_EOF_SYSTEM) {
    std::string line {textbox.text.at(cursor.a.y)};
    std::string right_cut {ekg::utf8_substr(line, cursor.a.x, UINT32_MAX)};
    size_t right_cut_size {ekg::utf8_length(right_cut)};

    ekg::textbox_t::cursor_t origin {cursor};
    origin.b = origin.a;

    cursor.is_ignored = true;
    ekg::ui::refresh_cursors_pos(
      textbox,
      origin,
      {right_cut_size, 1},
      {right_cut_size, 1},
      ekg::ui::textbox_operation::insert_line
    );
    cursor.is_ignored = false;

    textbox.text.set(cursor.a.y, ekg::utf8_substr(line, 0, cursor.a.x));
    textbox.text.insert(cursor.a.y, right_cut);

    cursor.a.y++;
    cursor.a.x = 0;
    cursor.b = cursor.a;
    cursor.highest_char_index = cursor.a.x;
    return;
  }

  size_t byte_pos {};
  std::string line {textbox.text.at(cursor.a.y)};
  ekg::utf8_find_byte_pos_by_utf_pos(line, cursor.a.x, byte_pos);

  line.insert(
    line.begin() + byte_pos,
    typed.begin(),
    typed.end()
  );

  ekg::textbox_t::cursor_t origin {cursor};
  ekg::vec2_t<size_t> displacement_a {};
  ekg::vec2_t<size_t> displacement_b {};

  ekg::io::chunk_t split_endings {};
  size_t added {textbox.text.set(cursor.a.y, line, split_endings)};

  if (added == 1) {
    displacement_a = {ekg::utf8_length(typed), 0};
    displacement_b = displacement_a;
    cursor.a.x += displacement_a.x;
  } else {
    displacement_a.y = added-1;
    cursor.a.y += displacement_a.y;

    size_t ending {typed.rfind('\n')};
    if (ending != std::string::npos) {
      displacement_a.x = ekg::utf8_length(typed.substr(ending, UINT32_MAX));
    } else {
      displacement_a.x = ekg::utf8_length(typed);
    }

    displacement_b = displacement_a;
    cursor.a.x = displacement_a.x;
  }

  origin.b = cursor.a;

  cursor.is_ignored = true;
  ekg::ui::refresh_cursors_pos(
    textbox,
    origin,
    displacement_a,
    displacement_b,
    (
      added == 1 ?
        ekg::ui::textbox_operation::insert_inline
        :
        ekg::ui::textbox_operation::insert_multiline
    )
  );
  cursor.is_ignored = false;

  cursor.b = cursor.a;
  cursor.highest_char_index = cursor.a.x;
}

void ekg::ui::reload(
  ekg::property_t &property,
  ekg::textbox_t &textbox
) {
  ekg::axis pick_axis {
    ekg::axis::horizontal
  };

  ekg::draw::font &draw_font {
    ekg::draw::get_font_renderer(textbox.font_size)
  };

  textbox.widget.rect_text_size.h = draw_font.text_height + draw_font.offset_text_height;
  textbox.widget.rect_text_size.w = textbox.widget.rect_text_size.h;

  ekg::aligned_t aligned_dimension {};
  ekg::align_rect_dimension(
    pick_axis,
    textbox.widget.rect_text_size,
    ekg::dpi.min_sizes,
    aligned_dimension
  );

  textbox.rect.scaled_height = ekg::max<ekg::pixel_thickness_t>(1, textbox.rect.scaled_height);
  textbox.rect.h = aligned_dimension.h * textbox.rect.scaled_height;

  size_t highest_size {};
  std::list<ekg::io::chunk_t> &chunks {textbox.text.chunks_data()};

  ekg::rect_t<float> &rect_abs {
    ekg::ui::get_abs_rect(property, textbox.rect)
  };

  textbox.widget.scrollbar.rect.x = rect_abs.x;
  textbox.widget.scrollbar.rect.y = rect_abs.y;
  textbox.widget.scrollbar.acceleration = {textbox.widget.rect_text_size.h, textbox.widget.rect_text_size.h};
  textbox.widget.scrollbar.color_scheme = ekg::p_core->handler_theme.get_current_theme().scrollbar_color_scheme;

  ekg::ui::reload(
    textbox.widget.scrollbar_property,
    textbox.widget.scrollbar,
    rect_abs,
    property.children,
    false
  );

  ekg::ui::refresh_scroll_positions(textbox);
}

void ekg::ui::event(
  ekg::property_t &property,
  ekg::textbox_t &textbox,
  const ekg::io::stage &stage
) {
  ekg::property_t &scrollbar_property {
    textbox.widget.scrollbar_property
  };

  scrollbar_property.widget.rect_scissor = property.widget.rect_scissor;

  ekg::ui::event(
    scrollbar_property,
    textbox.widget.scrollbar,
    stage,
    ekg::ui::get_abs_rect(property, textbox.rect)
  );

  property.widget.should_buffering = scrollbar_property.widget.should_buffering;
  property.states.is_absolute = scrollbar_property.states.is_absolute;
  property.scroll = scrollbar_property.scroll;

  switch (stage) {
    default: {
      ekg::input_info_t input {ekg::p_core->handler_input.input};
      ekg::vec2_t<float> interact {static_cast<ekg::vec2_t<float>>(input.interact)};

      /* focus part */

      bool should_enable_high_frequency {
        property.states.is_hovering
        ||
        property.states.is_focused
        ||
        scrollbar_property.widget.is_high_frequency
      };

      bool is_focus_action_fired {
        property.states.is_hovering
        &&
        input.was_pressed
        &&
        ekg::fired("textbox-focus")
      };

      if (
        !property.states.is_focused
        &&
        is_focus_action_fired
      ) {
        should_enable_high_frequency = true;
        property.states.is_focused = true;
      }

      if (
        property.states.is_focused
        &&
        !property.states.is_hovering
        &&
        input.was_pressed
        &&
        !input.was_typed
      ) {
        property.states.is_focused = false;
        should_enable_high_frequency = false;
      }

      if (!property.states.is_focused) {
        return;
      }

      if (property.states.is_hovering) {
        ekg::p_core->p_platform_base->system_cursor = ekg::system_cursor::ibeam;
      }

      if (
        textbox.widget.scrollbar.widget.states_horizontal_bar.is_hovering
        ||
        textbox.widget.scrollbar.widget.states_vertical_bar.is_hovering
      ) {
        ekg::p_core->p_platform_base->system_cursor = ekg::system_cursor::arrow;
      }

      ekg::vec2_t<size_t> pick_index {};
      bool should_pick_index {};
      bool picked_index {};

      if (
        property.states.is_focused
        &&
        property.states.is_hovering
        &&
        input.was_pressed
        &&
        !textbox.widget.scrollbar_property.states.is_hovering
      ) {
        should_pick_index = true;
      }

      /* where input logic and optmized parts are placed */

      if (textbox.widget.current_cursor_index != UINT64_MAX) {
        should_pick_index = true;
      }

      if (should_pick_index) {
        picked_index =
          ekg::ui::find_index_by_interact(
            property,
            textbox,
            interact,
            pick_index
          );
      }

      if (textbox.widget.set_cursor_static) {
        textbox.widget.unset_cursor_static = input.was_released;
      }

      ekg::ui::handle_cursor_interact(
        property,
        textbox,
        picked_index,
        pick_index,
        input
      );

      if (
        should_enable_high_frequency
      ) {
        ekg::io::dispatch(
          ekg::io::operation::high_frequency,
          property.at
        );
      }

      if (!input.was_typed && !input.was_pressed) {
        return;
      }

      /**
       * logic of cursors:
       *  for handling lot of cursors we will
       *  use only one loop for improve performance
       **/

      bool is_action_selected_all_fired {ekg::fired("textbox-action-select-all")};
      if (is_action_selected_all_fired) {
        size_t lines {textbox.text.length_of_lines(true)};
        if (lines > 0) {
          lines -= 1;
          std::string last_line {textbox.text.at(lines)};
          textbox.widget.cursors.clear();
          textbox.widget.cursors.push_back(
            {
              .a = {0, 0},
              .b = {ekg::utf8_length(last_line), lines}
            }
          );
        }

        return;
      }

      bool is_action_copy {ekg::fired("clipboard-copy")};
      bool is_action_paste {ekg::fired("clipboard-paste")};
      bool is_action_cut {ekg::fired("clipboard-cut")};

      bool is_action_erase_right_fired {ekg::fired("textbox-action-erase-right")};
      bool is_action_erase_left_fired {ekg::fired("textbox-action-erase-left")};
      bool is_action_erase_fired {is_action_erase_left_fired || is_action_erase_right_fired};
      bool is_action_selected_keybind_fired {ekg::fired("textbox-action-select")};
      bool is_action_selected_fired {};
      bool is_action_break_line_fired {ekg::fired("textbox-action-break-line")};
      bool block_if_ab_diff_when_erased_is_fired {};

      bool is_modifier_fired {ekg::fired("textbox-action-modifier")};
      bool is_left_fired {ekg::fired("textbox-action-left")};
      bool is_bind_left_fired {is_left_fired};
      bool is_modifier_left_fired {};
      bool is_right_fired {ekg::fired("textbox-action-right")};
      bool is_bind_right_fired {is_right_fired};
      bool is_modifier_right_fired {};
      bool is_up_fired {ekg::fired("textbox-action-up")};
      bool is_modifier_up_fired {is_up_fired && is_modifier_fired};
      bool is_down_fired {ekg::fired("textbox-action-down")};
      bool is_modifier_down_fired {is_down_fired && is_modifier_fired};
      bool is_arrows_fired {is_left_fired || is_right_fired || is_up_fired || is_down_fired};

      if (is_action_paste) {
        input.was_typed = true;
        input.typed = ekg::p_core->p_platform_base->get_clipboard_text();
      }

      std::string clipboard_builder {};
      if (is_action_cut) {
        input.was_typed = false;
      }

      if (
        is_arrows_fired
        ||
        is_action_erase_fired
        ||
        input.was_typed
        ||
        is_action_break_line_fired
        ||
        is_action_copy
        ||
        is_action_cut
        ||
        is_action_paste
      ) {
        textbox.widget.set_cursor_static = true;
      }

      bool is_ab_equals {};
      bool is_bounding {};

      size_t text_total_lines {textbox.text.length_of_lines()};
      size_t byte_pos {};
      size_t nearest_byte_pos {};
      size_t cursor_byte_pos {};
      size_t highest {};

      ekg::rect_t<float> &rect_abs {ekg::ui::get_abs_rect(property, textbox.rect)};
      ekg::vec2_t<float> cursor_pos {};
      std::string line {};

      size_t cursors_size {textbox.widget.cursors.size()};
      size_t cursor_count {};

      bool should_clear_equals_repeated_cursors {};

      for (ekg::textbox_t::cursor_t &cursor : textbox.widget.cursors) {
        if (cursor.is_deleted) continue; 

        cursor_count++;
        is_ab_equals = cursor.a == cursor.b;

        is_action_selected_fired = (is_arrows_fired && is_action_selected_keybind_fired) || (is_action_erase_fired && is_ab_equals);
        is_left_fired = is_bind_left_fired || (is_action_erase_left_fired && is_ab_equals);
        is_right_fired = is_bind_right_fired || (is_action_erase_right_fired && is_ab_equals);

        is_modifier_left_fired = is_left_fired && is_modifier_fired;
        is_modifier_right_fired = is_right_fired && is_modifier_fired;

        /**
         * Before move the cursor, we need make sure we are working with
         * right directions. This is necessary for make possible select.
         **/
        if (
          is_action_selected_fired
          &&
          !(is_action_erase_left_fired || is_action_erase_left_fired)
        ) {
          if (cursor == cursor.delta && is_left_fired) {
            cursor.b = cursor.a;
            cursor.direction = ekg::dock::left;
          } else if (cursor == cursor.delta && is_right_fired) {
            cursor.a = cursor.b;
            cursor.direction = ekg::dock::right;
          } else if (cursor < cursor.delta) {
            cursor.a = cursor.b;
            cursor.direction = ekg::dock::right;
          } else if (cursor > cursor.delta) {
            cursor.b = cursor.a;
            cursor.direction = ekg::dock::left;
          }

          is_ab_equals = true;
        } else {
          cursor.delta = cursor.a;
        }

        if (is_left_fired) {
          if (is_ab_equals) {
            if (cursor.a.x > 0) {
              cursor.a.x -= 1;
            } else if (cursor.a.x == 0 && cursor.a.y > 0) {
              cursor.a.y -= 1;
              cursor.a.x = ekg::utf8_length(textbox.text.at(cursor.a.y));
              is_modifier_left_fired = false;
            }
          }

          (
            is_modifier_left_fired
            &&
            ekg::utf8_find_byte_pos_by_utf_pos(
              (line = textbox.text.at(cursor.a.y)),
              cursor.a.x,
              cursor_byte_pos
            )
            &&
            ekg::utf8_nearest_regex_group_matched_position(
              line.cbegin(),
              line.cbegin() + cursor_byte_pos,
              nearest_byte_pos,
              textbox.regex_operations[ekg::textbox_t::operation::modifier_left],
              ekg::dock::left
            )
            &&
            ekg::utf8_align_utf_pos_by_byte_pos(
              line,
              cursor_byte_pos,
              cursor.a.x,
              nearest_byte_pos
            )
          );

          cursor.highest_char_index = cursor.a.x;
          cursor.b = cursor.a;
        }

        if (is_right_fired) {
          if (is_ab_equals) {
            size_t line_text_length {ekg::utf8_length(textbox.text.at(cursor.a.y))};
            if (cursor.b.x < line_text_length) {
              cursor.b.x += 1;
            } else if (cursor.a.y < text_total_lines-1) {
              cursor.b.y += 1;
              cursor.b.x = 0;
              is_modifier_right_fired = false;
            }
          }

          if (
            is_modifier_right_fired
            &&
            ekg::utf8_find_byte_pos_by_utf_pos(
              (line = textbox.text.at(cursor.b.y)),
              cursor.b.x,
              cursor_byte_pos
            )
            &&
            ekg::utf8_nearest_regex_group_matched_position(
              line.cbegin() + cursor_byte_pos,
              line.cend(),
              nearest_byte_pos,
              textbox.regex_operations[ekg::textbox_t::operation::modifier_right],
              ekg::dock::right
            )
            &&
            ekg::utf8_align_utf_pos_by_byte_pos(
              line,
              cursor_byte_pos,
              cursor.b.x,
              cursor_byte_pos + nearest_byte_pos
            )
          );

          cursor.highest_char_index = cursor.b.x;
          cursor.a = cursor.b;
        }

        if (is_up_fired) {
          if (is_ab_equals) cursor.a.x = cursor.highest_char_index;

          is_bounding = true;
          if (cursor.a.y > 0) {
            cursor.a.y -= 1;
            is_bounding = false;
          }

          if (!is_ab_equals) cursor.highest_char_index = cursor.a.x;

          size_t line_text_length {ekg::utf8_length(textbox.text.at(cursor.a.y))};
          if (cursor.a.x > line_text_length) {
            cursor.a.x = line_text_length;
          }
          
          if (is_bounding) {
            cursor.highest_char_index = cursor.a.x;
            cursor.a.x = 0;
          }

          cursor.b = cursor.a;
        }

        if (is_down_fired) {
          if (is_ab_equals) cursor.b.x = cursor.highest_char_index;

          is_bounding = true;
          if (cursor.b.y < text_total_lines-1) {
            cursor.b.y += 1;
            is_bounding = false;
          }

          if (!is_ab_equals) cursor.highest_char_index = cursor.b.x;

          size_t line_text_length {ekg::utf8_length(textbox.text.at(cursor.b.y))};
          if (cursor.b.x > line_text_length) {
            cursor.b.x = line_text_length;
          }

          if (is_bounding) {
            cursor.highest_char_index = cursor.b.x;
            cursor.b.x = line_text_length;
          }

          cursor.a = cursor.b;
        }

        /**
         * First we move cursor and check the right direction (first structure like this). Then.
         * We need check again the right direction after moved.
         * This will make possible modifier works with select from any direction.
         **/
        if (is_action_selected_fired) {
          if (cursor == cursor.delta && is_left_fired) {
            cursor.b = cursor.a;
            cursor.direction = ekg::dock::left;
          } else if (cursor == cursor.delta && is_right_fired) {
            cursor.a = cursor.b;
            cursor.direction = ekg::dock::right;
          } else if (cursor < cursor.delta) {
            cursor.a = cursor.b;
            cursor.direction = ekg::dock::right;
          } else if (cursor > cursor.delta) {
            cursor.b = cursor.a;
            cursor.direction = ekg::dock::left;
          }

          highest = cursor.highest_char_index;
          if (cursor.direction == ekg::dock::left) {
            cursor.a = cursor.a;
            cursor.b = cursor.delta;
            highest = cursor.a.x;
          } else if (cursor.direction == ekg::dock::right) {
            cursor.a = cursor.delta;
            cursor.b = cursor.b;
            highest = cursor.a.x;
          }

          if (is_left_fired) {
            cursor.highest_char_index = highest;
          } else if (is_right_fired) {
            cursor.highest_char_index = highest;
          }
        }

        if (is_action_cut || is_action_copy) {
          clipboard_builder += textbox.text.read(
            cursor.a,
            cursor.b
          )
          +
          (
            (
              (cursors_size > 1)
              &&
              !is_ab_equals
              &&
              cursor_count != cursors_size
            )
            ? EKG_EOF_SYSTEM : ""
          );
        }

        if (is_action_erase_fired || is_action_cut) {
          ekg::ui::handle_erase(textbox, cursor);
        }

        if (input.was_typed || is_action_break_line_fired) {
          ekg::ui::handle_insert(
            textbox,
            cursor,
            is_action_break_line_fired
              ? EKG_EOF_SYSTEM : input.typed
          );

          if (is_action_break_line_fired || is_action_paste) {
            text_total_lines = textbox.text.length_of_lines(true);    
          }
        }

        if (!is_action_selected_fired) {
          cursor.delta = cursor.a;
        }

        cursor_pos.y = textbox.widget.scrollbar_property.scroll.position.y + (cursor.a.y * textbox.widget.rect_text_size.h);
        cursor_pos.y = static_cast<float>(static_cast<int32_t>(floorf(cursor_pos.y)));

        if (cursor_pos.y + textbox.widget.rect_text_size.h > rect_abs.h) {
          textbox.widget.scrollbar_property.scroll.position.w -=
            ekg::max(cursor_pos.y + textbox.widget.rect_text_size.h - rect_abs.h, textbox.widget.rect_text_size.h);
        } else if (cursor_pos.y <= 0.0f) {
          textbox.widget.scrollbar_property.scroll.position.w +=
            ekg::max(-cursor_pos.y, textbox.widget.rect_text_size.h);
        }

        if (!cursor.is_deleted) {
          cursor.is_ignored = true;
          for (ekg::textbox_t::cursor_t &nearest_cursor : textbox.widget.cursors) {
            if (
              nearest_cursor.is_ignored
              ||
              nearest_cursor.is_deleted
              ||
              cursor != nearest_cursor
            ) continue;
            should_clear_equals_repeated_cursors = true;
            nearest_cursor.is_deleted = true;
          }
          cursor.is_ignored = false;
        }
      }

      if (should_clear_equals_repeated_cursors) {
        for (size_t it {}; it < textbox.widget.cursors.size(); it++) {
          ekg::textbox_t::cursor_t &cursor {textbox.widget.cursors.at(it)};
          if (!cursor.is_deleted) continue;
          textbox.widget.cursors.erase(textbox.widget.cursors.begin() + it);
        }
      }

      if (is_action_cut || is_action_copy) {
        ekg::p_core->p_platform_base->set_clipboard_text(clipboard_builder.c_str());
      }

      if (is_action_erase_fired || input.was_typed || is_action_break_line_fired) {
        ekg::ui::refresh_scroll_positions(textbox);
      }

      ekg::gui.ui.redraw = true;

      ekg_log_low_level(textbox.text.length_of_chunks())

      break;
    }
  case ekg::io::stage::pre:
    ekg::ui::pre_event(property, textbox.rect, false);
    break;
  case ekg::io::stage::post:
    ekg::ui::post_event(property);
    break;
  }
}

void ekg::ui::high_frequency(
  ekg::property_t &property,
  ekg::textbox_t &textbox
) {
  if (textbox.widget.set_cursor_static) {
    ekg::reset(textbox.widget.cursor_timing);
    textbox.widget.set_cursor_static = !textbox.widget.unset_cursor_static;
  }

  textbox.widget.unset_cursor_static = false;
  ekg::reset_if_reach(textbox.widget.cursor_timing, 1000);

  ekg::rect_t<float> &rect_abs {
    ekg::ui::get_abs_rect(property, textbox.rect)
  };

  ekg::ui::high_frequency(
    textbox.widget.scrollbar_property,
    textbox.widget.scrollbar,
    rect_abs
  );

  ekg::gui.ui.redraw = true;
  property.widget.should_buffering = true;
  property.widget.is_high_frequency = (
    property.states.is_focused
    ||
    textbox.widget.scrollbar_property.widget.is_high_frequency
  );
}

void ekg::ui::pass(
  ekg::property_t &property,
  ekg::textbox_t &textbox
) {
  ekg_draw_allocator_bind_local(
    &property.widget.geometry_buffer,
    &property.widget.gpu_data_buffer
  );

  if (property.widget.should_buffering || !textbox.text.audited()) {
    property.widget.should_buffering = true;
    return;
  }

  ekg_draw_allocator_pass();
}

void ekg::ui::buffering(
  ekg::property_t &property,
  ekg::textbox_t &textbox
) {
  ekg::rect_t<float> &rect_abs {
    ekg::ui::get_abs_rect(property, textbox.rect)
  };

  ekg_draw_allocator_assert_scissor(
    property.widget.rect_scissor,
    rect_abs,
    ekg::query<ekg::property_t>(property.parent_at).widget.rect_scissor,
    ekg::always_parented
  );

  textbox.text.unset_audited();

  /* start of background */

  ekg::draw::rect(
    rect_abs,
    textbox.color_scheme.background,
    ekg::draw::mode::fill,
    textbox.layers[ekg::layer::bg]
  );

  /* start of select */

  bool elapsed_mid_second {
    textbox.widget.cursor_timing.current_ticks > 500
  };

  ekg::rect_t<float> rect_select {};
  for (ekg::textbox_t::select_draw_layer_t &layer : textbox.widget.layers_select) {
    if (!layer.is_always_static && layer.is_ab_equals && elapsed_mid_second) {
      continue;
    }

    rect_select = layer.rect + rect_abs + textbox.widget.scrollbar_property.scroll.position;
    rect_select.y = layer.rect.y + rect_abs.y;

    ekg::draw::rect(
      rect_select,
      layer.is_ab_equals ? textbox.color_scheme.text_cursor_foreground : textbox.color_scheme.text_select_foreground,
      ekg::draw::mode::fill,
      ekg::at_t::not_found
    );
  }

  /* start of text */

  ekg::draw::font &draw_font {
    ekg::draw::get_font_renderer(textbox.font_size)
  };

  ekg::gpu::data_t &data {
    ekg::p_core->draw_allocator.bind_current_data()
  };

  ekg::vec2_t<float> pos {
    static_cast<float>(static_cast<int32_t>(rect_abs.x + textbox.color_scheme.gutter_margin)) + textbox.widget.scrollbar_property.scroll.position.x,
    floorf(static_cast<float>(static_cast<int32_t>(rect_abs.y - draw_font.offset_text_height)))
  };

  ekg::io::font_face_t &text_font_face {draw_font.faces[ekg::io::font_face_type::text]};
  ekg::io::font_face_t &emojis_font_face {draw_font.faces[ekg::io::font_face_type::emojis]};
  ekg::io::font_face_t &kanjis_font_face {draw_font.faces[ekg::io::font_face_type::kanjis]};

  data.buffer[0] = pos.x;
  data.buffer[1] = pos.y;
  data.buffer[2] = static_cast<float>(-draw_font.non_swizzlable_range);

  data.buffer[4] = static_cast<float>(textbox.color_scheme.text_foreground.x / 255);
  data.buffer[5] = static_cast<float>(textbox.color_scheme.text_foreground.y / 255);
  data.buffer[6] = static_cast<float>(textbox.color_scheme.text_foreground.z / 255);
  data.buffer[7] = static_cast<float>(textbox.color_scheme.text_foreground.w / 255);

  ekg::hash_t hash {};
  ekg::rect_t<float> vertex {};
  ekg::rect_t<float> uv {};
  ekg::rect_t<float> rendered {};

  size_t text_len {};
  size_t utf8_text_len {};
  uint8_t uc8 {};
  char32_t c32 {};

  FT_Face ft_face {};
  FT_Vector ft_vector_previous_char {};
  char32_t ft_uint_previous {};

  bool is_inline_selected {};
  bool is_complete_line_selected {};
  bool is_ab_equals_selected {};
  bool is_cursor_at_end_of_line {};
  bool cursors_going_on {!textbox.widget.cursors.empty()};
  bool reached_renderable_state {};
  bool get_out {};
  bool is_last_char_from_line {};

  float end_cursor_position {};
  float extra_rect_height {rect_abs.h + textbox.widget.rect_text_size.h};
  float glyph_wsize {};
  float previous_fitting_glyph {};
  float previous_kerning {};

  size_t current_line_for_cursor_complete {UINT64_MAX};
  size_t text_total_chars {textbox.text.length_of_chars()};
  size_t text_total_lines {textbox.text.length_of_lines()};
  size_t il {};
  size_t sum_chunk_sizes {};
  size_t chunk_size {};
  ekg::vec2_t<size_t> index {};

  ekg::pixel_t line_wsize {};
  ekg::textbox_t::cursor_t cursor {};
  std::list<ekg::io::chunk_t> &chunks {textbox.text.chunks_data()};
  size_t chunks_size {chunks.size()};
  bool is_renderable {};

  textbox.widget.scrollbar.rect.h = text_total_lines * textbox.widget.rect_text_size.h;
  textbox.widget.view_line_index = 
    static_cast<size_t>(
      -ekg::arithmetic_normalize<float>(
        textbox.widget.scrollbar_property.scroll.position.y,
        textbox.widget.scrollbar.rect.h
      )
      *
      text_total_lines
    );

  /**
   * This technique prevent from floating-point precision loss inside GPU.
   * 
   * Think that you are scrolling a 1milion text, if you subtract the scroll-position (1 milion)
   * with the text-content, for do scrolling effect, this will send 1000000.0f to the GPU.
   * Some GPUs can not handle high floating numbers, because matrix projection calculate
   * is too heavy to a large float32.
   * 
   * This method send to the GPU: from 0.0 to textbox-height. The scrolling effect happens but
   * without sending 1 milion of f32 to the GPU. The effect is emulated by scrolling only the first
   * line: from 0.0 to negative text-height; so the effect occurs without the needs of stupid
   * scrolling everything.
   * 
   * This technique works because the line height is fixed, ultimately, soon, should be re-worked
   * to support differents text-heights at same time, also, for widgets scrolling (I do not think
   * someone can write a GUI context with +5000000 heights from widgets without pages).
   *
   * - Rina - 11:39; 08/06/2025
   **/
  float visible_text_height {static_cast<float>(textbox.widget.view_line_index * textbox.widget.rect_text_size.h)};
  pos.y = textbox.widget.scrollbar_property.scroll.position.y + visible_text_height;
  pos.y = static_cast<float>(static_cast<int32_t>(floorf(pos.y)));
  pos.x = textbox.color_scheme.gutter_margin;

  std::string empty {"\n"};
  bool is_empty {};
  bool was_empty_before {};

  textbox.widget.layers_select.clear();
  for (std::list<ekg::io::chunk_t>::iterator ic {chunks.begin()}; ic != chunks.end(); ic++) {
    ekg::io::chunk_t &chunk {*ic};
    chunk_size = chunk.size();
    if (sum_chunk_sizes + chunk_size < textbox.widget.view_line_index) {
      pos.x = 0.0f;
      index.y += chunk_size;
      sum_chunk_sizes += chunk_size;
      continue;
    }

    il = 0;
    if (!reached_renderable_state) {
      il = textbox.widget.view_line_index - sum_chunk_sizes;
      index.y += il;
      textbox.widget.view_chunk_line_index = il;
      textbox.widget.view_chunk_it = ic;
      reached_renderable_state = true;
    }

    sum_chunk_sizes += chunk_size;
    for (;il < chunk_size; il++) {
      std::string &chunk_line {chunk.at(il)};
      std::string &line {(is_empty = chunk_line.empty()) ? empty : chunk_line};

      index.x = 0;
      text_len = line.size();
      for (size_t it {}; it < text_len; it++) {
        uc8 = static_cast<uint8_t>(line.at(it));

        ekg::utf8_sequence(
          uc8,
          c32,
          line,
          it
        );

        if (draw_font.ft_bool_kerning && ft_uint_previous) {
          switch (c32 < 256 || !emojis_font_face.was_loaded) {
            case true: {
              ft_face = text_font_face.ft_face;
              break;
            }

            default: {
              ft_face = emojis_font_face.ft_face;
              break;
            }
          }

          FT_Get_Kerning(ft_face, ft_uint_previous, c32, 0, &ft_vector_previous_char);
          pos.x += (previous_kerning = static_cast<float>(ft_vector_previous_char.x >> 6));
        }

        ekg::io::glyph_t &glyph {draw_font.mapped_glyph[c32]};
        glyph.kerning = previous_kerning;

        is_last_char_from_line = it+1 == text_len;
        if (
          cursors_going_on
          &&
          (
            ekg::ui::find_cursor(textbox, index, cursor)
            ||
            (
              is_cursor_at_end_of_line =
                is_last_char_from_line
                &&
                ++index.x && ekg::ui::find_cursor(textbox, index, cursor)
            )
          )
        ) {
          glyph_wsize = glyph.wsize;
          end_cursor_position = glyph_wsize * is_cursor_at_end_of_line;
          is_ab_equals_selected = property.states.is_focused && cursor == index;

          is_inline_selected = cursor >= index && cursor < index && !is_ab_equals_selected;
          is_complete_line_selected = false;

          if (is_inline_selected) {
            if (
              (
                (
                  cursor >= ekg::vec2_t<size_t>(0, index.y)
                )
                &&
                (
                  cursor <= ekg::vec2_t<size_t>(text_len, index.y)
                )
              )
            ) {
              is_inline_selected = false;
            }

            is_complete_line_selected = !is_inline_selected;
          }

          ekg::textbox_t::cursor_t nearest_cursor {};
          ekg::vec2_t<size_t> next_line_index(0, index.y + 1);
          bool is_next_line_selected_in_some_way {};

          if (
            is_last_char_from_line
            &&
            (is_next_line_selected_in_some_way = ekg::ui::find_cursor(textbox, next_line_index, nearest_cursor))
            &&
            !is_complete_line_selected
          ) {
            glyph_wsize = (glyph_wsize * !is_cursor_at_end_of_line) + draw_font.space_wsize;
          }

          ekg::vec2_t<size_t> next_char_index(next_line_index.x + 1, next_line_index.y);
          if (
            !textbox.color_scheme.caret_cursor
            &&
            is_next_line_selected_in_some_way
            &&
            !ekg::ui::find_cursor(textbox, next_char_index, nearest_cursor)
          ) {
            cursor.rect.x = textbox.color_scheme.gutter_margin;
            cursor.rect.y = pos.y + textbox.widget.rect_text_size.h;
            cursor.rect.w = textbox.color_scheme.cursor_thickness;
            cursor.rect.h = textbox.widget.rect_text_size.h;
            textbox.widget.layers_select.push_back({.is_ab_equals = true, .is_always_static = true, .rect = cursor.rect});
          }

          if (is_ab_equals_selected) {
            cursor.rect.x = pos.x + end_cursor_position;
            cursor.rect.y = pos.y;
            cursor.rect.w = textbox.color_scheme.caret_cursor ? glyph_wsize : textbox.color_scheme.cursor_thickness;
            cursor.rect.h = textbox.widget.rect_text_size.h;
            textbox.widget.layers_select.push_back({.is_ab_equals = true, .rect = cursor.rect});
          }

          if (is_inline_selected) {
            cursor.rect.x = pos.x + end_cursor_position;
            cursor.rect.y = pos.y;
            cursor.rect.w = glyph_wsize;
            cursor.rect.h = textbox.widget.rect_text_size.h;
            textbox.widget.layers_select.push_back({.rect = cursor.rect});
          }

          if (
            it == 0
          ) {
            line_wsize = 0;
          }

          if (is_complete_line_selected && !is_empty) {
            line_wsize += glyph_wsize;
          }

          is_cursor_at_end_of_line = false;
        }

        if (is_empty) {
          hash += ekg_generate_hash(pos.x, c32, glyph.x++);
          continue;
        }

        if (!glyph.was_sampled) {
          draw_font.new_glyphs_to_atlas.emplace_back(c32);
          glyph.was_sampled = true;
        }

        vertex.x = pos.x + glyph.left;
        vertex.y = pos.y + draw_font.atlas_rect.h - glyph.top;

        vertex.w = glyph.w;
        vertex.h = glyph.h;

        uv.x = glyph.x;
        uv.w = vertex.w / draw_font.atlas_rect.w;
        uv.h = vertex.h / draw_font.atlas_rect.h;

        ekg::p_core->draw_allocator.push_back_geometry(
          vertex.x,
          vertex.y,
          uv.x,
          uv.y
        );

        ekg::p_core->draw_allocator.push_back_geometry(
          vertex.x,
          vertex.y + vertex.h,
          uv.x,
          uv.y + uv.h
        );

        ekg::p_core->draw_allocator.push_back_geometry(
          vertex.x + vertex.w,
          vertex.y + vertex.h,
          uv.x + uv.w,
          uv.y + uv.h
        );

        ekg::p_core->draw_allocator.push_back_geometry(
          vertex.x + vertex.w,
          vertex.y + vertex.h,
          uv.x + uv.w,
          uv.y + uv.h
        );

        ekg::p_core->draw_allocator.push_back_geometry(
          vertex.x + vertex.w,
          vertex.y,
          uv.x + uv.w,
          uv.y
        );

        ekg::p_core->draw_allocator.push_back_geometry(
          vertex.x,
          vertex.y,
          uv.x,
          uv.y
        );

        pos.x += glyph.wsize;
        ft_uint_previous = c32;
        previous_fitting_glyph = glyph.wsize - (glyph.left + glyph.w);

        /** 
         * Peek `ekg/io/memory.hpp` for better hash definition and purpose.
         **/
        hash += ekg_generate_hash(pos.x, c32, glyph.x);
        index.x++;
      }

      if (
        is_complete_line_selected
      ) {
        cursor.rect.x = textbox.color_scheme.gutter_margin;
        cursor.rect.y = pos.y;
        cursor.rect.h = textbox.widget.rect_text_size.h;
        cursor.rect.w = line_wsize + draw_font.space_wsize;
        textbox.widget.layers_select.push_back({.rect = cursor.rect});
        is_complete_line_selected = false;
        line_wsize = 0;
      }

      pos.x = textbox.color_scheme.gutter_margin;
      pos.y += textbox.widget.rect_text_size.h;
      rendered.y += textbox.widget.rect_text_size.h;
      index.y++;
      hash += pos.y * 32;
      index.x += is_empty;
      previous_fitting_glyph = 0.0f;
    
      if (rendered.y > extra_rect_height) {
        get_out = true;
        break;
      }
    }

    if (get_out) {
      break;
    }
  }

  draw_font.flush();
  data.hash = hash;

  ekg::draw::allocator::is_simple_shape = false;
  ekg::p_core->draw_allocator.bind_texture(draw_font.atlas_texture_sampler_at);
  ekg::p_core->draw_allocator.dispatch();

  /* start of scrollbar */

  textbox.widget.scrollbar.rect.x = rect_abs.x;
  textbox.widget.scrollbar.rect.y = rect_abs.y;

  ekg::ui::buffering(
    textbox.widget.scrollbar_property,
    textbox.widget.scrollbar,
    rect_abs,
    ekg::query<ekg::property_t>(property.parent_at).widget.rect_scissor
  );

  /* start of outline */

  ekg::draw::rect(
    rect_abs,
    textbox.color_scheme.outline,
    ekg::draw::mode::outline,
    ekg::at_t::not_found
  );

  /* start of final tweaks */

  /**
   * Cursors should be rendered backward from text-render.
   * Textbox first try-render render the cursors, then at end of renderer-segment
   * check if latest size is diff, then re-call buffering once for render
   * the cursos (backward).
   * 
   * The EKG renderer engine does not allow doing strip on gpu resources (not yet).
   * May soon strip-resources should be implemented and this code part become deprecated.  
   **/
  size_t layers_select_size {textbox.widget.layers_select.size()};
  if (textbox.widget.last_layers_select_size != layers_select_size) {
    property.widget.should_buffering = true;
    textbox.widget.last_layers_select_size = layers_select_size;
    ekg::gui.ui.redraw = true;
  }

  /* end of pass */

  ekg_draw_allocator_pass();
}

void ekg::ui::unmap(
  ekg::textbox_t &textbox
) {

}
