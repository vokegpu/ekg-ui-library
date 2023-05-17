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
#include "ekg/draw/draw.hpp"

void ekg::ui::scroll_embedded_widget::calculate_rect_bar_sizes() {
    this->rect_vertical_scroll_bar.h = this->rect_mother->h;
    float offset_vertical {this->rect_child.h - this->rect_mother->h};
    this->rect_vertical_scroll_bar.h = offset_vertical;
}

ekg::ui::scroll_embedded_widget::scroll_embedded_widget() {
    this->rect_mother = &this->rect_child;
}

void ekg::ui::scroll_embedded_widget::on_destroy() {

}
 
void ekg::ui::scroll_embedded_widget::on_reload() {
    if (this->mother_id == 0 || this->child_id_list.empty()) {
        this->calculate_rect_bar_sizes();
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

    this->acceleration.y = 99999.0f;
    for (int32_t &ids : this->child_id_list) {
        if ((widgets = ekg::core->get_fast_widget_by_id(ids)) == nullptr) {
            continue;
        }

        if (widgets->dimension.w > this->rect_child.w) {
            this->rect_child.w = widgets->dimension.w;
        }

        this->rect_child.h += widgets->dimension.h + service_layout_min_offset;
        if (widgets->dimension.h < this->acceleration.y) {
            this->acceleration.y = widgets->dimension.h;
        }
    }

    this->acceleration.y += service_layout_min_offset;
    this->calculate_rect_bar_sizes();
}

void ekg::ui::scroll_embedded_widget::on_pre_event(SDL_Event &sdl_event) {
    if (ekg::input::pressed() || ekg::input::released() || ekg::input::motion() || ekg::input::wheel()) {
        auto &interact {ekg::interact()};
        this->flag.hovered = ekg::rect_collide_vec(this->rect_child, interact) && (ekg::rect_collide_vec(*this->rect_mother, interact));
    }
}

void ekg::ui::scroll_embedded_widget::on_event(SDL_Event &sdl_event) {
    this->is_vertical_enabled = this->rect_child.h > this->rect_mother->h;
    this->is_horizontal_enabled = this->rect_child.w > this->rect_mother->w;

    if (this->flag.focused && this->flag.hovered && ekg::input::wheel() && (this->is_vertical_enabled || this->is_horizontal_enabled)) {
        auto &interact {ekg::interact()};
        if (this->is_vertical_enabled) {
            this->scroll.w = this->scroll.y + (interact.w * (this->acceleration.y * 1.0000000000054835f));
        }
    }
}

void ekg::ui::scroll_embedded_widget::on_post_event(SDL_Event &sdl_event) {
}

void ekg::ui::scroll_embedded_widget::on_update() {
    ekg::vec2 vertical_scroll_limit {0.0f, this->rect_child.h - this->rect_mother->h};
    ekg::vec2 horizontal_scroll_limit {0.0f, this->rect_child.w - this->rect_mother->w};

    this->scroll.x = ekg::lerp(this->scroll.x, this->scroll.z, ekg::display::dt + ekg::scrollsmooth);
    this->scroll.y = ekg::lerp(this->scroll.y, this->scroll.w, ekg::display::dt + ekg::scrollsmooth);

    if (this->rect_child.h < this->rect_mother->h) {
        this->scroll.y = 0.0f;
    } else if (this->scroll.y < -vertical_scroll_limit.y) {
        this->scroll.y = -vertical_scroll_limit.y;
        this->scroll.w = this->scroll.y;
    } else if (this->scroll.y > vertical_scroll_limit.x) {
        this->scroll.y = vertical_scroll_limit.x;
        this->scroll.w = this->scroll.y;
    }

    ekg::dispatch(ekg::env::redraw);
}

void ekg::ui::scroll_embedded_widget::on_draw_refresh() {
    auto &theme {ekg::theme()};

    this->rect_vertical_scroll_bar.x = this->rect_mother->x + this->rect_mother->w - static_cast<float>(theme.scrollbar_pixel_thickness);
    this->rect_vertical_scroll_bar.w = static_cast<float>(theme.scrollbar_pixel_thickness);

    float out_of_mother_height {(this->rect_child.h - this->rect_mother->h)};
    if (out_of_mother_height >= this->rect_mother->h - 50.0f) {
        out_of_mother_height = (out_of_mother_height);
    }

    this->rect_vertical_scroll_bar.h = this->rect_mother->h - out_of_mother_height;
    this->rect_vertical_scroll_bar.y = abs(this->scroll.y);

    ekg::draw::rect(this->rect_vertical_scroll_bar.x,
                    this->rect_mother->y + this->rect_vertical_scroll_bar.y,
                    this->rect_vertical_scroll_bar.w, this->rect_vertical_scroll_bar.h, theme.scrollbar_background);
}