/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, free to use in anything (utility, hacking, cheat, game, software).
 * 5- Malware, rat and others virus. We do not care.
 * 6- Do not modify this license under any instance.
 *
 * @VokeGpu 2023 all rights reserved.
 */

#include "ekg/ui/button/ui_button_widget.hpp"
#include "ekg/ui/button/ui_button.hpp"
#include "ekg/util/env.hpp"
#include "ekg/ekg.hpp"
#include "ekg/draw/draw.hpp"

void ekg::ui::button_widget::on_destroy() {
    abstract_widget::on_destroy();
}

void ekg::ui::button_widget::on_reload() {
    abstract_widget::on_reload();

    auto ui {(ekg::ui::button*) this->data};
    auto &rect {this->get_abs_rect()};
    auto f_renderer {ekg::f_renderer(ui->get_font_size())};

    float text_width {f_renderer.get_text_width(ui->get_text())};
    float text_height {f_renderer.get_text_height()};

    float dimension_offset {static_cast<float>((int32_t) (text_height / 2.0f))};
    float offset {ekg::find_min_offset(text_width, dimension_offset)};

    this->dimension.w = ekg::min(this->dimension.w, text_width);
    this->dimension.h = (text_height + dimension_offset) * static_cast<float>(ui->get_scaled_height());

    this->min_size.x = ekg::min(this->min_size.x, text_height);
    this->min_size.y = ekg::min(this->min_size.y, this->dimension.h);

    this->rect_text.w = text_width;
    this->rect_text.h = text_height;

    auto &layout {ekg::core->service_layout};
    layout.set_preset_mask({offset, offset, this->dimension.h}, ekg::axis::horizontal, this->dimension.w);
    layout.insert_into_mask({&this->rect_text, ui->get_text_align()});
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
        auto ui {(ekg::ui::button*) this->data};
        ui->set_value(this->flag.hovered);

        auto callback {ui->get_callback()};
        if (ui->get_value() && callback != nullptr) {
            ekg::dispatch(callback);
            ui->set_value(false);
        }

        ekg::dispatch_ui_event(ui->get_tag().empty() ? "Unknown Button UI" : ui->get_tag(), "callback", (uint16_t) ui->get_type());
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
    auto ui {(ekg::ui::button*) this->data};
    auto &rect {this->get_abs_rect()};
    auto &theme {ekg::theme()};
    auto &f_renderer {ekg::f_renderer(ui->get_font_size())};

    ekg::draw::bind_scissor(ui->get_id());
    ekg::draw::sync_scissor(rect, ui->get_parent_id());

    ekg::draw::rect(rect, theme.button_background);
    ekg::draw::rect(rect, theme.button_outline, ekg::drawmode::outline);

    if (this->flag.highlight) {
        ekg::draw::rect(rect, theme.button_highlight);
    }

    if (this->flag.activy) {
        ekg::draw::rect(rect, theme.button_activy);
        ekg::draw::rect(rect, {theme.button_activy, theme.button_outline.w}, ekg::drawmode::outline);
    }

    f_renderer.blit(ui->get_text(), rect.x + this->rect_text.x, rect.y + this->rect_text.y, theme.button_string);
    ekg::draw::bind_off_scissor();
}