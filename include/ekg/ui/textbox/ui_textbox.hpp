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
#include "ekg/ui/display.hpp"
#include "ekg/util/io.hpp"

namespace ekg::ui {
  class textbox : public ekg::ui::abstract, public ekg::value_t<std::vector<std::string>, ekg::ui::textbox>  {
  protected:
    ekg::font font_size {};
    std::string formatted_text {};
    bool must_format_text {};
    ekg::state typing_state {};
    uint8_t tab_size {4};

    uint64_t max_lines {UINT32_MAX};
    uint64_t max_chars_per_line {UINT32_MAX};
  public:
    /**
     * Set UI `must_format_text` value.
     * If true `ekg::ui::listbox::get_text` re-format the content,
     * else false content is not re-formatted.
     */
    ekg::ui::textbox *set_must_format_text(bool state);

    /**
     * Return UI `must_format_text` value.
     */
    bool is_must_format_text();

    ekg::ui::textbox *set_font_size(ekg::font font);

    ekg::font get_font_size();

    ekg::ui::textbox *set_place(uint16_t dock);

    int32_t get_scaled_height();

    ekg::ui::textbox *set_width(float w);

    float get_width();

    ekg::ui::textbox *set_scaled_height(int32_t h);

    float get_height();

    /**
     * Return formatted text from text box content.
     * if `must_format_text` is true, then re-format the text,
     * and automatically set `must_format_text` to false.
     */
    std::string get_text();

    ekg::ui::textbox *set_tab_size(uint8_t size);

    uint8_t get_tab_size();

    ekg::ui::textbox *set_max_chars_per_line(uint64_t chars_per_line);

    uint64_t get_max_chars_per_line();

    ekg::ui::textbox *set_max_lines(uint64_t lines);

    uint64_t get_max_lines();

    /**
     * Set enabled/disabled typing state.
     * Textbox will not accept-type response until typing state is enabled.
     **/
    ekg::ui::textbox *set_typing_state(ekg::state state);

    /**
     * Returns typing state: enabled/disabled.
     **/
    ekg::state get_typing_state();
  };
}

#endif