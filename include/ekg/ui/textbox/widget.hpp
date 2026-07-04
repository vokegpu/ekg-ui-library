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
#ifndef EKG_UI_TEXTBOX_WIDGET_HPP
#define EKG_UI_TEXTBOX_WIDGET_HPP

#include "ekg/ui/property.hpp"
#include "textbox.hpp"

namespace ekg::ui {
  enum class textbox_operation {
    insert_inline,
    insert_multiline,
    insert_line,
    erase_inline,
    erase_multiline
  };

  void refresh_cursors_pos(
    ekg::textbox_t &textbox,
    ekg::textbox_t::cursor_t &origin,
    const ekg::vec2_t<size_t> &displacement_a,
    const ekg::vec2_t<size_t> &displacement_b,
    const ekg::ui::textbox_operation &operation
  );

  bool find_cursor(
    ekg::textbox_t &textbox,
    ekg::vec2_t<size_t> &index,
    ekg::textbox_t::cursor_t &cursor_out
  );

  bool find_index_by_interact(
    ekg::property_t &property,
    ekg::textbox_t &textbox,
    ekg::vec2_t<float> &interact,
    ekg::vec2_t<size_t> &index
  );

  void handle_cursor_interact(
    ekg::property_t &property,
    ekg::textbox_t &textbox,
    bool picked_index,
    ekg::vec2_t<size_t> &pick_index,
    ekg::input_info_t &input
  );

  void handle_erase(
    ekg::textbox_t &textbox,
    ekg::textbox_t::cursor_t &cursor
  );

  void refresh_scroll_positions(
    ekg::textbox_t &textbox
  );

  void handle_insert(
    ekg::textbox_t &textbox,
    ekg::textbox_t::cursor_t &cursor,
    std::string_view typed
  );

  void reload(
    ekg::property_t &property,
    ekg::textbox_t &textbox
  );

  void event(
    ekg::property_t &property,
    ekg::textbox_t &textbox,
    const ekg::io::stage &stage
  );

  void high_frequency(
    ekg::property_t &property,
    ekg::textbox_t &textbox
  );

  void pass(
    ekg::property_t &property,
    ekg::textbox_t &textbox
  );

  void buffering(
    ekg::property_t &property,
    ekg::textbox_t &textbox
  );

  void unmap(
    ekg::textbox_t &textbox
  );
}

#endif
