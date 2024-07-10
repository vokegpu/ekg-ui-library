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

#ifndef EKG_UI_TEXT_BOX_WIDGET_H
#define EKG_UI_TEXT_BOX_WIDGET_H

#include <unordered_map>
#include "ekg/ui/scrollbar/ui_scrollbar_embedded_widget.hpp"
#include "ekg/ui/abstract/ui_abstract_widget.hpp"

/* start of `ekg_textbox_clamp_text_chunk_size` macro */
#define ekg_textbox_clamp_text_chunk_size(text_chunk_list, max_size) \
  if (p_ui->p_value->size() > max_size) p_ui->p_value->erase(p_ui->p_value->begin() + max_size, p_ui->p_value->end())
/* end of `ekg_textbox_clamp_text_chunk_size` macro */

/* start of `ekg_textbox_clamp_text_chunk_size` macro */
#define ekg_textbox_clamp_line(line, max_size) \
  if (line.size() > max_size) line.erase(line.begin() + max_size, line.end())
/* end of `ekg_textbox_clamp_line` macro */

/* start of `ekg_textbox_get_cursor_text` macro */
#define ekg_textbox_get_cursor_text(cursor) \
  p_ui->p_value->at(cursor.chunk_index)
/* end of `ekg_textbox_clamp_line` macro */

namespace ekg::ui {
  class textbox_widget : public ekg::ui::abstract_widget {
  public:
    enum action {
      add_text,
      erase_text,
      break_line
    };

    struct cursor_pos {
    public:
      int64_t select_index {};
      int64_t chunk_index {};
      int64_t text_index {};
      int64_t last_text_index {};
    public:
      inline bool operator>(ekg::ui::textbox_widget::cursor_pos &r) {
        return (this->text_index > r.text_index && this->chunk_index == r.chunk_index) ||
               this->chunk_index > r.chunk_index;
      }

      inline bool operator<(ekg::ui::textbox_widget::cursor_pos &r) {
        return (this->text_index < r.text_index && this->chunk_index == r.chunk_index) ||
               this->chunk_index < r.chunk_index;
      }

      inline bool operator>=(ekg::ui::textbox_widget::cursor_pos &r) {
        return (this->text_index >= r.text_index && this->chunk_index == r.chunk_index) ||
               this->chunk_index > r.chunk_index;
      }

      inline bool operator<=(ekg::ui::textbox_widget::cursor_pos &r) {
        return (this->text_index <= r.text_index && this->chunk_index == r.chunk_index) ||
               this->chunk_index < r.chunk_index;
      }

      inline bool operator==(ekg::ui::textbox_widget::cursor_pos &r) {
        return this->text_index == r.text_index && this->chunk_index == r.chunk_index;
      }

      inline bool operator!=(ekg::ui::textbox_widget::cursor_pos &r) {
        return !(*this == r);
      }
    };

    struct cursor {
    public:
      /*
       * pos[0] is the cursor position A,
       * pos[1] is the cursor position B,
       * pos[2] is the initial select position,
       * pos[3] is the select target side.
       */
      ekg::ui::textbox_widget::cursor_pos pos[4] {};
    };
  public:
    std::vector<ekg::rect> cursor_draw_data_list {};
    std::vector<uint64_t> text_utf_char_index_list {};
    uint64_t last_text_chunk_size {};

    std::string widget_side_text {};
    std::vector<ekg::ui::textbox_widget::cursor> loaded_multi_cursor_list {
        ekg::ui::textbox_widget::cursor()
    };

    ekg::rect rect_text {};
    ekg::rect rect_cursor {};
    ekg::ui::scrollbar_embedded_widget embedded_scroll {};

    float cursor_char_wsize[3] {};
    float text_offset {};
    float text_height {};
    ekg::vec2 cursor_delta {};

    bool text_edited {};
    bool update_ui_text {};
    bool is_ui_enabled {};
    bool is_action_modifier_enable {};
    bool is_action_select_enable {};
    bool was_typed {};

    bool is_clipboard_copy {};
    bool is_clipboard_cut {};
    bool is_clipboard_paste {};

    int64_t total_utf_chars {};
    std::string cached_tab_size {};
    uint64_t visible_text[4] {};
    uint64_t latest_size_until_refresh {};
  public:
    bool find_cursor(
      ekg::ui::textbox_widget::cursor &target_cursor,
      int64_t total_it,
      int64_t it_chunk,
      bool last_line_utf_char_index
    );

    void check_cursor_text_bounding(ekg::ui::textbox_widget::cursor &cursor, bool reset_second_cursor_pos);

    void process_text(
      ekg::ui::textbox_widget::cursor &cursor,
      std::string_view text,
      ekg::ui::textbox_widget::action action,
      int64_t direction
    );

    void move_cursor(ekg::ui::textbox_widget::cursor_pos &cursor, int64_t x, int64_t y);

    void update_ui_text_data();

    void check_nearest_word(ekg::ui::textbox_widget::cursor &cursor, int64_t &x, int64_t &y);

    void move_target_cursor(ekg::ui::textbox_widget::cursor &cursor, int64_t x, int64_t y);

    void clamp_text_chunk_size();

  public:
    void on_create() override;

    void on_reload() override;

    void on_pre_event(ekg::os::io_event_serial &io_event_serial) override;

    void on_event(ekg::os::io_event_serial &io_event_serial) override;

    void on_post_event(ekg::os::io_event_serial &io_event_serial) override;

    void on_update() override;

    void on_draw_refresh() override;
  };
}

#endif