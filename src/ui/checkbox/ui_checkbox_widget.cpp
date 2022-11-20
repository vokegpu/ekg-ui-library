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

#include "ekg/ui/checkbox/ui_checkbox_widget.hpp"
#include "ekg/util/env.hpp"
#include "ekg/ekg.hpp"
#include "ekg/draw/draw.hpp"

void ekg::ui::checkbox_widget::destroy() {
    abstract_widget::destroy();
}

void ekg::ui::checkbox_widget::on_reload() {
    abstract_widget::on_reload();

    auto ui {(ekg::ui::checkbox*) this->data};
    auto &rect {this->get_abs_rect()};
    auto dock {ui->get_text_dock()};
    auto scaled_height {ui->get_scaled_height()};
    auto f_renderer {ekg::f_renderer(ui->get_font_size())};

    float text_width {f_renderer.get_text_width(ui->get_text())};
    float text_height {f_renderer.get_text_height()};
    float box_size {text_height};

    float dimension_offset = text_height / 2;
    float offset {ekg::find_min_offset(text_width, dimension_offset)};
    float min_width {box_size + dimension_offset + dimension_offset + (ui->get_text().empty() ? 0 : text_width + dimension_offset)};

    this->dimension.w = ekg::min(this->dimension.w, min_width);
    this->dimension.h = (text_height + dimension_offset) * static_cast<float>(scaled_height);

    this->offset.w = box_size;
    this->offset.h = this->dimension.h - dimension_offset;

    ekg::set_rect_clamped(this->dimension, ekg::theme().min_widget_size);
    ekg::set_dock_scaled({0, 0, this->dimension.w, this->dimension.h}, {min_width, this->offset.h}, this->docker_text);

    if (ekg::bitwise::contains(dock, ekg::dock::center)) {
        this->extra.x = this->docker_text.center.x + offset + text_width;
        this->extra.y = this->docker_text.center.y;
        this->offset.x = this->docker_text.center.x + offset;
        this->offset.y = this->docker_text.center.y;
    }

    if (ekg::bitwise::contains(dock, ekg::dock::left)) {
        this->extra.x = this->docker_text.left.x + box_size + offset + offset;
        this->offset.x = this->docker_text.left.x + offset;
    }

    if (ekg::bitwise::contains(dock, ekg::dock::right)) {
        this->extra.x = this->docker_text.right.x + offset;
        this->offset.x = this->docker_text.right.x + box_size + offset + offset;
    }

    if (ekg::bitwise::contains(dock, ekg::dock::top)) {
        this->extra.y = this->docker_text.top.y;
        this->offset.y = this->docker_text.top.y;
    }

    if (ekg::bitwise::contains(dock, ekg::dock::bottom)) {
        this->extra.y = this->docker_text.bottom.y;
        this->offset.y = this->docker_text.bottom.y;
    }
}

void ekg::ui::checkbox_widget::on_pre_event(SDL_Event &sdl_event) {
    abstract_widget::on_pre_event(sdl_event);
}

void ekg::ui::checkbox_widget::on_event(SDL_Event &sdl_event) {
    abstract_widget::on_event(sdl_event);

    bool pressed {ekg::input::pressed()};
    bool released {ekg::input::released()};

    if (ekg::input::motion() || pressed || released) {
        ekg::set(this->flag.highlight, this->flag.hovered);
        ekg::set(this->flag.focused, this->flag.hovered && ekg::rect_collide_vec(this->offset + (this->dimension + *this->parent), ekg::interact()));
    }

    if (pressed && !this->flag.activy && this->flag.hovered && ekg::input::pressed("checkbox-activy")) {
        ekg::set(this->flag.activy, true);
    } else if (released && this->flag.activy) {
        if (this->flag.hovered) {
            auto ui {(ekg::ui::checkbox*) this->data};
            ui->set_value(!ui->get_value());
        }

        ekg::set(this->flag.activy, false);
    }
}

void ekg::ui::checkbox_widget::on_post_event(SDL_Event &sdl_event) {
    abstract_widget::on_post_event(sdl_event);
}

void ekg::ui::checkbox_widget::on_update() {
    abstract_widget::on_update();
}

void ekg::ui::checkbox_widget::on_draw_refresh() {
    abstract_widget::on_draw_refresh();
    auto ui {(ekg::ui::checkbox*) this->data};
    auto &rect {this->get_abs_rect()};
    auto &theme {ekg::theme()};
    auto &f_renderer {ekg::f_renderer(ui->get_font_size())};
    auto box {this->offset + rect};

    ekg::draw::bind_scissor(ui->get_id());
    ekg::draw::sync_scissor_pos(static_cast<int32_t>(rect.x), static_cast<int32_t>(rect.y));

    ekg::draw::rect(rect, theme.checkbox_background);
    ekg::draw::rect(rect, theme.checkbox_outline, 1);

    if (this->flag.highlight) {
        ekg::draw::bind_animation(this->data->get_id());
        ekg::draw::rect(rect, theme.checkbox_highlight);
        ekg::draw::bind_animation(false);
    }

    if (this->flag.focused && !ui->get_value()) {
        ekg::draw::bind_animation(this->data->get_id());
        ekg::draw::rect(box, theme.checkbox_highlight);
        ekg::draw::bind_animation(false);
    }

    ekg::draw::rect(box, theme.checkbox_highlight);

    if (this->flag.activy) {
        ekg::draw::bind_animation(this->data->get_id());
        ekg::draw::rect(this->flag.focused ? box : rect, theme.checkbox_activy);
        ekg::draw::bind_animation(false);
    }

    if (ui->get_value()) {
        ekg::draw::bind_animation(this->data->get_id());
        ekg::draw::rect(box, theme.checkbox_activy);
        ekg::draw::bind_animation(false);
    }

    if (ui->get_value() && this->flag.highlight) {
        ekg::draw::bind_animation(this->data->get_id());
        ekg::draw::rect(box, {theme.checkbox_activy, theme.checkbox_outline.w}, 1);
        ekg::draw::bind_animation(false);
    }

    f_renderer.blit(ui->get_text(), rect.x + this->extra.x, rect.y + this->extra.y, theme.checkbox_string);

    ekg::draw::bind_off_animation();
    ekg::draw::bind_off_scissor();
}
