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
 * @VokeGpu 2022 all rights reserved.
 */

#include "ekg/ui/button/ui_button_widget.hpp"
#include "ekg/ui/button/ui_button.hpp"
#include "ekg/util/env.hpp"
#include "ekg/ekg.hpp"
#include "ekg/draw/draw.hpp"

void ekg::ui::button_widget::destroy() {
    abstract_widget::destroy();
}

void ekg::ui::button_widget::on_reload() {
    abstract_widget::on_reload();

    auto ui {(ekg::ui::button*) this->data};
    auto &rect {this->get_abs_rect()};
    auto dock {ui->get_text_dock()};
    auto scaled_height {ui->get_scaled_height()};
    auto f_renderer {ekg::f_renderer(ui->get_font_size())};

    float text_width {f_renderer.get_text_width(ui->get_text())};
    float text_height {f_renderer.get_text_height()};

    float dimension_offset {text_height / 2};
    float offset {ekg::find_min_offset(text_width, dimension_offset)};

    this->dimension.w = ekg::min(this->dimension.w, text_width + dimension_offset);
    this->dimension.h = (text_height + dimension_offset) * static_cast<float>(scaled_height);

    ekg::set_rect_clamped(this->dimension, ekg::theme().min_widget_size);
    ekg::set_dock_scaled({0, 0, this->dimension.w, this->dimension.h}, {text_width, text_height}, this->docker_text);

    if (ekg::bitwise::contains(dock, ekg::dock::center)) {
        this->extra.x = this->docker_text.center.x;
        this->extra.y = this->docker_text.center.y;
    }

    if (ekg::bitwise::contains(dock, ekg::dock::left)) {
        this->extra.x = this->docker_text.left.x + offset;
    }

    if (ekg::bitwise::contains(dock, ekg::dock::right)) {
        this->extra.x = this->docker_text.right.x - offset;
    }

    if (ekg::bitwise::contains(dock, ekg::dock::top)) {
        this->extra.y = this->docker_text.top.y + offset;
    }

    if (ekg::bitwise::contains(dock, ekg::dock::bottom)) {
        this->extra.y = this->docker_text.bottom.y - offset;
    }
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

    if (pressed && !this->flag.activy && this->flag.hovered && ekg::input::pressed("button-activy")) {
        ekg::set(this->flag.activy, true);
    } else if (released && this->flag.activy) {
        auto ui {(ekg::ui::button*) this->data};
        ui->set_value(this->flag.hovered);

        if (ui->get_value() && ui->get_callback() != nullptr) {
            ekg::dispatch(ui->get_callback());
            ui->set_value(false);
        }

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
    ekg::draw::rect(rect, theme.button_background);
    ekg::draw::rect(rect, theme.button_outline, 1);

    if (this->flag.highlight) {
        ekg::draw::rect(rect, theme.button_highlight);
    }

    if (this->flag.activy) {
        ekg::draw::rect(rect, theme.button_activy);
        ekg::draw::rect(rect, {theme.button_activy, theme.button_outline.w}, 1);
    }

    f_renderer.blit(ui->get_text(), rect.x + this->extra.x, rect.y + this->extra.y, theme.button_string);
    ekg::draw::bind_off_scissor();
}