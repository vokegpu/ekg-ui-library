/*
 * MIT License
 * 
 * Copyright (c) 2022-2023 Rina Wilk / vokegpu@gmail.com
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

#ifndef EKG_UI_LISTBOX_H
#define EKG_UI_LISTBOX_H

#include "ekg/ui/abstract/ui_abstract.hpp"
#include  <vector>

namespace ekg::ui {
    class listbox : public ekg::ui::abstract {
    protected:
        std::vector<ekg::ui::item> item_list {};
        std::string value {};
    public:
        int64_t contains(std::string_view item_name);
        ekg::ui::listbox *insert(std::string_view item_name);
        ekg::ui::listbox *insert(const std::vector<std::string> &item_name_list);
        ekg::ui::listbox *erase(std::string_view item_name);
        std::vector<ekg::ui::item> &get_item_list();

        ekg::ui::listbox *set_value(std::string_view value_item_name);
        std::string get_value();        
    };
}

#endif