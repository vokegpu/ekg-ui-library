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

#include "ekg/ui/listbox/ui_listbox_widget.hpp"
#include "ekg/ui/listbox/ui_listbox.hpp"

void ekg::ui::listbox_widget::process_component_template(ekg::item &parent_item) {
    for (uint64_t it {}; it < parent_item.size(); it++) {
        ekg::item &item {parent_item.at(it)};
        ekg::component &component {item.component};

        if (ekg::bitwise::contains(item.attr, ekg::attr::row)) {
            component.rect_dimension.x = rect.w;
            rect.w += this->rect_widget.w;
        }

        if (ekg::bitwise::contains(item.attr, ekg::attr::category)) {
            this->process_component_template(item);

            component.rect_dimension.h = 60.0f; // temp
            component.rect_dimension.w = 
        }

        parent_item.component.rect_dimension.w = ekg::min(component.rect_dimension.x + component.rect_dimension.w, parent_item.component.rect_dimension.w);
        parent_item.component.rect_dimension.h = ekg::min(component.rect_dimension.y + component.rect_dimension.h, parent_item.component.rect_dimension.h);
    }
}

void ekg::ui::listbox_widget::on_reload() {
    auto p_ui {(ekg::ui::listbox*) this->p_data};
    auto &ui_item {p_ui->item()};
    auto &rect {this->get_abs_rect()};

    this->rect_widget = {};
    this->process_component_template(ui_item);
}

void ekg::ui::listbox_widget::on_update() {

}

void ekg::ui::listbox_widget::on_draw_refresh() {
}