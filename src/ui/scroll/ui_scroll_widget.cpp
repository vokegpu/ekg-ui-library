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

#include "ekg/ui/scroll/ui_scroll_widget.hpp"
#include "ekg/draw/draw.hpp"
#include "ekg/ekg.hpp"

void ekg::ui::scroll_widget::on_destroy() {
}

void ekg::ui::scroll_widget::on_reload() {
    this->scroll.rect_mother = this->parent;
    this->scroll.mother_id = this->data->get_parent_id();
    this->scroll.widget_id = this->data->get_id();
    this->dimension.w = this->parent->w;
    this->dimension.h = this->parent->h;
    this->scroll.on_reload();
}

void ekg::ui::scroll_widget::on_pre_event(SDL_Event &sdl_event) {
    this->scroll.on_pre_event(sdl_event);
    this->flag.hovered = this->scroll.flag.hovered && !this->scroll.flag.activy;
    this->flag.absolute = this->scroll.is_dragging_bar() || this->scroll.flag.activy;
}

void ekg::ui::scroll_widget::on_event(SDL_Event &sdl_event) {
    this->flag.hovered = this->scroll.flag.hovered;
    this->scroll.on_event(sdl_event);

    if ((this->flag.absolute || this->scroll.flag.activy) && !this->is_high_frequency) {
        ekg::update_high_frequency(this);
        this->is_high_frequency = true;
    }

    if (this->scroll.flag.state || this->scroll.flag.extra_state || this->flag.absolute) {
        ekg::dispatch(ekg::env::redraw);
    }
}

void ekg::ui::scroll_widget::on_post_event(SDL_Event &sdl_event) {
    ekg::ui::abstract_widget::on_post_event(sdl_event);
    this->scroll.flag.hovered = false;
    this->scroll.flag.activy = false;
}

void ekg::ui::scroll_widget::on_update() {
    this->scroll.on_update();
    this->is_high_frequency = this->scroll.check_activy_state(this->flag.hovered || this->scroll.flag.activy);
}

void ekg::ui::scroll_widget::on_draw_refresh() {
    this->dimension.w = this->parent->w;
    this->dimension.h = this->parent->h;

    ekg::draw::bind_scissor(this->data->get_id());
    ekg::draw::sync_scissor(this->get_abs_rect(), this->data->get_parent_id());
    this->scroll.on_draw_refresh();
    ekg::draw::bind_off_scissor();
}