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

void ekg::ui::slider_widget::destroy() {
    abstract_widget::destroy();

    auto ui {(ekg::ui::slider*) this->data};
    auto &rect = (this->data->widget() = this->layout + *this->parent);
    auto dock {ui->get_bar_dock()};
    auto scaled_height {ui->get_scaled_height()};
    auto f_renderer {ekg::f_renderer(ui->get_font_size())};

    float size_bar {f_renderer.get_text_height()};
    float offset {size_bar / 3};
    
    ekg::set_rect_clamped(this->layout, ekg::theme().min_widget_size);

    this->layout.w = ekg::min(this->layout.w, offset * 2);
    this->layout.h = this->layout.h, (size_bar + offset) * static_cast<float>(ui->get_scaled_height());

    switch (dock) {
        case ekg::dock::top: {
            this->offset.h = this->layout.h;
            this->offset.w = (ui->get_width() / 2.0) - offset;
            this->offset.x = (this->layout.w / 2) - (this->offset.w / 2);
            this->offset.y = offset;
            break;
        }

        case ekg::dock::bottom: {
            this->offset.h = this->layout.h;
            this->offset.w = (this->layout.w / 2.0) - offset;
            this->offset.x = (this->layout.w / 2) - (this->offset.w / 2);
            this->offset.y = offset;
            break;
        }

        case ekg::dock::right: {
            this->offset.w = rect.w;
            this->offset.h = (this->layout.w / 2.0) - offset;
            this->offset.x = offset;
            this->offset.y = (this->layout.h / 2) - (this->offset.h / 2);
            break;
        }

        default: {
            this->offset.w = rect.w;
            this->offset.h = (this->layout.w / 2.0) - offset;
            this->offset.x = offset;
            this->offset.y = (this->layout.h / 2) - (this->offset.h / 2);
            break;
        }
    }
}

void ekg::ui::slider_widget::on_reload() {
    abstract_widget::on_reload();
}

void ekg::ui::slider_widget::on_pre_event(SDL_Event &sdl_event) {
    abstract_widget::on_pre_event(sdl_event);
}

void ekg::ui::slider_widget::on_event(SDL_Event &sdl_event) {
    abstract_widget::on_event(sdl_event);
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
    auto &rect = (this->data->widget() = this->layout + *this->parent);
    auto &theme {ekg::theme()};
    auto &f_renderer {ekg::f_renderer(ui->get_font_size())};

    ekg::draw::rect(this->offset + rect, theme.slider_background);

    if (this->flag.highlight) {
        ekg::draw::rect(this->offset + rect, theme.slider_highlight);
    }
    
    ekg::draw::rect(this->extra + rect, theme.slider_activy);
}