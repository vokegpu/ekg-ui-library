
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

#ifndef EKG_UI_POPUP_H
#define EKG_UI_POPUP_H

#include "ekg/ui/abstract/ui_abstract.hpp"
#include "ekg/util/io.hpp"
#include "ekg/ui/display.hpp"

namespace ekg::ui {
  class popup : public ekg::ui::abstract {
  protected:
    std::vector<ekg::ui::item> item_list {};

    uint16_t text_flags {};
    int32_t scaled_height {}, token_id {};
    ekg::font font_size;
  public:
    ekg::ui::popup *insert(const std::vector<std::string> &item_list);

    ekg::ui::popup *insert(std::string_view item_name);

    ekg::ui::popup *link(std::string_view item_name, ekg::ui::popup *p_popup);

    ekg::ui::popup *erase(std::string_view item_name);

    std::string &emplace_back();

    int64_t contains(std::string_view item_name);

    ekg::ui::item &get(uint64_t index);

    std::vector<ekg::ui::item> &get_item_list();

    ekg::ui::popup *set_pos(float x, float y);

    ekg::vec2 get_pos();

    ekg::ui::popup *set_text_align(uint16_t dock);

    uint16_t get_text_align();

    ekg::ui::popup *set_width(float w);

    float get_width();

    ekg::ui::popup *set_scaled_height(int32_t h);

    int32_t get_scaled_height();

    float get_height();

    ekg::ui::popup *set_font_size(ekg::font font);

    ekg::font get_font_size();
  };
}

#endif