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

        // measure that this componet is does not contains category attribute
        component.rect_dimension_closed.x = rect_dimension_update.x;
        component.rect_dimension_closed.y = rect_dimension_update.y + rect_dimension_update.h;
        component.rect_dimension_closed.w = this->component_category_last.rect_dimension_closed.w;
        component.rect_dimension_closed.h = this->item_font_metrics.y + this->item_font_metrics.x;

        if (ekg::bitwise::contains(item.attr, ekg::attr::row)) {
            this->rect_widget.w  += this->component_category_last.rect_dimension_closed.w;
            component.rect_dimension_closed.x = this->rect_widget.x + this->rect_widget.w;

            /*
             * Update the size of parent item if the item attr contains row flag.
             */
            if (item.p_item_parent != nullptr) {
                item.p_item_parent->component.rect_dimension_closed.w = this->rect_widget.w + component.rect_dimension_closed.w - item.p_item_parent->component.rect_dimension_closed.x;
            }
        } else {
            rect_dimension_update.h += component.rect_dimension_closed.h;
        }

        if (ekg::bitwise::contains(item.attr, ekg::attr::category)) {
            component.rect_text.w = category_f_renderer.get_text_width(item.value);
            component.rect_text.h = this->category_font_metrics.y;

            component.rect_dimension_closed.w = component.rect_text.w + 50.0f;
            component.rect_dimension_closed.h = this->category_font_metrics.y + this->category_font_metrics.x;

            component.rect_text.x = ekg::find_min_offset(category_f_renderer.get_text_width(item.value), this->category_font_metrics.x);
            component.rect_text.y = component.rect_text.x;

            this->component_category_last = component;
        } else {
            component.rect_text.w = item_f_renderer.get_text_width(item.value);
            component.rect_text.h = this->item_font_metrics.y;

            component.rect_text.x = ekg::find_min_offset(item_f_renderer.get_text_width(item.value), this->item_font_metrics.x);
            component.rect_text.y = component.rect_text.x;
        }

        if (!item.empty()) {
            this->process_component_template(item);
        }

        parent_item.component.rect_dimension_opened.w = ekg::min(parent_item.component.rect_dimension_closed.x + rect_dimension_update.w, parent_item.component.rect_dimension.w);
        parent_item.component.rect_dimension_opened.h = ekg::min(parent_item.component.rect_dimension_closed.y + rect_dimension_update.h, parent_item.component.rect_dimension.h);
        
        this->loaded_item_list.push_back(&item);
    }
}

void ekg::ui::listbox_widget::on_create() {
    auto p_ui {(ekg::ui::listbox*) this->p_data};
    auto &item {p_ui->item()};
    item.p_semaphore = &this->semaphore;
}

void ekg::ui::listbox_widget::on_reload() {
    auto p_ui {(ekg::ui::listbox*) this->p_data};
    auto &ui_item {p_ui->item()};
    auto &rect {this->get_abs_rect()};

    auto &category_f_renderer {ekg::f_renderer(p_ui->get_category_font_size())};
    auto &item_f_renderer {ekg::f_renderer(p_ui->get_item_font_size())};

    this->category_font_metrics.y = category_f_renderer.get_text_height();
    this->category_font_metrics.x = this->category_font_metrics.y / 2.0f;

    this->item_font_metrics.y = item_f_renderer.get_text_height();
    this->item_font_metrics.x = this->item_font_metrics.x / 2.0f;

    this->rect_widget = {};
    this->component_category_last = {};
    this->loaded_item_list = {};
    this->process_component_template(ui_item);

    this->dimension.h = this->item_font_metrics.y * static_cast<float>(p_ui->get_scaled_height());
}

void ekg::ui::listbox_widget::on_event(SDL_Event &sdl_event) {
    bool motion {ekg::input::motion()};
    bool pressed {ekg::input::pressed()};
    bool released {ekg::input::released()};

    if (motion || pressed || released) {
        ekg::vec4 &interact {ekg::input::interact()};
        ekg::rect &rect {this->get_abs_rect()};

        uint64_t hovered {UINT64_MAX};
        std::vector<ekg::item*> hovered_item_list {};

        bool was_hovered {};
        bool should_redraw {};

        for (uint64_t it {}; it < this->loaded_item_list.size(); it++) {
            auto &p_item {this->loaded_item_list.at(it)};
            auto &component {p_item->component};

            was_hovered = ekg::rect_collide_vec(component.rect_dimension_closed + rect, interact);
            if (hovered == UINT64_MAX && was_hovered) {
                hovered = it;
                hovered_item_list.emplace_back() = p_item;
            }

            if (should_redraw == false && p_item->component.is_hovering != was_hovered) {
                ekg::dispatch(ekg::env::redraw);
                should_redraw = true;
            }

            p_item->component.is_hovering = was_hovered;
        }

        for (ekg::item *&p_item_hovered : hovered_item_list) {
            if (p_item_hovered != nullptr &&
                !ekg::bitwise::contains(p_item_hovered->attr, ekg::attr::category) &&
                !ekg::bitwise::contains(p_item_hovered->attr, ekg::attr::row) &&
                p_item_hovered->p_item_parent != nullptr &&
                ekg::bitwise::contains(p_item_hovered->p_item_parent->attr, ekg::attr::row_member) &&
                p_item_hovered->p_item_parent->p_item_parent != nullptr &&
                p_item_hovered->p_item_parent->p_item_parent->size() > 1) {

                ekg::item *&p_item_parent_mommy {p_item_hovered->p_item_parent->p_item_parent};

                uint64_t member_index {};
                bool found {};

                for (uint64_t it {}; it < p_item_hovered->p_item_parent->size(); it++) {
                    auto &item {p_item_hovered->p_item_parent->at(it)};
                    if (item.value == p_item_hovered->value) {
                        member_index = it;
                        found = true;
                        break;
                    }
                }

                if (found) {
                    for (ekg::item &items : *p_item_parent_mommy) {
                        if (ekg::bitwise::contains(items.attr, ekg::attr::row_member) && ekg::bitwise::contains(items.attr, ekg::attr::row)) {
                            if (items.size() > member_index) { 
                                auto &item_target {items.at(member_index)};                           
                                if (!ekg::bitwise::contains(item_target.attr, ekg::attr::row) && item_target.component.is_hovering != p_item_hovered->component.is_hovering) {
                                    item_target.component.is_hovering = p_item_hovered->component.is_hovering;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
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
                                     p_item->component.rect_dimension_closed.x + p_item->component.rect_text.x + rect.x,
                                     p_item->component.rect_dimension_closed.y + p_item->component.rect_text.y + rect.y,
                                     theme.listbox_category_string);
        } else {
            ekg::draw::rect(p_item->component.rect_dimension_closed + rect, theme.listbox_item_background);
            ekg::draw::rect(p_item->component.rect_dimension_closed + rect, theme.listbox_item_outline, ekg::draw_mode::outline);

            item_f_renderer.blit(p_item->value,
                                 p_item->component.rect_dimension_closed.x + p_item->component.rect_text.x + rect.x,
                                 p_item->component.rect_dimension_closed.y + p_item->component.rect_text.y + rect.y,
                                 theme.listbox_category_string);
        }

        if (p_item->component.is_hovering) {
            ekg::draw::rect(p_item->component.rect_dimension_closed + rect, theme.listbox_highlight);
        }

        if (ekg::bitwise::contains(p_item->attr, ekg::attr::separator)) {
            ekg::draw::rect(p_item->component.rect_dimension_closed.x + rect.x,
                            p_item->component.rect_dimension_closed.y + rect.y + p_item->component.rect_dimension_closed.h - ekg_pixel_div_2,
                            p_item->component.rect_dimension_closed.w,
                            1.0f,
                            theme.listbox_separator);
        }
    }

    ekg::draw::bind_off_scissor();
}