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

#include "ekg/ui/label/ui_label_widget.hpp"
#include "ekg/ekg.hpp"
#include "ekg/draw/draw.hpp"

void ekg::ui::label_widget::on_destroy() {
    abstract_widget::on_destroy();
}

void ekg::ui::label_widget::on_reload() {
    abstract_widget::on_reload();

    auto ui {(ekg::ui::label*) this->data};
    auto &rect {this->get_abs_rect()};
    auto &f_renderer {ekg::f_renderer(ui->get_font_size())};
    auto scaled_height {ui->get_scaled_height()};

    float text_width {f_renderer.get_text_width(ui->get_text(), scaled_height)};
    float text_height {f_renderer.get_text_height()};

    float dimension_offset {text_height / 2};
    float offset {ekg::find_min_offset(text_width, dimension_offset)};

    this->dimension.w = ekg::min(this->dimension.w, text_width);
    this->dimension.h = (text_height + dimension_offset) * static_cast<float>(scaled_height);

    this->min_size.x = ekg::min(this->min_size.x, text_height);
    this->min_size.y = ekg::min(this->min_size.y, this->dimension.h);

    this->rect_text.w = text_width;
    this->rect_text.h = text_height * static_cast<float>(scaled_height);

    auto &layout {ekg::core->service_layout};
    layout.set_preset_mask({offset, offset, this->dimension.h}, ekg::axis::horizontal, this->dimension.w);
    layout.insert_into_mask({&this->rect_text, ui->get_text_align()});
    layout.process_layout_mask();

    auto &layout_mask {layout.get_layout_mask()};

    ekg::assert_task_flag = false;
    this->dimension.w = ekg::assert_task(this->dimension.w, this->dimension.w <= text_width ? layout_mask.w : this->dimension.w);
    this->dimension.h = ekg::assert_task(this->dimension.h, ekg::min(this->dimension.h, layout_mask.h));

    if (ekg::assert_task_flag && ui->has_parent()) {
        ekg::synclayout(this);
        this->is_targeting_absolute_parent = true;
    }
}

void ekg::ui::label_widget::on_pre_event(SDL_Event &sdl_event) {
    abstract_widget::on_pre_event(sdl_event);
}

void ekg::ui::label_widget::on_event(SDL_Event &sdl_event) {
    abstract_widget::on_event(sdl_event);
}

void ekg::ui::label_widget::on_post_event(SDL_Event &sdl_event) {
    abstract_widget::on_post_event(sdl_event);
}

void ekg::ui::label_widget::on_update() {
}

void ekg::ui::label_widget::on_draw_refresh() {
    auto ui {(ekg::ui::label*) this->data};
    auto &rect {this->get_abs_rect()};
    auto &f_renderer {ekg::f_renderer(ui->get_font_size())};

    ekg::draw::bind_scissor(this->data->get_id());
    ekg::draw::sync_scissor(rect, ui->get_parent_id());
    f_renderer.blit(ui->get_text(), rect.x + this->rect_text.x, rect.y + this->rect_text.y, ekg::theme().label_string);
    ekg::draw::bind_off_scissor();
}