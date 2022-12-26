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

#include "ekg/ui/popup/ui_popup_widget.hpp"
#include "ekg/ui/popup/ui_popup.hpp"
#include "ekg/ekg.hpp"

void ekg::ui::popup_widget::destroy() {

}

void ekg::ui::popup_widget::on_reload() {
	auto ui {(ekg::ui::popup*) this->data};
	auto &theme {ekg::theme()};
	auto &f_renderer {ekg::f_renderer(ui->get_font_size())};
    auto &rect {this->get_abs_rect()};

    float max_width {};
    float text_width {};

    for (ekg::component &components : ui->get_component_list()) {
        text_width = f_renderer.get_text_width(components.name);
        if (text_width > max_width) {
            max_width = text_width;
        }
    }

    float text_height {f_renderer.get_text_height()};
    float dimension_offset {text_height / 2};
    float offset {ekg::find_min_offset(text_width, dimension_offset)};

    this->dimension.w = ekg::min(this->dimension.w, max_width);

    ekg::ui::popup_widget::element element {};
    for (ekg::component &components : ui->get_component_list()) {
        text_width = f_renderer.get_text_width(components.name);
    }
}

void ekg::ui::popup_widget::on_pre_event(SDL_Event &sdl_event) {

}

void ekg::ui::popup_widget::on_event(SDL_Event &sdl_event) {

}

void ekg::ui::popup_widget::on_post_event(SDL_Event &sdl_event) {
}

void ekg::ui::popup_widget::on_update() {

}

void ekg::ui::popup_widget::on_draw_refresh() {
}