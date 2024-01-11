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

#ifndef EKG_UI_TEXTBOX_H
#define EKG_UI_TEXTBOX_H

#include "ekg/ui/abstract/ui_abstract.hpp"

namespace ekg::ui {
  class textbox : public ekg::ui::abstract {
  protected:
    std::string text {};
    ekg::font font_size {};
    bool enabled {true};
    uint8_t tab_size {4};

    uint64_t max_lines {UINT32_MAX};
    uint64_t max_chars_per_line {UINT32_MAX};
  public:
    /**
     * As the name says, unsafe
     * due the desynchronization between widget
     * and UI interface.
     */
    void unsafe_set_text(std::string &ref_text);

  public:
    ekg::ui::textbox *set_enabled(bool enabled);

    bool is_enabled();

    ekg::ui::textbox *set_font_size(ekg::font font);

    ekg::font get_font_size();

    ekg::ui::textbox *set_place(uint16_t dock);

    int32_t get_scaled_height();

    ekg::ui::textbox *set_width(float w);

    float get_width();

    ekg::ui::textbox *set_scaled_height(int32_t h);

    float get_height();

    ekg::ui::textbox *set_text(std::string_view text);

    std::string get_text();

    ekg::ui::textbox *set_tab_size(uint8_t size);

    uint8_t get_tab_size();

    ekg::ui::textbox *set_max_chars_per_line(uint64_t chars_per_line);

    uint64_t get_max_chars_per_line();

    ekg::ui::textbox *set_max_lines(uint64_t lines);

    uint64_t get_max_lines();

    ekg::ui::textbox *clear();
  };
}

#endif