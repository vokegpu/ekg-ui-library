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

#include "ekg/ui/scroll/ui_scroll_embedded_widget.hpp"
#include "ekg/ekg.hpp"
#include "ekg/ui/frame/ui_frame_widget.hpp"

ekg::ui::scroll_embedded_widget() {
    this->rect_mother = &this->rect_child;
}

void ekg::ui::scroll_embedded_widget::on_destroy() {

}
 
void ekg::ui::scroll_embedded_widget::on_reload() {
    if (this->mother_id == 0 || this->child_id_list.empty()) {
        return;
    }

    this->rect_child.x = 0;
    this->rect_child.y = 0;

    ekg::ui::abstract_widget *mother_widget {ekg::core->get_fast_widget_by_id(this->mother_id)};
    ekg::ui::abstract_widget *widgets {};
    float service_layout_min_offset {ekg::core->get_service_layout().get_min_offset()};

    switch (mother_widget->data->get_type()) {
    case ekg::type::frame:
        ekg::ui::frame_widget *frame {(ekg::ui::frame_widget*) mother_widget};
        this->rect_child.h += frame->docker_activy_drag.top.h;
        break;
    }

    for (int32_t &ids : this->child_id_list) {
        if ((widgets = ekg::core->get_fast_widget_by_id(ids)) == nullptr) {
            continue;
        }

        if (widgets->dimension.w > this->rect_child.w) {
            this->rect_child.w = widgets->dimension.w;
        }

        this->rect_child.h += widgets->dimension.w + service_layout_min_offset;
    }
}

void ekg::ui::scroll_embedded_widget::on_pre_event(SDL_Event &sdl_event) {
    if (ekg::input::pressed() || ekg::input::released() || ekg::input::motion() || ekg::input::wheel()) {
        auto &interact {ekg::interact()};
        auto &rect {this->get_abs_rect()};
        this->flag.hovered = ekg::rect_collide_vec(rect, interact) && (ekg::rect_collide_vec(*this->parent, interact));
    }
}

void ekg::ui::scroll_embedded_widget::on_event(SDL_Event &sdl_event) {
    this->is_vertical_enabled = this->rect_child.h > this->rect_mother.h;
    this->is_horizontal_enabled = this->rect_child.w > this->rect_mother.w;

    if (!this->flag.hovered) {
        return;
    }

    if (this->is_vertical_enabled) {

    }
}

void ekg::ui::scroll_embedded_widget::on_post_event(SDL_Event &sdl_event) {

}

void ekg::ui::scroll_embedded_widget::on_update() {

}

void ekg::ui::scroll_embedded_widget::on_draw_refresh() {

}