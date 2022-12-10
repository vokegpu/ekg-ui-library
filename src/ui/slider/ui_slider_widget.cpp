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
    auto text_dock_flags {ui->get_text_align()};
    auto bar_dock_flags {ui->get_bar_align()};
    auto bar_axis {ui->get_bar_axis()};
    auto &theme {ekg::theme()};
    auto value_precision {ui->get_precision()};

    float text_height {f_renderer.get_text_height()};
    float value {ui->get_value()}, min {ui->get_value_min()}, max {ui->get_value_max()};

    this->string_value = ekg::string_float_precision(max, value_precision);
    float text_width {f_renderer.get_text_width(this->string_value)}, max_text_width {text_width}; // use max val as width
    this->string_value = ekg::string_float_precision(value, value_precision);

    float dimension_offset {text_height / 2};
    float offset {ekg::find_min_offset(text_width, dimension_offset)};
    float dimension_height {(text_height + dimension_offset) * static_cast<float>(ui->get_scaled_height())};

    float normalised_bar_thicnkess {static_cast<float>(theme.slider_bar_thicnkess) / 100},
          normalised_target_thickness {static_cast<float>(theme.slider_target_thickness) / 100};
    auto &layout {ekg::core->get_service_layout()};
    bool centered_text {text_dock_flags == ekg::dock::center};

    this->dimension.w = ekg::min(this->dimension.w, text_height);
    this->dimension.h = dimension_height;

    if (centered_text) {
        auto &f_renderer_small {ekg::f_renderer(this->font_render_size)};
        text_width = f_renderer_small.get_text_width(this->string_value);
        text_height = f_renderer_small.get_text_height();
    }

    this->rect_text.w = text_width;
    this->rect_text.h = text_height;

    float bar_difference_size {};

    switch (bar_axis) {
        case ekg::axis::horizontal: {
            /* we need to subtract the bar with the text size if text align is not center (center follows the target) */
            float text_rendering_not_center {((text_width + offset * 2) * (!centered_text))};
            this->rect_bar.w = this->dimension.w - (offset  * 2) - text_rendering_not_center;
            this->rect_bar.h = dimension_height * normalised_bar_thicnkess;

            this->rect_bar_value.w = this->rect_bar.w * (value - min) / (max - min);
            this->rect_bar_value.h = this->rect_bar.h;

            // Radius are equals to both dimension (w, h)
            this->rect_target.w = dimension_height * normalised_target_thickness;
            this->rect_target.h = this->rect_target.w;

            bar_difference_size = this->rect_bar.h;
            this->rect_bar.h = ekg::min(this->rect_bar.h, this->rect_target.h);

            layout.set_preset_mask({offset, offset, dimension_height}, bar_axis, this->dimension.w);
            break;
        }

        case ekg::axis::vertical: {
            // todo axis height.
            break;
        }
    }

    this->font_render_size = ekg::font::small;
    if (!centered_text) {
        layout.insert_into_mask({&this->rect_text, text_dock_flags});
        this->font_render_size = ekg::font::normal;
    }

    layout.insert_into_mask({&this->rect_bar, bar_dock_flags});
    layout.process_layout_mask();

    auto &layout_mask {layout.get_layout_mask()};
    this->dimension.w = layout_mask.w;
    this->dimension.h = layout_mask.h;

    this->rect_bar.h = bar_difference_size;
    this->rect_target.x = this->rect_bar.x + this->rect_bar_value.w - (this->rect_target.w / 2);
    this->rect_target.y = this->rect_bar.y + (this->rect_bar.h / 2) - (this->rect_target.h / 2);

    if (centered_text && bar_axis == ekg::axis::horizontal && ekg::bitwise::contains(bar_dock_flags, ekg::dock::top | ekg::dock::bottom | ekg::dock::center)) {
        float middle_size {this->dimension.h - (this->rect_target.y + this->rect_target.h)};
        middle_size /= 2;

        this->rect_text.x = this->rect_target.x + (this->rect_target.w / 2) - (this->rect_text.w / 2);
        this->rect_text.y = ekg::bitwise::contains(bar_dock_flags, ekg::dock::top | ekg::dock::center) ? this->rect_target.y + this->rect_target.h + (middle_size - (this->rect_text.h / 2)) : offset + (middle_size - (this->rect_text.h / 2));
    
        bool text_out_dimension_left {this->rect_text.x < offset};
        this->rect_text.x = text_out_dimension_left ? offset : this->rect_text.x;

        bool text_out_dimension_right {this->rect_text.x + this->rect_text.w > this->dimension.w - offset};
        this->rect_text.x = text_out_dimension_right ? (this->dimension.w - offset - this->rect_text.w) : this->rect_text.x;
    }
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
    auto &f_renderer {ekg::f_renderer(this->font_render_size)};
    auto bar {this->rect_bar + rect}, bar_value {this->rect_bar_value + rect};;

    ekg::draw::bind_scissor(ui->get_id());
    ekg::draw::sync_scissor_pos(rect.x, rect.y);
    ekg::draw::rect(bar, theme.slider_background);

    if (this->flag.highlight) {
        ekg::draw::rect(bar, theme.slider_highlight);
    }
    
    ekg::draw::rect(this->rect_target + rect, theme.slider_activy, ekg::drawmode::circle);
    ekg::draw::rect(bar.x, bar.y, bar_value.w, bar_value.h, theme.slider_activy_bar);

    f_renderer.blit(this->string_value, rect.x + this->rect_text.x, rect.y + this->rect_text.y, theme.slider_string);
    ekg::draw::bind_off_scissor();
}