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

#ifndef EKG_UI_LISTBOX_WIDGET_H
#define EKG_UI_LISTBOX_WIDGET_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include "ekg/util/io.hpp"

namespace ekg::ui {
    class listbox_widget : public ekg::ui::abstract_widget {
    public:
        struct container {
        public:
            std::vector<ekg::rect>
        };
    public:
        ekg::item *p_item {};
        ekg::rect rect_widget {};
        ekg::component component_category_last {};
        std::vector<ekg::item*> loaded_item_list {};
        bool semaphore {};
        ekg::vec2 category_font_metrics {};
        ekg::vec2 item_font_metrics {};
    public:
        void process_component_template(ekg::item &parent_item);
    public:
        void on_create() override;
        void on_reload() override;
        void on_update() override;
        void on_event(SDL_Event &sdl_event) override;
        void on_draw_refresh() override;
    };
}

#endif