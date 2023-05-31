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
#include "ekg/util/util_event.hpp"
#include "ekg/draw/draw.hpp"
#include "ekg/ekg.hpp"

void ekg::ui::scroll_widget::on_destroy() {
}

void ekg::ui::scroll_widget::on_reload() {
    this->scroll.rect_mother = this->parent;
    this->scroll.mother_id = this->data->get_parent_id();
    this->scroll.widget_id = this->data->get_id();
    this->scroll.on_reload();
}

void ekg::ui::scroll_widget::on_pre_event(SDL_Event &sdl_event) {
    this->scroll.on_pre_event(sdl_event);
    this->flag.hovered = this->scroll.flag.hovered;
    this->flag.absolute = this->scroll.is_dragging_bar() || this->flag.hovered || this->scroll.flag.activy;
}

void ekg::ui::scroll_widget::on_event(SDL_Event &sdl_event) {
    this->scroll.on_event(sdl_event);

    if ((this->flag.hovered || this->flag.absolute || this->scroll.flag.activy) && !this->is_high_frequency) {
        ekg::update_high_frequency(this);
        this->is_high_frequency = true;
    }

    if (this->scroll.flag.state || this->scroll.flag.extra_state || this->flag.absolute) {
        ekg::scissor(this->data->get_parent_id());
        ekg::dispatch(ekg::env::redraw);
    }
}

void ekg::ui::scroll_widget::on_post_event(SDL_Event &sdl_event) {
    ekg::ui::abstract_widget::on_post_event(sdl_event);
    this->scroll.flag.hovered = false;
    this->scroll.flag.activy = false;
}

void ekg::ui::scroll_widget::on_update() {
    this->scroll.on_update();
    ekg::scissor(this->data->get_parent_id());
    this->is_high_frequency = this->scroll.check_activy_state(this->flag.hovered || this->flag.absolute || this->scroll.flag.activy);
}

void ekg::ui::scroll_widget::on_draw_refresh() {
    this->dimension.w = this->parent->w;
    this->dimension.h = this->parent->h;

    ekg::draw::bind_scissor(this->data->get_id());
    this->scroll.on_draw_refresh();
    ekg::draw::bind_off_scissor();
}