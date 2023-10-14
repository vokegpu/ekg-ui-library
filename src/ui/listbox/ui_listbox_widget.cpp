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
#include "ekg/ekg.hpp"
#include "ekg/draw/draw.hpp"

void ekg::ui::listbox_widget::process_component_template(ekg::item &parent_item) {
    auto p_ui {(ekg::ui::listbox*) this->p_data};
    auto &category_f_renderer {ekg::f_renderer(p_ui->get_category_font_size())};
    auto &item_f_renderer {ekg::f_renderer(p_ui->get_item_font_size())};

    ekg::rect rect_dimension_update {parent_item.component.rect_dimension_closed};

    for (uint64_t it {}; it < parent_item.size(); it++) {
        ekg::item &item {parent_item.at(it)};
        ekg::component &component {item.component};

        component.rect_dimension_closed.x = rect_dimension_update.x;
        component.rect_dimension_closed.y = rect_dimension_update.y + rect_dimension_update.h;
        component.rect_dimension_closed.w = this->component_category_last.rect_dimension_closed.w;
        component.rect_dimension_closed.h = item_f_renderer.get_text_height();

        if (ekg::bitwise::contains(item.attr, ekg::attr::row)) {
            this->rect_widget.w  += this->component_category_last.rect_dimension_closed.w;
            component.rect_dimension_closed.x = this->rect_widget.x + this->rect_widget.w;
        } else {
            rect_dimension_update.h += component.rect_dimension_closed.h;
        }

        if (ekg::bitwise::contains(item.attr, ekg::attr::category)) {
            component.rect_dimension_closed.w = category_f_renderer.get_text_width(item.value) + 50.0f;
            component.rect_dimension_closed.h = category_f_renderer.get_text_height();
            this->component_category_last = component;
        }

        if (!item.empty()) {
            this->process_component_template(item);
        }

        parent_item.component.rect_dimension_opened.w = ekg::min(parent_item.component.rect_dimension_closed.x + rect_dimension_update.w, parent_item.component.rect_dimension.w);
        parent_item.component.rect_dimension_opened.h = ekg::min(parent_item.component.rect_dimension_closed.y + rect_dimension_update.h, parent_item.component.rect_dimension.h);
        
        this->loaded_item_list.push_back(&item);
    }
}

void ekg::ui::listbox_widget::on_reload() {
    auto p_ui {(ekg::ui::listbox*) this->p_data};
    auto &ui_item {p_ui->item()};
    auto &rect {this->get_abs_rect()};

    this->rect_widget = {};
    this->component_category_last = {};
    this->loaded_item_list = {};
    this->process_component_template(ui_item);

    auto &item_f_renderer {ekg::f_renderer(p_ui->get_item_font_size())};
    this->dimension.h = item_f_renderer.get_text_height() * static_cast<float>(p_ui->get_scaled_height());
}

void ekg::ui::listbox_widget::on_update() {

}

void ekg::ui::listbox_widget::on_draw_refresh() {
    auto p_ui {(ekg::ui::listbox*) this->p_data};

    auto &rect {this->get_abs_rect()};
    auto &theme {ekg::theme()};
    auto &category_f_renderer {ekg::f_renderer(p_ui->get_category_font_size())};
    auto &item_f_renderer {ekg::f_renderer(p_ui->get_item_font_size())};

    ekg::draw::bind_scissor(p_ui->get_id());
    ekg::draw::sync_scissor(rect, p_ui->get_parent_id());

    // draw the outline frame
    ekg::draw::rect(rect, theme.listbox_outline, ekg::draw_mode::outline);

    for (ekg::item *&p_item : this->loaded_item_list) {
        if (ekg::bitwise::contains(p_item->attr, ekg::attr::category)) {
            ekg::draw::rect(p_item->component.rect_dimension_closed + rect, theme.listbox_category_background);
            ekg::draw::rect(p_item->component.rect_dimension_closed + rect, theme.listbox_category_outline, ekg::draw_mode::outline);

            category_f_renderer.blit(p_item->value,
                                     p_item->component.rect_dimension_closed.x + rect.x,
                                     p_item->component.rect_dimension_closed.y + rect.y,
                                     theme.listbox_category_string);
        } else {
            ekg::draw::rect(p_item->component.rect_dimension_closed + rect, theme.listbox_item_background);
            ekg::draw::rect(p_item->component.rect_dimension_closed + rect, theme.listbox_item_outline, ekg::draw_mode::outline);

            item_f_renderer.blit(p_item->value,
                                 p_item->component.rect_dimension_closed.x + rect.x,
                                 p_item->component.rect_dimension_closed.y + rect.y,
                                 theme.listbox_category_string);
        }

        if (ekg::bitwise::contains(p_item->attr, ekg::attr::separator)) {
            ekg::draw::rect(p_item->component.rect_dimension_closed.x + rect.x, 
                            p_item->component.rect_dimension_closed.y + rect.y + p_item->component.rect_dimension_closed.h - 0.5f,
                            p_item->component.rect_dimension_closed.w,
                            1.0f,
                            theme.listbox_separator);
        }
    }

    ekg::draw::bind_off_scissor();
}