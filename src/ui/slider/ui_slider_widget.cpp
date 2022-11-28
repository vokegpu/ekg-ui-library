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

#include "ekg/ui/slider/ui_slider_widget.hpp"
#include "ekg/ui/slider/ui_slider.hpp"
#include "ekg/ekg.hpp"
#include "ekg/draw/draw.hpp"

void ekg::ui::slider_widget::update_bar(float x, float y) {
    auto ui {(ekg::ui::slider*) this->data};
    auto &rect {this->get_abs_rect()};
    auto bar {this->offset + rect};
    auto orientation {ui->get_bar_dock()};
    float factor {}, dimension_factor {}, min {ui->get_value_min()}, max {ui->get_value_max()};

    switch (orientation) {
        case ekg::dock::left: {
            break;
        }

        default: {
            factor = ekg::min(x - bar.x, 0.0f);
            dimension_factor = bar.w;
            break;
        }
    }

    if (factor == 0) {
        factor = ui->get_value_min();
    } else {
        factor = (factor / dimension_factor) * (max - min) + min;
    }

    ui->set_value(factor);
}

void ekg::ui::slider_widget::destroy() {
    abstract_widget::destroy();
}

void ekg::ui::slider_widget::on_reload() {
    abstract_widget::on_reload();

    auto ui {(ekg::ui::slider*) this->data};
    auto &rect {this->get_abs_rect()};
    auto dock {ui->get_bar_dock()};
    auto scaled_height {ui->get_scaled_height()};
    auto f_renderer {ekg::f_renderer(ui->get_font_size())};

    /* Use text height as text width because slider does not have text to display.. */
    float text_height {f_renderer.get_text_height()};
    float text_width {f_renderer.get_text_height()};

    float dimension_offset {text_height / 2};
    float offset {ekg::find_min_offset(text_width, dimension_offset)};
    float value {ui->get_value()}, min {ui->get_value_min()}, max {ui->get_value_max()};

    this->dimension.w = ekg::min(this->dimension.w, dimension_offset * 2);
    this->dimension.h = ekg::min(this->dimension.h, (text_height + dimension_offset) * static_cast<float>(ui->get_scaled_height()));

    /*
      Offset rect is the fully bar metric.
      Extra rect is the current value bar metric.
     */

    // todo bar top, down & left orientation.;

    switch (dock) {
        case ekg::dock::top: {
            break;
        }

        case ekg::dock::bottom: {
            break;
        }

        case ekg::dock::left: {
            break;
        }

        default: {
            this->offset.w = rect.w - offset;
            this->offset.h = (this->dimension.h / 2) - offset;
            this->offset.x = offset;
            this->offset.y = (this->dimension.h / 2) - (this->offset.h / 2);
            this->extra.h = this->offset.h;
            this->extra.w = this->offset.w * (value - min) / (max - min);
            break;
        }
    }

    this->extra.x = this->offset.x;
    this->extra.y = this->offset.y;
}

void ekg::ui::slider_widget::on_pre_event(SDL_Event &sdl_event) {
    abstract_widget::on_pre_event(sdl_event);
}

void ekg::ui::slider_widget::on_event(SDL_Event &sdl_event) {
    abstract_widget::on_event(sdl_event);

    auto ui {(ekg::ui::slider*) this->data};
    auto &rect {this->get_abs_rect()};
    auto &interact {ekg::interact()};
    bool pressed {ekg::input::pressed()}, released {ekg::input::released()}, motion {ekg::input::motion()}, increase {}, descrease {};

    if (motion || pressed || released) {
        ekg::set(this->flag.highlight, this->flag.hovered && ekg::rect_collide_vec(this->offset + rect, interact));
    }

    if (this->flag.hovered && ekg::input::wheel() && ((increase = ekg::input::pressed("slider-bar-increase")) || (descrease = ekg::input::pressed("slider-bar-decrease")))) {
        ui->set_value(ui->get_value() + (interact.w * 2));
    } else if (this->flag.hovered && pressed && ekg::input::pressed("slider-activy")) {
        this->flag.activy = true;
    } else if  (released) {
        this->flag.activy = false;
    } else if (this->flag.activy && motion) {
        this->update_bar(interact.x, interact.y);
    }
}

void ekg::ui::slider_widget::on_post_event(SDL_Event &sdl_event) {
    abstract_widget::on_post_event(sdl_event);
}

void ekg::ui::slider_widget::on_update() {
    abstract_widget::on_update();
}

void ekg::ui::slider_widget::on_draw_refresh() {
    abstract_widget::on_draw_refresh();
    auto ui {(ekg::ui::slider*) this->data};
    auto &rect {this->get_abs_rect()};
    auto &theme {ekg::theme()};
    auto &f_renderer_small {ekg::f_renderer(ekg::font::small)};

    ekg::draw::bind_scissor(ui->get_id());
    ekg::draw::sync_scissor_pos(rect.x, rect.y);
    ekg::draw::rect(this->offset + rect, theme.slider_background);

    if (this->flag.highlight) {
        ekg::draw::rect(this->offset + rect, theme.slider_highlight);
    }
    
    ekg::draw::rect(this->extra + rect, theme.slider_activy);
    ekg::draw::bind_off_scissor();

    ekg::draw::rect(20, 80, 200, 200, {255, 255, 255, 255}, -1);
}