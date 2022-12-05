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
    auto bar {this->rect_bar + rect};
    auto orientation {ui->get_bar_align()};
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
    auto dock {ui->get_bar_align()};
    auto scaled_height {ui->get_scaled_height()};
    auto &f_renderer {ekg::f_renderer(ui->get_font_size())};
    auto &f_renderer_small {ekg::f_renderer(ekg::font::small)};
    auto text_dock_flags {ui->get_text_align()};
    auto bar_dock_flags {ui->get_bar_align()};
    auto bar_axis {ui->get_bar_axis()};
    auto &theme {ekg::theme()};

    /* Use text height as text width because slider does not have text to display.. */
    float text_height {f_renderer.get_text_height()};
    float text_width {f_renderer.get_text_height()};

    float dimension_offset {text_height / 2};
    float offset {ekg::find_min_offset(text_width, dimension_offset)};
    float value {ui->get_value()}, min {ui->get_value_min()}, max {ui->get_value_max()};
    float dimension_height {(text_height + dimension_offset) * static_cast<float>(ui->get_scaled_height())};

    const std::string parsed {std::to_string(value)};
    this->parsed_value = parsed.substr(0, ekg::max(parsed.find('.') + ui->get_precision() + (1 * ui->get_precision()), parsed.size()));
    if (text_dock_flags == ekg::dock::none) {
        this->parsed_value.clear();
    }

    float normalised_bar_thicnkess {static_cast<float>(theme.slider_bar_thicnkess) / 100},
            normalised_target_thickness {static_cast<float>(theme.slider_target_thickness / 100)};
    auto &layout {ekg::core->get_service_layout()};

    this->rect_text.w = text_width;
    this->rect_text.h = text_height;

    if (ekg::bitwise::contains(text_dock_flags, ekg::dock::none)) {
        text_width = 0;
    }

    if (bar_axis == ekg::axis::horizontal) {
        this->rect_bar.w = this->dimension.w - (offset  * 2);
        this->rect_bar.h = (dimension_height) * normalised_bar_thicnkess;

        this->rect_bar_value.w = this->rect_bar.w * (value - min) / (max - min);
        this->rect_bar_value.h = this->rect_bar.h;

        this->rect_target.w = this->rect_bar.h;
        this->rect_target.h = this->rect_bar.h;

        layout.set_preset_mask({offset, offset, dimension_height}, bar_axis, this->dimension.w);
    } else {

    }

    if (text_dock_flags != ekg::center && text_dock_flags != ekg::dock::none) {
        layout.insert_into_mask({&this->rect_text, text_dock_flags});
    }

    layout.insert_into_mask({&this->rect_bar, bar_dock_flags});
    layout.process_layout_mask();

    auto &layout_mask {layout.get_layout_mask()};

    this->rect_target.y = this->rect_bar.y + (this->rect_bar.h  / 2) - (this->rect_target.h / 2);
    this->rect_target.x = this->rect_bar.x + this->rect_bar_value.w - (this->rect_target.w / 2);
    this->dimension.w = ekg::min(this->dimension.w, layout_mask.w);
    this->dimension.h = ekg::min(this->dimension.h, layout_mask.h);
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
        ekg::set(this->flag.highlight, this->flag.hovered && ekg::rect_collide_vec(this->rect_bar + rect, interact));
    }

    this->flag.hovered = this->flag.hovered && this->flag.highlight;

    if (this->flag.hovered && ekg::input::wheel() && ((increase = ekg::input::pressed("slider-bar-increase")) || (descrease = ekg::input::pressed("slider-bar-decrease")))) {
        ui->set_value(ui->get_value() + (interact.w * static_cast<float>(ekg::pi)));
    } else if (this->flag.hovered && pressed && ekg::input::pressed("slider-activy")) {
        this->flag.activy = true;
        this->update_bar(interact.x, interact.y);
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
    auto &f_renderer_small {ekg::f_renderer(ekg::font::big)};
    auto bar {this->rect_bar + rect}, bar_value {this->rect_bar_value + rect};;

    ekg::draw::bind_scissor(ui->get_id());
    ekg::draw::sync_scissor_pos(rect.x, rect.y);
    ekg::draw::rect(bar, theme.slider_background);

    if (this->flag.highlight) {
        ekg::draw::rect(bar, theme.slider_highlight);
    }
    
    ekg::draw::rect(this->rect_target + rect, theme.slider_activy, ekg::drawmode::circle);
    ekg::draw::rect(bar.x, bar.y, bar_value.w, bar_value.h, theme.slider_activy);

    ekg::draw::bind_off_scissor();
    f_renderer_small.blit(this->parsed_value, 20, 80, theme.slider_string);
}