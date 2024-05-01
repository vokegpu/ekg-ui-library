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

#include "ekg/ui/textbox/ui_textbox_widget.hpp"
#include "ekg/ui/textbox/ui_textbox.hpp"
#include "ekg/ekg.hpp"
#include "ekg/draw/draw.hpp"
#include "ekg/util/geometry.hpp"
#include "ekg/util/text.hpp"

void ekg::ui::textbox_widget::check_nearest_word(ekg::ui::textbox_widget::cursor &cursor, int64_t &x, int64_t &y) {
  if (!this->is_action_modifier_enable) {
    return;
  }

  ekg::ui::textbox_widget::cursor_pos &target_cursor_pos {
    cursor.pos[3] == cursor.pos[0] ? cursor.pos[0] : cursor.pos[1]
  };

  std::string &cursor_text {ekg_textbox_get_cursor_text(target_cursor_pos)};
  uint64_t cursor_text_size {cursor_text.size()};

  if (cursor_text.empty() ||
      (x == 0 || (x < 0 && target_cursor_pos.text_index == 0) ||
      (x > 0 && target_cursor_pos.text_index == cursor_text_size))
     ) {
    return;
  }

  bool is_dir_right {x > 0};
  bool should_update_last_char {};

  uint64_t it {};
  int64_t utf_index {};

  std::string utf_string {};
  char32_t char32 {};
  uint8_t char8 {};

  x = 0;

  /**
   * the cursor index text position is utf encoded,
   * not string coded, so it should start by begin
   * and not by cursor.
   */

  while (it < cursor_text_size) {
    char8 = static_cast<uint8_t>(cursor_text.at(it));

    if (is_dir_right && utf_index >= target_cursor_pos.text_index) {
      if (char8 != 32) {
        should_update_last_char = true;
      } else if (should_update_last_char) {
        x = utf_index - target_cursor_pos.text_index;
        break;
      }
    } else if (!is_dir_right) {
      if (utf_index == target_cursor_pos.text_index) {
        break;
      }

      if (char8 == 32) {
        should_update_last_char = true;
      } else if (should_update_last_char) {
        x = utf_index;
        should_update_last_char = false;
      }
    }

    utf_index++;
    it += ekg::utf_check_sequence(char8, char32, utf_string, cursor_text, it) + 1;
  }

  /**
   * when the loop is finished, sometime it does not check any possible index,
   * so it is important to force at latest one.
   */
  if (is_dir_right && x == 0) {
    x = utf_index - target_cursor_pos.text_index;
    x = x == 0 ? 1 : x;
  } else if (!is_dir_right) {
    x = x - utf_index;
  }
}

/**
 * If the cursor is not selected, then target the first cursor,
 * therefore when the cursor is selected, the code check if moving cursor is
 * enabled, then set the A & B pos based on direction.
 */
void ekg::ui::textbox_widget::move_target_cursor(ekg::ui::textbox_widget::cursor &cursor, int64_t x, int64_t y) {
  ekg::reset(ekg::core->ui_timing);
  ekg::dispatch(ekg::env::redraw);

  ekg::ui::textbox_widget::cursor_pos target_cursor_pos {};
  if ((cursor.pos[0] == cursor.pos[3] && this->is_action_select_enable) ||
      ((x < 0 || y < 0) && cursor.pos[0] != cursor.pos[1] && !this->is_action_select_enable)) {
    target_cursor_pos = cursor.pos[0];
  } else {
    target_cursor_pos = cursor.pos[1];
  }

  if (cursor.pos[0] == cursor.pos[1] || this->is_action_select_enable) {
    this->move_cursor(target_cursor_pos, x, y);
  }

  if (!this->is_action_select_enable) {
    cursor.pos[0] = target_cursor_pos;
    cursor.pos[1] = target_cursor_pos;
    cursor.pos[2] = target_cursor_pos;
    cursor.pos[3] = target_cursor_pos;
    return;
  }

  cursor.pos[3] = target_cursor_pos;

  if (cursor.pos[3] >= cursor.pos[2]) {
    cursor.pos[0] = cursor.pos[2];
    cursor.pos[1] = target_cursor_pos;
  } else {
    cursor.pos[0] = target_cursor_pos;
    cursor.pos[1] = cursor.pos[2];
  }
}

/**
 * This method is not called all the time, the part of batching rects for rendering after,
 * works okay, but I will write a fast select rect batching.
 */
void ekg::ui::textbox_widget::update_ui_text_data() {
  auto *p_ui {(ekg::ui::textbox *) this->p_data};
  auto &f_renderer {ekg::f_renderer(p_ui->get_font_size())};
  auto &rect {this->get_abs_rect()};

  this->rect_text.w = 0.0f;
  this->total_utf_chars = 0;
  std::string formated_text {};

  float x {};
  float y {this->text_offset};
  uint64_t text_chunk_size {this->p_text_chunk_list->size()};

  this->rect_cursor.w = 2.0f;
  this->rect_cursor.h = this->text_height;
  this->is_ui_enabled = p_ui->get_state() == ekg::state::enable;

  for (int64_t chunk_index {}; chunk_index < text_chunk_size; chunk_index++) {
    this->rect_text.w = ekg_min(this->rect_text.w, f_renderer.get_text_width(this->p_text_chunk_list->at(chunk_index)));
  }

  this->embedded_scroll.acceleration.y = (this->text_height * 3.0f) + (this->text_offset * 2.0f);
  this->rect_text.w += this->text_offset * 2.0f;
  p_ui->set_must_format_text(true);
}

void ekg::ui::textbox_widget::move_cursor(ekg::ui::textbox_widget::cursor_pos &cursor, int64_t x, int64_t y) {
  std::string &cursor_text {ekg_textbox_get_cursor_text(cursor)};
  int64_t cursor_text_size {static_cast<int64_t>(ekg::utf_length(cursor_text))};

  bool chunk_bounding_size_index {cursor.chunk_index + 1 == this->p_text_chunk_list->size()};
  bool check_cursor_x {x != 0};

  if (y < 0) {
    if (cursor.chunk_index == 0) {
      cursor.text_index = 0;
    } else {
      cursor.chunk_index--;
      cursor_text_size = static_cast<int64_t>(ekg::utf_length(ekg_textbox_get_cursor_text(cursor)));

      cursor.text_index = cursor.last_text_index;
      cursor.text_index = ekg_max(cursor.text_index, static_cast<int64_t>(cursor_text_size));
    }
  } else if (y > 0) {
    if (chunk_bounding_size_index) {
      cursor.text_index = cursor_text_size;
    } else {
      cursor.chunk_index++;

      cursor_text_size = static_cast<int64_t>(ekg::utf_length(ekg_textbox_get_cursor_text(cursor)));
      cursor.text_index = cursor.last_text_index;
      cursor.text_index = ekg_max(cursor.text_index, static_cast<int64_t>(cursor_text_size));
    }
  }

  cursor.text_index += x;

  if (cursor.text_index < 0 && cursor.chunk_index > 0 && check_cursor_x) {
    cursor.chunk_index--;
    y = -1;
    cursor.text_index = static_cast<int64_t>(ekg::utf_length(ekg_textbox_get_cursor_text(cursor)));
    cursor_text_size = cursor.text_index;
  }

  if (cursor.text_index > cursor_text_size && chunk_bounding_size_index && check_cursor_x) {
    cursor.text_index = cursor_text_size;
    y = -1;
  } else if (cursor.text_index > cursor_text_size && check_cursor_x) {
    cursor.chunk_index++;
    cursor.text_index = 0;
    y = 1;
  }

  cursor.text_index = ekg_min(cursor.text_index, static_cast<int64_t>(0));
  cursor.chunk_index = ekg_max(cursor.chunk_index, static_cast<int64_t>(this->p_text_chunk_list->size()));

  if (check_cursor_x) {
    cursor.last_text_index = cursor.text_index;
  }

  std::string &current_cursor_text {ekg_textbox_get_cursor_text(cursor)};
  auto p_ui {(ekg::ui::textbox *) this->p_data};
  auto &f_renderer {ekg::f_renderer(p_ui->get_font_size())};
  auto &rect {this->get_abs_rect()};

  const ekg::vec2 cursor_pos {
      rect.x + this->embedded_scroll.scroll.x +
      (f_renderer.get_text_width(ekg::utf_substr(current_cursor_text, 0, cursor.text_index))),
      rect.y + this->embedded_scroll.scroll.y + this->text_offset +
      (static_cast<float>(cursor.chunk_index) * this->text_height)
  };

  const ekg::vec4 cursor_outspace_screen {
      rect.x - cursor_pos.x,
      rect.y - cursor_pos.y,
      (cursor_pos.x + this->cursor_char_wsize[1] + this->cursor_char_wsize[2]) -
      (rect.x + rect.w - this->rect_cursor.w),
      (cursor_pos.y + this->rect_cursor.h) - (rect.y + rect.h - this->text_offset)
  };

  if (cursor_outspace_screen.x > 0.0f &&
      (x < 0 || cursor_outspace_screen.x > this->cursor_char_wsize[0] + this->rect_cursor.w)) {
    this->embedded_scroll.scroll.z += cursor_outspace_screen.x;
  } else if (cursor_outspace_screen.z > 0.0f && (x > 0 || cursor_outspace_screen.z >
                                                          this->cursor_char_wsize[1] + this->cursor_char_wsize[2] +
                                                          this->rect_cursor.w)) {
    this->embedded_scroll.scroll.z -= cursor_outspace_screen.z;
  }

  /**
   * Instead of targeting the scroll using the render cursor pos, this use the current cursor chunk it.
   */

  if (cursor_outspace_screen.y > 0.0f &&
      (y < 0 || cursor_outspace_screen.y > this->rect_cursor.h + this->text_offset)) {
    this->embedded_scroll.scroll.w += cursor_outspace_screen.y;
  } else if (cursor_outspace_screen.w > 0.0f &&
             (y > 0 || cursor_outspace_screen.w > this->rect_cursor.h + this->rect_cursor.h + this->text_offset)) {
    this->embedded_scroll.scroll.w -= cursor_outspace_screen.w;
  }

  ekg::reset(ekg::core->ui_timing);
  ekg::dispatch(ekg::env::redraw);
}

void ekg::ui::textbox_widget::process_text(
  ekg::ui::textbox_widget::cursor &cursor,
  std::string_view text,
  ekg::ui::textbox_widget::action action,
  int64_t direction
) {
  auto p_ui {(ekg::ui::textbox *) this->p_data};
  if (!(this->is_ui_enabled = p_ui->get_state() == ekg::state::enable) && !(text == "clipboard" && this->is_clipboard_copy)) {
    return;
  }

  this->text_edited = true;

  std::string &cursor_text_a {ekg_textbox_get_cursor_text(cursor.pos[0])};
  std::string &cursor_text_b {ekg_textbox_get_cursor_text(cursor.pos[1])};

  uint64_t ui_max_chars_per_line {p_ui->get_max_chars_per_line()};
  uint64_t ui_max_lines {p_ui->get_max_lines()};
  uint64_t previous_text_chunk_size {this->p_text_chunk_list->size()};

  bool max_size_reached[2] {
      cursor_text_a.size() == ui_max_chars_per_line,
      cursor_text_b.size() == ui_max_chars_per_line
  };

  switch (action) {
    case ekg::ui::textbox_widget::action::add_text:
      if ((this->is_action_modifier_enable &&
           !(this->is_clipboard_cut || this->is_clipboard_copy || this->is_clipboard_paste))) {
        break;
      }

      /**
       * Always the tab is pressed, the process should add the spaces instead of \t,
       * it is cached to prevent useless iteration.
       */
      if (text == "\t") {
        uint8_t ui_tab_size {p_ui->get_tab_size()};

        if (this->cached_tab_size.size() != ui_tab_size) {
          this->cached_tab_size.clear();
          for (uint8_t it {}; it < ui_tab_size; it++) {
            this->cached_tab_size += ' ';
          }
        }

        text = this->cached_tab_size;
        direction = static_cast<int64_t>(ui_tab_size);
      } else if (text == "clipboard") {
        text = "";

        if (this->is_clipboard_cut || this->is_clipboard_copy) {
          if (cursor.pos[0] != cursor.pos[1]) {
            std::string copy_text {};
            if (cursor.pos[0].chunk_index == cursor.pos[1].chunk_index) {
              /**
               * For some reason the STL substr implementation actually
               * do the following addition for B (second parameter),
               * (B += A), the subtract result between cursor 1 and 0
               * should temp fix this.
               */
              copy_text += ekg::utf_substr(
                cursor_text_a, cursor.pos[0].text_index,
                cursor.pos[1].text_index - cursor.pos[0].text_index
              );
            } else {
              copy_text += ekg::utf_substr(cursor_text_a, cursor.pos[0].text_index, ekg::utf_length(cursor_text_a));
              copy_text += '\n';

              uint64_t it {static_cast<uint64_t>(cursor.pos[0].chunk_index + 1)};
              uint64_t size {this->p_text_chunk_list->size()};

              bool not_last_line {};
              bool last_line {};

              while (it < cursor.pos[1].chunk_index && it < size) {
                copy_text += this->p_text_chunk_list->at(it);

                last_line = it + 1 == size;
                not_last_line = !last_line;

                if (not_last_line || (this->p_text_chunk_list->at(it).empty() && last_line)) {
                  copy_text += '\n';
                }

                it++;
              }

              copy_text += ekg::utf_substr(cursor_text_b, 0, cursor.pos[1].text_index);
            }

            ekg::core->p_os_platform->set_clipboard_text(copy_text.c_str());
          }

          if (this->is_clipboard_copy) {
            break;
          }
        }
      }

      if (max_size_reached[0]) {
        break;
      }

      cursor_text_a = (
        ekg::utf_substr(cursor_text_a, 0, cursor.pos[0].text_index) +
        text.data() +
        ekg::utf_substr(cursor_text_b, cursor.pos[1].text_index, ekg::utf_length(cursor_text_b))
      );

      ekg_textbox_clamp_line(cursor_text_a, ui_max_chars_per_line);

      if (cursor.pos[0].chunk_index != cursor.pos[1].chunk_index) {
        this->p_text_chunk_list->erase(
          this->p_text_chunk_list->begin() + cursor.pos[0].chunk_index + 1,
          this->p_text_chunk_list->begin() + cursor.pos[1].chunk_index + 1
        );
      }

      if (this->is_clipboard_paste && ekg::core->p_os_platform->has_clipboard_text() && !(text = ekg::core->p_os_platform->get_clipboard_text()).empty()) {
        direction = static_cast<int64_t>(ekg::utf_length(text));

        std::vector<std::string> utf_clipboard_decoded {};
        ekg::utf_decode(text, utf_clipboard_decoded);

        uint64_t sum_decoded_size {this->p_text_chunk_list->size() + utf_clipboard_decoded.size() - 1};
        if (sum_decoded_size > ui_max_lines) {
          uint64_t subtract_decoded_size {sum_decoded_size - ui_max_lines};
          utf_clipboard_decoded.erase(
            utf_clipboard_decoded.begin() + static_cast<int64_t>(subtract_decoded_size),
            utf_clipboard_decoded.end()
          );
        }

        if (utf_clipboard_decoded.size() == 1) {
          cursor_text_a = (
            ekg::utf_substr(cursor_text_a, 0, cursor.pos[0].text_index) +
            utf_clipboard_decoded.at(0) +
            ekg::utf_substr(cursor_text_a, cursor.pos[0].text_index, ekg::utf_length(cursor_text_a))
          );
        } else if (utf_clipboard_decoded.size() > 1) {
          int64_t last_clipboard_list_index {static_cast<int64_t>(utf_clipboard_decoded.size() - 1)};
          std::string &last_clipboard_line {utf_clipboard_decoded.at(last_clipboard_list_index)};

          cursor.pos[1].text_index = static_cast<int64_t>(ekg::utf_length(last_clipboard_line));
          cursor.pos[1].chunk_index = cursor.pos[0].chunk_index + last_clipboard_list_index;
          cursor.pos[1].select_index = cursor.pos[1].text_index;

          std::string stored_text = ekg::utf_substr(
            cursor_text_a, cursor.pos[0].text_index,
            ekg::utf_length(cursor_text_a)
          );

          cursor_text_a = (
            ekg::utf_substr(cursor_text_a, 0, cursor.pos[0].text_index) + utf_clipboard_decoded.at(0)
          );

          ekg_textbox_clamp_line(cursor_text_a, ui_max_chars_per_line);

          last_clipboard_line = last_clipboard_line + stored_text;
          this->p_text_chunk_list->insert(
            this->p_text_chunk_list->begin() + cursor.pos[0].chunk_index + 1,
            utf_clipboard_decoded.begin() + 1,
            utf_clipboard_decoded.end()
          );

          cursor.pos[0] = cursor.pos[1];
          direction = 0;
        }
      }

      ekg_textbox_clamp_text_chunk_size(this->p_text_chunk_list, ui_max_lines);
      this->move_cursor(cursor.pos[0], direction, 0);

      break;

    case ekg::ui::textbox_widget::action::erase_text:
      if (this->is_action_modifier_enable && cursor.pos[0] == cursor.pos[1] &&
          (cursor.pos[0].text_index > 0 || direction > 0)) {
        int64_t cursor_dir[2] {direction, 0};

        if (this->is_action_select_enable) {
          cursor_dir[0] = direction < 0 ? (-cursor.pos[0].text_index) :
                          (static_cast<int64_t>(ekg::utf_length(cursor_text_a)) - cursor.pos[0].text_index);
        } else {
          this->check_nearest_word(cursor, cursor_dir[0], cursor_dir[1]);
        }

        this->move_cursor(direction < 0 ? cursor.pos[0] : cursor.pos[1], cursor_dir[0], cursor_dir[1]);
      }

      if (cursor.pos[0] == cursor.pos[1] && direction < 0 &&
          (cursor.pos[0].text_index > 0 || cursor.pos[0].chunk_index > 0)
         ) {
        if (cursor.pos[0].text_index - 1 < 0 && cursor.pos[0].chunk_index > 0) {
          std::string stored_text {cursor_text_a};

          this->move_cursor(cursor.pos[0], -1, 0);
          this->p_text_chunk_list->erase(this->p_text_chunk_list->begin() + cursor.pos[0].chunk_index + 1);

          std::string &upper_line_text {ekg_textbox_get_cursor_text(cursor.pos[0])};
          upper_line_text += stored_text;

          ekg_textbox_clamp_line(upper_line_text, ui_max_chars_per_line);
        } else {
          int64_t it {ekg_min(cursor.pos[0].text_index - 1, static_cast<int64_t>(0))};
          cursor_text_a = (
            ekg::utf_substr(cursor_text_a, 0, it) +
            ekg::utf_substr(cursor_text_a, it + 1, ekg::utf_length(cursor_text_a))
          );

          ekg_textbox_clamp_line(cursor_text_a, ui_max_chars_per_line);
          this->move_cursor(cursor.pos[0], -1, 0);
        }
      } else if (cursor.pos[0] != cursor.pos[1] && direction != 0) {
        cursor_text_a = ekg::utf_substr(cursor_text_a, 0, cursor.pos[0].text_index) +
                        ekg::utf_substr(cursor_text_b, cursor.pos[1].text_index, ekg::utf_length(cursor_text_b));

        if (cursor.pos[0].chunk_index != cursor.pos[1].chunk_index) {
          this->p_text_chunk_list->erase(
            this->p_text_chunk_list->begin() + cursor.pos[0].chunk_index + 1,
            this->p_text_chunk_list->begin() + cursor.pos[1].chunk_index + 1
          );
        }

        ekg_textbox_clamp_line(cursor_text_a, ui_max_chars_per_line);
      } else if (cursor.pos[0] == cursor.pos[1] && direction > 0) {
        int64_t cursor_text_size {static_cast<int64_t>(ekg::utf_length(cursor_text_a))};
        bool chunk_bounding_size_index {cursor.pos[0].chunk_index + 1 == this->p_text_chunk_list->size()};

        if (cursor.pos[0].text_index >= cursor_text_size && !chunk_bounding_size_index) {
          cursor_text_a += this->p_text_chunk_list->at(cursor.pos[0].chunk_index + 1);
          this->p_text_chunk_list->erase(this->p_text_chunk_list->begin() + cursor.pos[0].chunk_index + 1);
        } else if (cursor.pos[0].text_index < cursor_text_size) {
          int64_t it {cursor.pos[0].text_index};
          cursor_text_a = ekg::utf_substr(cursor_text_a, 0, it) +
                          ekg::utf_substr(cursor_text_a, it + 1, ekg::utf_length(cursor_text_a));
        }

        ekg_textbox_clamp_line(cursor_text_a, ui_max_chars_per_line);
      }

      break;

    case ekg::ui::textbox_widget::action::break_line:
      if (this->p_text_chunk_list->size() >= ui_max_lines) {
        break;
      }

      int64_t cursor_dir[2] {0, 1};
      std::string line {};

      if (!this->is_action_modifier_enable) {
        line = ekg::utf_substr(cursor_text_a, cursor.pos[0].text_index, ekg::utf_length(cursor_text_a));
        cursor_text_a = ekg::utf_substr(cursor_text_a, 0, cursor.pos[0].text_index);

        cursor_dir[0] = 1;
        cursor_dir[1] = 0;
      }

      this->p_text_chunk_list->insert(this->p_text_chunk_list->begin() + cursor.pos[0].chunk_index + 1, line);

      ekg_textbox_clamp_text_chunk_size(this->p_text_chunk_list, ui_max_lines);
      this->move_cursor(cursor.pos[0], cursor_dir[0], cursor_dir[1]);

      ekg::reset(ekg::core->ui_timing);
      ekg::dispatch(ekg::env::redraw);

      break;
  }

  if (!this->is_clipboard_copy) {
    cursor.pos[2] = cursor.pos[1] = cursor.pos[0];
    cursor.pos[3] = cursor.pos[0];
  }

  if (previous_text_chunk_size != this->p_text_chunk_list->size()) {
    this->rect_text.h = (this->text_height * static_cast<float>(this->p_text_chunk_list->size()));
    this->embedded_scroll.rect_child.h = this->rect_text.h;
    this->embedded_scroll.clamp_scroll();
    this->update_ui_text = true;
  }

  ekg::reset(ekg::core->ui_timing);
  ekg::dispatch(ekg::env::redraw);
}

void ekg::ui::textbox_widget::check_cursor_text_bounding(
  ekg::ui::textbox_widget::cursor &cursor,
  bool reset_second_cursor_pos
) {
  if (!this->flag.focused) {
    return;
  }

  auto p_ui {(ekg::ui::textbox *) this->p_data};
  auto &rect {this->get_abs_rect()};
  auto &f_renderer {ekg::f_renderer(p_ui->get_font_size())};

  float x {};
  float y {rect.y + this->embedded_scroll.scroll.y + this->text_offset};

  ekg::rect char_rect {};
  ekg::vec4 &interact {ekg::input::interact()};

  interact.x = ekg_clamp(interact.x, rect.x + this->rect_cursor.w, rect.x + rect.w - this->rect_cursor.w);
  interact.y = ekg_clamp(interact.y, rect.y + this->text_offset * 2.0f, rect.y + rect.h - this->text_offset * 2.0f);

  uint64_t text_chunk_size {this->p_text_chunk_list->size()};
  bool checked {};

  char32_t char32 {};
  uint8_t char8 {};

  uint64_t utf_char_index {};
  std::string utf_string {};
  bool is_index_chunk_at_end {};

  uint64_t text_index {};
  uint64_t chunk_index {};
  uint64_t it {};

  y += (this->text_height * static_cast<float>(this->visible_text[1]));
  for (chunk_index = this->visible_text[1]; chunk_index < text_chunk_size; chunk_index++) {
    std::string &text {this->p_text_chunk_list->at(chunk_index)};

    x = rect.x + this->rect_cursor.w + this->embedded_scroll.scroll.x;
    utf_char_index = 0;
    f_renderer.ft_uint_previous = 0;
    is_index_chunk_at_end = chunk_index == text_chunk_size - 1;

    if (y > rect.y + rect.h) {
      break;
    }

    for (it = 0; it < text.size(); it++) {
      char8 = static_cast<uint8_t>(text.at(it));
      it += ekg::utf_check_sequence(char8, char32, utf_string, text, it);

      if (f_renderer.ft_bool_kerning && f_renderer.ft_uint_previous) {
        FT_Get_Kerning(f_renderer.ft_face, f_renderer.ft_uint_previous, char32, 0, &f_renderer.ft_vector_previous_char);
        x += static_cast<float>(f_renderer.ft_vector_previous_char.x >> 6);
      }

      char_rect.x = x;
      char_rect.y = y;
      char_rect.w = (f_renderer.allocated_char_data[char32].wsize) / 2;
      char_rect.h = this->text_height + 1.0f;

      if (ekg::rect_collide_vec_precisely(char_rect, interact)) {
        text_index = utf_char_index;
        checked = true;
        break;
      }

      char_rect.w += char_rect.w;
      if (ekg::rect_collide_vec_precisely(char_rect, interact)) {
        text_index = utf_char_index + 1;
        checked = true;
        break;
      }

      utf_char_index++;
      f_renderer.ft_uint_previous = char32;
      x += char_rect.w;
    }

    /**
     * If interact position is not colliding with any char in this line, then set index to the end or begin of line.
     */
    char_rect.x = rect.x;
    char_rect.y = y;
    char_rect.w = rect.w;
    char_rect.h = this->text_height + 1.0f + (rect.h * static_cast<float>(is_index_chunk_at_end));

    if (ekg::rect_collide_vec(char_rect, interact) && (!checked || text.empty())) {
      char_rect.w = this->text_offset;
      if (ekg::rect_collide_vec(char_rect, interact)) {
        text_index = 0;
      } else {
        text_index = utf_char_index;
      }

      checked = true;
      break;
    }

    if (checked) break;
    y += this->text_height;
  }

  if (reset_second_cursor_pos) {
    if (checked) {
      cursor.pos[0].chunk_index = static_cast<int64_t>(chunk_index);
      cursor.pos[0].text_index = static_cast<int64_t>(text_index);
      cursor.pos[0].last_text_index = static_cast<int64_t>(text_index);
    } else {
      cursor.pos[0].chunk_index = static_cast<int64_t>(chunk_index - (!this->p_text_chunk_list->empty()));
      cursor.pos[0].text_index = static_cast<int64_t>(utf_char_index);
      cursor.pos[0].last_text_index = static_cast<int64_t>(text_index);
    }

    cursor.pos[1] = cursor.pos[0];
    cursor.pos[2] = cursor.pos[0];
    cursor.pos[3] = cursor.pos[0];
  } else {
    if (checked) {
      it = (text_index < cursor.pos[2].text_index && chunk_index == cursor.pos[2].chunk_index) ||
           (chunk_index < cursor.pos[2].chunk_index) ? 0 : 1;

      cursor.pos[it].chunk_index = static_cast<int64_t>(chunk_index);
      cursor.pos[it].text_index = static_cast<int64_t>(text_index);
      cursor.pos[it].last_text_index = static_cast<int64_t>(text_index);
    } else {
      it = text_index < cursor.pos[2].text_index && chunk_index <= cursor.pos[2].chunk_index ? 0 : 1;

      cursor.pos[it].chunk_index = static_cast<int64_t>(chunk_index - (!this->p_text_chunk_list->empty()));
      cursor.pos[it].text_index = static_cast<int64_t>(utf_char_index);
      cursor.pos[it].last_text_index = static_cast<int64_t>(text_index);
    }

    if (it == 1) {
      cursor.pos[0] = cursor.pos[2];
      cursor.pos[3] = cursor.pos[1];
    } else {
      cursor.pos[1] = cursor.pos[2];
      cursor.pos[3] = cursor.pos[0];
    }
  }

  ekg::dispatch(ekg::env::redraw);
}

void ekg::ui::textbox_widget::on_create() {
  this->p_text_chunk_list = static_cast<ekg::ui::textbox*>(this->p_data);
}

void ekg::ui::textbox_widget::on_reload() {
  auto p_ui {(ekg::ui::textbox *) this->p_data};
  auto &rect {this->get_abs_rect()};
  auto &f_renderer {ekg::f_renderer(p_ui->get_font_size())};
  auto scaled_height {p_ui->get_scaled_height()};

  float text_width {f_renderer.get_text_width(p_ui->get_tag())};
  float dimension_offset {this->text_height / 2};

  this->text_height = f_renderer.get_text_height();
  this->text_offset = ekg::find_min_offset(text_width, dimension_offset);

  this->dimension.w = ekg_min(this->dimension.w, text_width);
  this->dimension.h = (this->text_height + dimension_offset) * static_cast<float>(scaled_height);

  this->min_size.x = ekg_min(this->min_size.x, this->text_height);
  this->min_size.y = ekg_min(this->min_size.y, this->dimension.h);

  if (this->last_text_chunk_size != this->p_text_chunk_list->size()) {
    //this->widget_side_text = p_ui->get_text();

    /**
     * Generate default empty text for say to the rest of textbox,
     * that is empty and not with null lines.
     */
    if (this->p_text_chunk_list->empty()) {
      this->p_text_chunk_list->emplace_back();
    }

    this->text_edited = true;
    this->update_ui_text = true;
    this->rect_text.h = (this->text_height * static_cast<float>(this->p_text_chunk_list->size()));

    float vertical_scroll_limit {this->rect_text.h - rect.h};
    float new_text_height_diff {
      this->text_height *
      (
        /***
         * before casting to float, must cast to int64_t,
         * uint64_t range is large than int64_t
         * but uint64_t can not subtract less than 0,
         * otherwise it returns overflow.
         */
        static_cast<float>(
          static_cast<int64_t>(
            this->p_text_chunk_list->size()
          ) - this->last_text_chunk_size
        ) +
        1.0f
      )
    };

    /**
     * If the difference between the new and old texts,
     * is nearest of scrolling y, it should follow the scrolling.
     */
    if (vertical_scroll_limit > 0 && this->embedded_scroll.scroll.w < -(vertical_scroll_limit - new_text_height_diff)) {
      this->embedded_scroll.scroll.w = -vertical_scroll_limit;
    }

    if (!this->is_high_frequency) {
      ekg::update_high_frequency(this);
    }

    this->last_text_chunk_size = this->p_text_chunk_list->size();
  }

  this->embedded_scroll.p_rect_mother = &rect;
  this->embedded_scroll.rect_child = this->rect_text;
  this->embedded_scroll.widget_id = this->p_data->get_id();
  this->embedded_scroll.on_reload();
}

void ekg::ui::textbox_widget::on_pre_event(ekg::os::io_event_serial &io_event_serial) {
  abstract_widget::on_pre_event(io_event_serial);

  this->embedded_scroll.on_pre_event(io_event_serial);
  this->flag.absolute = (
    this->embedded_scroll.is_dragging_bar() ||
    this->embedded_scroll.flag.activity ||
    this->flag.state
  );
}

void ekg::ui::textbox_widget::on_event(ekg::os::io_event_serial &io_event_serial) {
  bool pressed {ekg::input::pressed() && ekg::input::action("textbox-activity")};
  bool released {ekg::input::released()};
  bool motion {ekg::input::motion()};

  if (this->flag.hovered) {
    ekg::cursor = ekg::system_cursor::ibeam;
  }

  this->is_action_select_enable = ekg::input::action("textbox-action-select");
  if (this->flag.hovered && pressed) {
    ekg::set(this->flag.focused, this->flag.hovered);
    ekg::reset(ekg::core->ui_timing);

    auto &main_cursor {this->loaded_multi_cursor_list.at(0)};
    ekg::ui::textbox_widget::cursor clicked_pos {};
    this->check_cursor_text_bounding(clicked_pos, true);

    auto &interact {ekg::input::interact()};
    this->cursor_delta.x = interact.x;
    this->cursor_delta.y = interact.y;

    bool movement_input_enabled {this->flag.focused && this->is_action_select_enable};
    if (movement_input_enabled && !this->flag.state) {
      if (clicked_pos.pos[0] >= main_cursor.pos[2]) {
        main_cursor.pos[2] = main_cursor.pos[0];
        main_cursor.pos[1] = clicked_pos.pos[0];
        main_cursor.pos[3] = clicked_pos.pos[0];
      } else {
        main_cursor.pos[2] = main_cursor.pos[1];
        main_cursor.pos[0] = clicked_pos.pos[0];
        main_cursor.pos[3] = clicked_pos.pos[1];
      }
    } else {
      main_cursor.pos[0] = clicked_pos.pos[0];
      main_cursor.pos[1] = clicked_pos.pos[1];
      main_cursor.pos[2] = clicked_pos.pos[2];
      main_cursor.pos[3] = clicked_pos.pos[0];
    }

    this->flag.state = this->flag.hovered;
  } else if (this->flag.state && motion && !this->embedded_scroll.is_dragging_bar()) {
    auto &interact {ekg::input::interact()};
    ekg::vec2 diff {
      interact.x - this->cursor_delta.x,
      interact.y - this->cursor_delta.y
    };

    this->check_cursor_text_bounding(
      this->loaded_multi_cursor_list.at(0),
      !(static_cast<int32_t>(diff.x * diff.x + diff.y * diff.y) > 5)
    );
  }

  if (released) {
    this->flag.state = false;
  }

  // @TODO dragging scroll horizontal not working
  this->embedded_scroll.on_event(io_event_serial);
  this->flag.highlight = this->flag.hovered;

  if ((this->flag.focused || this->flag.hovered || this->flag.absolute) && !this->is_high_frequency) {
    ekg::update_high_frequency(this);
  }

  if (!this->flag.hovered && pressed && !ekg::input::typed()) {
    ekg::ui::textbox_widget::cursor main_cursor {this->loaded_multi_cursor_list.at(0)};
    main_cursor.pos[1] = main_cursor.pos[0];

    this->loaded_multi_cursor_list.clear();
    this->loaded_multi_cursor_list.push_back(main_cursor);

    ekg::set(this->flag.focused, false);
  }

  // firing clipboard input by ekg::input::clipboard does not accept if mouse is out of textbox,
  // in the case of popup with clipboard operations, it will not process the action.

  this->is_clipboard_cut = ekg::input::action("clipboard-cut");
  this->is_clipboard_paste = ekg::input::action("clipboard-paste");
  this->is_clipboard_copy = ekg::input::action("clipboard-copy");

  bool should_process_textbox {
    this->flag.focused && (
      (io_event_serial.event_type == ekg::platform_event_type::key_down || io_event_serial.event_type == ekg::platform_event_type::text_input) ||
      (this->is_clipboard_copy || this->is_clipboard_cut || this->is_clipboard_paste)
    )
  };

  if (!should_process_textbox) {
    return;
  }

  if (io_event_serial.event_type == ekg::platform_event_type::text_input) {
    for (ekg::ui::textbox_widget::cursor &cursor: this->loaded_multi_cursor_list) {
      this->process_text(
        cursor,
        io_event_serial.text_input,
        ekg::ui::textbox_widget::action::add_text,
        1
      );
    }
  } else if (io_event_serial.event_type == ekg::platform_event_type::key_down && ekg::input::receive("escape")) {
    ekg::ui::textbox_widget::cursor main_cursor {this->loaded_multi_cursor_list.at(0)};
    main_cursor.pos[1] = main_cursor.pos[0];

    this->loaded_multi_cursor_list.clear();
    this->loaded_multi_cursor_list.push_back(main_cursor);

    ekg::set(this->flag.focused, false);
  } else if (ekg::input::action("textbox-action-select-all")) {
    ekg::ui::textbox_widget::cursor main_cursor {this->loaded_multi_cursor_list.at(0)};
    main_cursor.pos[0].chunk_index = 0;
    main_cursor.pos[0].text_index = 0;
    main_cursor.pos[0].last_text_index = 0;

    main_cursor.pos[1].chunk_index = ekg_min(static_cast<int64_t>(this->p_text_chunk_list->size()) - 1, static_cast<int64_t>(0));
    main_cursor.pos[1].text_index = static_cast<int64_t>(ekg::utf_length(ekg_textbox_get_cursor_text(main_cursor.pos[1])));
    main_cursor.pos[1].last_text_index = main_cursor.pos[1].chunk_index;

    this->loaded_multi_cursor_list.clear();
    this->loaded_multi_cursor_list.push_back(main_cursor);
  } else {
    int64_t cursor_dir[2] {};
    this->is_action_modifier_enable = ekg::input::action("textbox-action-modifier");

    for (ekg::ui::textbox_widget::cursor &cursor: this->loaded_multi_cursor_list) {
      cursor_dir[0] = cursor_dir[1] = 0;

      if (ekg::input::action("textbox-action-up")) {
        cursor_dir[1] = -1;
      } else if (ekg::input::action("textbox-action-down")) {
        cursor_dir[1] = 1;
      } else if (ekg::input::action("textbox-action-left")) {
        cursor_dir[0] = -1;
      } else if (ekg::input::action("textbox-action-right")) {
        cursor_dir[0] = 1;
      } else if (ekg::input::action("textbox-action-delete-left")) {
        this->process_text(cursor, "backspace", ekg::ui::textbox_widget::action::erase_text, -1);
      } else if (ekg::input::action("textbox-action-delete-right")) {
        this->process_text(cursor, "delete", ekg::ui::textbox_widget::action::erase_text, 1);
      } else if (ekg::input::action("textbox-action-break-line")) {
        this->process_text(cursor, "return", ekg::ui::textbox_widget::action::break_line, 1);
      } else if (ekg::input::action("textbox-action-tab")) {
        this->process_text(cursor, "\t", ekg::ui::textbox_widget::action::add_text, 1);
      } else if (this->is_clipboard_copy || this->is_clipboard_paste || this->is_clipboard_cut) {
        this->process_text(cursor, "clipboard", ekg::ui::textbox_widget::action::add_text, 0);
      }

      if (cursor_dir[0] != 0 || cursor_dir[1] != 0) {
        this->check_nearest_word(cursor, cursor_dir[0], cursor_dir[1]);
        this->move_target_cursor(cursor, cursor_dir[0], cursor_dir[1]);
      }
    }
  }
}

void ekg::ui::textbox_widget::on_post_event(ekg::os::io_event_serial &io_event_serial) {
  abstract_widget::on_post_event(io_event_serial);
  this->embedded_scroll.flag.hovered = false;
  this->embedded_scroll.flag.activity = false;
}

void ekg::ui::textbox_widget::on_update() {
  this->embedded_scroll.on_update();
  this->is_high_frequency = this->embedded_scroll.check_activity_state(this->flag.focused || this->flag.hovered);
}

/**
 * The find cursor method perform bloat,
 * but how the entire selection system is CPU-batched (it means not called all the render time),
 * the performance is not necessary bad.
 */
bool ekg::ui::textbox_widget::find_cursor(
  ekg::ui::textbox_widget::cursor &target_cursor,
  int64_t utf_char_index,
  int64_t chunk_index,
  bool last_line_utf_char_index
) {
  bool a_cursor_pos {};
  bool b_cursor_pos {};

  for (ekg::ui::textbox_widget::cursor &cursor: this->loaded_multi_cursor_list) {
    a_cursor_pos = (
      utf_char_index >= cursor.pos[0].text_index ||
      (
        last_line_utf_char_index &&
        utf_char_index + 1 >= cursor.pos[0].text_index)
      ) && chunk_index == cursor.pos[0].chunk_index;

    b_cursor_pos = (
      (
        (
          cursor.pos[0] == cursor.pos[1] &&
          utf_char_index <= cursor.pos[1].text_index
        ) ||
        utf_char_index < cursor.pos[1].text_index
      ) ||
        (
          last_line_utf_char_index &&
          utf_char_index + 1 < cursor.pos[1].text_index)
      ) && chunk_index == cursor.pos[1].chunk_index;

    if (
        (
          cursor.pos[0].chunk_index != cursor.pos[1].chunk_index &&
          (a_cursor_pos || b_cursor_pos)
        ) ||
          (
            a_cursor_pos && b_cursor_pos
          ) ||
        (
          chunk_index > cursor.pos[0].chunk_index &&
          chunk_index < cursor.pos[1].chunk_index
        )
      ) {
      target_cursor = cursor;
      return true;
    }
  }

  return false;
}

void ekg::ui::textbox_widget::on_draw_refresh() {
  auto p_ui {(ekg::ui::textbox *) this->p_data};
  auto &rect {this->get_abs_rect()};
  auto &f_renderer {ekg::f_renderer(p_ui->get_font_size())};
  auto &theme {ekg::theme()};
  auto &allocator {ekg::core->gpu_allocator};

  ekg::draw::sync_scissor(this->scissor, rect, this->p_parent_scissor);
  ekg::draw::rect(rect, theme.textbox_background);

  this->embedded_scroll.clamp_scroll();

  float x {rect.x + this->embedded_scroll.scroll.x};
  float y {};

  ekg::gpu::data_t &data {allocator.bind_current_data()};
  ekg::vec4 color {theme.textbox_string};

  ekg_draw_assert_scissor();

  x = static_cast<float>(static_cast<int32_t>(x));
  data.buffer_content[0] = x;

  data.buffer_content[2] = static_cast<float>(ekg::gpu::allocator::concave);
  data.buffer_content[3] = static_cast<float>(ekg::gpu::allocator::concave);

  data.buffer_content[4] = color.x;
  data.buffer_content[5] = color.y;
  data.buffer_content[6] = color.z;
  data.buffer_content[7] = color.w;
  data.factor = 1;

  ekg::rect vertices {};
  ekg::rect coordinates {};

  this->rect_text.x = 0.0f;
  this->rect_text.y = 0.0f;
  this->is_ui_enabled = p_ui->get_state() == ekg::state::enable;

  char32_t char32 {};
  std::string utf_string {};
  uint8_t char8 {};

  ekg::ui::textbox_widget::cursor cursor {};

  uint64_t text_size {};
  uint64_t utf_char_index {};

  uint64_t it {};
  uint64_t text_chunk_size {this->p_text_chunk_list->size()};

  bool optimize_batching {};
  bool do_not_fill_line {};
  bool draw_additional_selected_last_char {};
  bool is_utf_char_last_index {};
  bool render_cursor {};

  /**
   * 0 == previous char wsize
   * 1 == current char wisze
   * 2 == next char wsize
   */
  this->cursor_char_wsize[0] = 0.0f;
  this->cursor_char_wsize[1] = 0.0f;
  this->cursor_char_wsize[2] = 0.0f;

  if (this->update_ui_text) {
    this->update_ui_text_data();
    this->update_ui_text = false;
  }

  this->cursor_draw_data_list.clear();
  this->rect_text.h = (this->text_height * static_cast<float>(text_chunk_size));

  /**
   * This line of code check the renderable text chunk index value,
   * dynamically calculating the amount of scroll with the size of
   * rect text height.
   */
  this->visible_text[1] = (
    this->embedded_scroll.scroll.y == 0.0f ?
      0 :
      static_cast<uint64_t>(((-this->embedded_scroll.scroll.y) / this->rect_text.h) * text_chunk_size)
  );

  // Multiply with the current visible index for get the perfect y position.
  y = (this->text_height * static_cast<float>(this->visible_text[1]));

  // Get the diff. between the visible text position and subtract with rect position for performn the scrolling effect.
  float visible_text_height {this->text_height * this->visible_text[1]};
  float rendering_text_scroller_diff {rect.y - (rect.y + this->embedded_scroll.scroll.y + visible_text_height)};
  float rendering_text_offset {((this->text_height / 2) - ((this->text_height - f_renderer.offset_text_height) / 2))};

  data.buffer_content[1] = floorf(rect.y);

  // Prevent from floating point loss in GPU.
  y = static_cast<float>(static_cast<int32_t>(floorf(-rendering_text_scroller_diff)));

  /**
   * The text iterator jump utf 8 - 16 - 32 sequences.
   * For better performance, textbox does not render texts out of rect.
   */
  for (uint64_t chunk_index {this->visible_text[1]}; chunk_index < text_chunk_size; chunk_index++) {
    std::string &text {this->p_text_chunk_list->at(chunk_index)};

    x = this->rect_cursor.w;
    f_renderer.ft_uint_previous = 0;
    utf_char_index = 0;
    do_not_fill_line = false;

    data.factor += static_cast<int32_t>(y) + 32;
    if (y > rect.h) {
      data.factor += static_cast<int32_t>(text_chunk_size) * 32;
      break;
    }

    if (text.empty() &&
        this->find_cursor(cursor, static_cast<int64_t>(utf_char_index), static_cast<int64_t>(chunk_index), false)) {
      do_not_fill_line = true;

      ekg::rect &select_rect {this->cursor_draw_data_list.emplace_back()};
      select_rect.x = x;
      select_rect.y = y;
      select_rect.w = this->rect_cursor.w + ((this->rect_cursor.w) * static_cast<float>(cursor.pos[0] != cursor.pos[1]));
      select_rect.h = this->text_height;
    }

    text_size = ekg::utf_length(text);
    for (it = 0; it < text.size(); it++) {
      char8 = static_cast<uint8_t>(text.at(it));
      it += ekg::utf_check_sequence(char8, char32, utf_string, text, it);

      if (f_renderer.ft_bool_kerning && f_renderer.ft_uint_previous) {
        FT_Get_Kerning(f_renderer.ft_face, f_renderer.ft_uint_previous, char32, 0, &f_renderer.ft_vector_previous_char);
        x += static_cast<float>(f_renderer.ft_vector_previous_char.x >> 6);
      }

      ekg::draw::glyph_char_t &char_data {f_renderer.allocated_char_data[char32]};
      is_utf_char_last_index = utf_char_index + 1 == text_size;

      if (this->find_cursor(
          cursor,
          static_cast<int64_t>(utf_char_index),
          static_cast<int64_t>(chunk_index),
          is_utf_char_last_index
        ) &&
          !(chunk_index > cursor.pos[0].chunk_index && chunk_index < cursor.pos[1].chunk_index)) {
        ekg::rect &select_rect {this->cursor_draw_data_list.emplace_back()};

        // The end of line cursor drawing require check before to addition.
        select_rect.x = (
          x +
          (
            char_data.wsize * static_cast<float>(
              is_utf_char_last_index &&
              cursor.pos[0] == cursor.pos[1] &&
              cursor.pos[0].text_index == utf_char_index + 1
            )
          )
        );

        select_rect.y = y;

        // Draw the offset signal for next line selected.
        select_rect.w = (
          cursor.pos[0] != cursor.pos[1] ?
            (
              char_data.wsize +
              (this->rect_cursor.w + this->rect_cursor.w) *
              (is_utf_char_last_index && cursor.pos[1].chunk_index > chunk_index && cursor.pos[0].chunk_index == chunk_index)
            ) :
            (
              this->rect_cursor.w
            )
          );

        select_rect.h = this->text_height;
      }

      if (x + this->embedded_scroll.scroll.x < rect.w) {
        vertices.x = x + char_data.left;
        vertices.y = y + f_renderer.full_height - char_data.top;

        vertices.w = char_data.w;
        vertices.h = char_data.h;

        coordinates.x = char_data.x;
        coordinates.w = vertices.w / f_renderer.full_width;
        coordinates.h = vertices.h / f_renderer.full_height;

        allocator.push_back_geometry(
          vertices.x,
          vertices.y,
          coordinates.x,
          coordinates.y
        );
        
        allocator.push_back_geometry(
          vertices.x,
          vertices.y + vertices.h,
          coordinates.x,
          coordinates.y + coordinates.h
        );

        allocator.push_back_geometry(
          vertices.x + vertices.w,
          vertices.y + vertices.h,
          coordinates.x + coordinates.w,
          coordinates.y + coordinates.h
        );

        allocator.push_back_geometry(
          vertices.x + vertices.w,
          vertices.y + vertices.h,
          coordinates.x + coordinates.w,
          coordinates.y + coordinates.h
        );

        allocator.push_back_geometry(
          vertices.x + vertices.w,
          vertices.y,
          coordinates.x + coordinates.w,
          coordinates.y
        );
        
        allocator.push_back_geometry(
          vertices.x,
          vertices.y,
          coordinates.x,
          coordinates.y
        );

        data.factor += static_cast<int32_t>(x) + static_cast<int32_t>(char32);
      }

      if (x + this->embedded_scroll.scroll.x > rect.w) {
        break;
      }

      f_renderer.ft_uint_previous = char32;
      x += char_data.wsize;
      utf_char_index++;
    }

    if (!do_not_fill_line && chunk_index > cursor.pos[0].chunk_index && chunk_index < cursor.pos[1].chunk_index) {
      ekg::rect &select_rect {this->cursor_draw_data_list.emplace_back()};
      select_rect.x = this->rect_cursor.w;
      select_rect.y = y;
      select_rect.w = x + this->rect_cursor.w + this->rect_cursor.w;
      select_rect.h = this->text_height;
    }

    y += this->text_height;
  }

  allocator.bind_texture(f_renderer.get_sampler_texture());
  allocator.dispatch();

  bool draw_cursor {this->flag.focused && !ekg::reach(ekg::core->ui_timing, 500)};
  for (ekg::rect &cursor_rect: this->cursor_draw_data_list) {
    cursor_rect.x = cursor_rect.x + rect.x + this->embedded_scroll.scroll.x;
    cursor_rect.y = cursor_rect.y + f_renderer.offset_text_height + f_renderer.offset_text_height + rect.y;

    switch (static_cast<int32_t>(cursor_rect.w)) {
      case 2:
        if (draw_cursor) {
          ekg::draw::rect(cursor_rect, theme.textbox_cursor);
        }

        break;
      default:
        ekg::draw::rect(cursor_rect, theme.textbox_select);
        break;
    }
  }

  this->rect_text.h += this->text_offset + ekg_pixel_div_2;
  this->embedded_scroll.rect_child = this->rect_text;
  this->embedded_scroll.scissor = this->scissor;
  this->embedded_scroll.on_draw_refresh();

  ekg::draw::rect(rect, theme.textbox_outline, ekg::draw_mode::outline);
}