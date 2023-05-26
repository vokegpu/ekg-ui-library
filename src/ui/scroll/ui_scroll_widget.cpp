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

#include "ekg/ui/scroll/ui_scroll_widget.hpp"
#include "ekg/util/util_ui.hpp"

void ekg::ui::scroll_widget::on_destroy() {
}

void ekg::ui::scroll_widget::on_reload() {
    this->scroll.rect_mother = this->parent;
    this->scroll.mother_id = this->data->get_parent_id();
    this->scroll.on_reload();
}

void ekg::ui::scroll_widget::on_pre_event(SDL_Event &sdl_event) {
    this->scroll.on_pre_event(sdl_event);
    this->flag = this->scroll.flag;
}

void ekg::ui::scroll_widget::on_event(SDL_Event &sdl_event) {
    if ((this->flag.hovered || this->flag.absolute) && !this->is_high_frequency) {
        ekg::update_high_frequency(this);
    }

    this->scroll.on_event(sdl_event);
}

void ekg::ui::scroll_widget::on_post_event(SDL_Event &sdl_event) {

}

void ekg::ui::scroll_widget::on_update() {
    this->scroll.on_update();
    ekg::scissor(this);
    this->is_high_frequency = this->flag.hovered || this->flag.absolute;
}

void ekg::ui::scroll_widget::on_draw_refresh() {
    this->scroll.clamp_scroll();
    this->scroll.on_draw_refresh();
}