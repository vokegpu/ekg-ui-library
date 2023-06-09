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

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include "ekg/util/util_event.hpp"
#include "ekg/draw/draw.hpp"
#include "ekg/os/info.hpp"

ekg::ui::abstract_widget::abstract_widget() {
    this->parent = &this->empty_parent;
    this->scroll = &this->empty_scroll;
}

ekg::ui::abstract_widget::~abstract_widget() {
    this->on_destroy();
}

void ekg::ui::abstract_widget::on_destroy() {

}

void ekg::ui::abstract_widget::on_reload() {
    ekg::dispatch(ekg::env::redraw);
}

void ekg::ui::abstract_widget::on_pre_event(SDL_Event &sdl_event) {
    if (ekg::input::pressed() || ekg::input::released() || ekg::input::motion() || ekg::input::wheel()) {
        auto &interact {ekg::interact()};
        auto &rect {this->get_abs_rect()};
        this->flag.hovered = ekg::rect_collide_vec(rect, interact) && (this->data->get_type() == ekg::type::popup || this->data->get_parent_id() == 0 || ekg::draw::is_visible(this->data->get_id(), interact));
    }
}

void ekg::ui::abstract_widget::on_event(SDL_Event &sdl_event) {

}

void ekg::ui::abstract_widget::on_post_event(SDL_Event &sdl_event) {
    this->flag.hovered = false;
    this->flag.highlight = !(!this->flag.hovered && (ekg::os == ekg::platform::os_android && ekg::input::released())) && this->flag.highlight;
}

void ekg::ui::abstract_widget::on_update() {

}

void ekg::ui::abstract_widget::on_draw_refresh() {

}

ekg::rect ekg::ui::abstract_widget::get_static_rect() {
    return this->dimension + *this->parent;
}

ekg::rect &ekg::ui::abstract_widget::get_abs_rect() {
    return (this->data->widget() = this->dimension + *this->parent + *this->scroll);
}
