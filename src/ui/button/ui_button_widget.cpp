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

#include "ekg/ui/button/ui_button_widget.hpp"
#include "ekg/ui/button/ui_button.hpp"
#include "ekg/ekg.hpp"
#include "ekg/draw/draw.hpp"

void ekg::ui::button_widget::on_destroy() {
    abstract_widget::on_destroy();
}

void ekg::ui::button_widget::on_reload() {
    abstract_widget::on_reload();

    auto p_ui {(ekg::ui::button*) this->p_data};
    auto &rect {this->get_abs_rect()};
    auto f_renderer {ekg::f_renderer(p_ui->get_font_size())};

    float text_width {f_renderer.get_text_width(p_ui->get_text())};
    float text_height {f_renderer.get_text_height()};

    float dimension_offset {static_cast<float>((int32_t) (text_height / 2.0f))};
    float offset {ekg::find_min_offset(text_width, dimension_offset)};

    this->dimension.w = ekg::min(this->dimension.w, text_width);
    this->dimension.h = (text_height + dimension_offset) * static_cast<float>(p_ui->get_scaled_height());

    this->min_size.x = ekg::min(this->min_size.x, text_height);
    this->min_size.y = ekg::min(this->min_size.y, this->dimension.h);

    this->rect_text.w = text_width;
    this->rect_text.h = text_height;

    auto &layout {ekg::core->service_layout};
    layout.set_preset_mask({offset, offset, this->dimension.h}, ekg::axis::horizontal, this->dimension.w);
    layout.insert_into_mask({&this->rect_text, p_ui->get_text_align()});
    layout.process_layout_mask();

    auto &layout_mask {layout.get_layout_mask()};
    this->dimension.w = (int32_t)(this->dimension.w <= text_width ? layout_mask.w : this->dimension.w);
    this->dimension.h = ekg::min(this->dimension.h, layout_mask.h);
}

void ekg::ui::button_widget::on_pre_event(SDL_Event &sdl_event) {
    abstract_widget::on_pre_event(sdl_event);
}

void ekg::ui::button_widget::on_event(SDL_Event &sdl_event) {
    abstract_widget::on_event(sdl_event);

    bool pressed {ekg::input::pressed()};
    bool released {ekg::input::released()};

    if (ekg::input::motion() || pressed || released) {
        ekg::set(this->flag.highlight, this->flag.hovered);
    }

    if (pressed && !this->flag.activy && this->flag.hovered && ekg::input::action("button-activy")) {
        ekg::set(this->flag.activy, true);
    } else if (released && this->flag.activy) {
        auto p_ui {(ekg::ui::button*) this->p_data};
        p_ui->set_value(this->flag.hovered);

        auto callback {p_ui->get_callback()};
        if (p_ui->get_value() && callback != nullptr) {
            ekg::dispatch(callback);
            p_ui->set_value(false);
        }

        ekg::dispatch_ui_event(p_ui->get_tag().empty() ? "Unknown Button UI" : p_ui->get_tag(), "callback", (uint16_t) p_ui->get_type());
        ekg::set(this->flag.activy, false);
    }
}

void ekg::ui::button_widget::on_post_event(SDL_Event &sdl_event) {
    abstract_widget::on_post_event(sdl_event);
}

void ekg::ui::button_widget::on_update() {
    abstract_widget::on_update();
}

void ekg::ui::button_widget::on_draw_refresh() {
    abstract_widget::on_draw_refresh();
    auto p_ui {(ekg::ui::button*) this->p_data};
    auto &rect {this->get_abs_rect()};
    auto &theme {ekg::theme()};
    auto &f_renderer {ekg::f_renderer(p_ui->get_font_size())};

    ekg::draw::bind_scissor(p_ui->get_id());
    ekg::draw::sync_scissor(rect, p_ui->get_parent_id());

    ekg::draw::rect(rect, theme.button_background);
    ekg::draw::rect(rect, theme.button_outline, ekg::drawmode::outline);

    if (this->flag.highlight) {
        ekg::draw::rect(rect, theme.button_highlight);
    }

    if (this->flag.activy) {
        ekg::draw::rect(rect, theme.button_activy);
        ekg::draw::rect(rect, {theme.button_activy, theme.button_outline.w}, ekg::drawmode::outline);
    }

    f_renderer.blit(p_ui->get_text(), rect.x + this->rect_text.x, rect.y + this->rect_text.y, theme.button_string);
    ekg::draw::bind_off_scissor();
}