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

void ekg::ui::scroll_embedded_widget::check_axis_states() {
    this->is_vertical_enabled = this->rect_child.h > this->rect_mother->h;
    this->is_horizontal_enabled = this->rect_child.w > this->rect_mother->w;
}

void ekg::ui::scroll_embedded_widget::reset_scroll() {
    this->scroll.x = this->scroll.z;
    this->scroll.y = this->scroll.y;
}

bool ekg::ui::scroll_embedded_widget::check_activy_state(bool state) {
    state = state || (static_cast<int32_t>(this->scroll.x) != static_cast<int32_t>(this->scroll.z)) || (static_cast<int32_t>(this->scroll.y) != static_cast<int32_t>(this->scroll.w));
    if (!state) {
        this->scroll.x = this->scroll.z;
        this->scroll.y = this->scroll.w;
    }

    return state;
}

bool ekg::ui::scroll_embedded_widget::is_dragging_bar() {
    return this->flag.state || this->flag.extra_state;
}

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
    if (this->mother_id == 0 && this->child_id_list.empty()) {
        this->calculate_rect_bar_sizes();
        this->check_axis_states();
        return;
    }

    ekg::ui::abstract_widget *mother_widget {ekg::core->get_fast_widget_by_id(this->mother_id)};
    ekg::ui::abstract_widget *widgets {};
    float service_layout_min_offset {ekg::core->get_service_layout().get_min_offset()};

    this->child_id_list = mother_widget->data->get_child_id_list();

    switch (mother_widget->data->get_type()) {
    case ekg::type::frame:
        ekg::ui::frame_widget *frame {(ekg::ui::frame_widget*) mother_widget};
        frame->p_scroll_embedded = this;
        break;
    }

    this->rect_child.x = 0.0f;
    this->rect_child.y = 0.0f;
    this->rect_child.w = 0.0f;
    this->rect_child.h = 0.0f;

    float text_height {ekg::f_renderer(ekg::font::normal).get_text_height()};
    this->acceleration.y = text_height + (text_height / 2.0f);

    for (int32_t &ids : this->child_id_list) {
        if ((widgets = ekg::core->get_fast_widget_by_id(ids)) == nullptr || widgets->data->get_type() == ekg::type::scroll) {
            continue;
        }

        if (widgets->dimension.w > this->rect_child.w) {
            this->rect_child.w = widgets->dimension.w;
        }

        if (widgets->dimension.y > this->rect_child.h) {
            this->rect_child.h = widgets->dimension.y + widgets->dimension.h;
        }

        if (widgets->dimension.h < this->acceleration.y) {
            this->acceleration.y = widgets->dimension.h;
        }

        widgets->scroll = &this->scroll;
    }

    this->acceleration.y += service_layout_min_offset;
    this->calculate_rect_bar_sizes();
    this->check_axis_states();
}

void ekg::ui::scroll_embedded_widget::on_pre_event(SDL_Event &sdl_event) {
    if (ekg::input::pressed() || ekg::input::released() || ekg::input::motion() || ekg::input::wheel()) {
        ekg::rect scaled_vertical_bar {this->rect_vertical_scroll_bar};
        scaled_vertical_bar.y += this->rect_mother->y;

        ekg::rect scaled_horizontal_bar {this->rect_horizontal_scroll_bar};
        scaled_horizontal_bar.x += this->rect_mother->x;

        ekg::vec4 &interact {ekg::interact()};
        bool visible {ekg::draw::is_visible(this->widget_id, interact)};

        this->flag.activy = visible && this->is_vertical_enabled && ekg::input::pressed("scrollbar-scroll");
        this->flag.hovered = this->flag.activy || (visible && (ekg::rect_collide_vec(scaled_vertical_bar, interact) || ekg::rect_collide_vec(scaled_horizontal_bar, interact)));
    }
}

void ekg::ui::scroll_embedded_widget::on_event(SDL_Event &sdl_event) {
    this->check_axis_states();

    bool vertical_hovered {};
    bool horizontal_hovered {};
    auto &interact {ekg::interact()};

    if (this->flag.hovered && ekg::input::pressed("scrollbar-scroll") && this->is_vertical_enabled) {
        this->scroll.w = ekg::clamp(this->scroll.y + (interact.w * this->acceleration.y), this->rect_mother->h - this->rect_child.h, 0.0f);
    }

    if (this->flag.hovered && ekg::input::pressed() && ekg::input::pressed("scrollbar-drag")) {
        ekg::rect scaled_bar {this->rect_vertical_scroll_bar};
        scaled_bar.y += this->rect_mother->y;
        this->flag.state = ekg::rect_collide_vec(scaled_bar, interact);
        this->bar_drag.x = interact.y - scaled_bar.y;
    
        scaled_bar = this->rect_horizontal_scroll_bar;
        scaled_bar.x += this->rect_mother->x;

        this->flag.extra_state = ekg::rect_collide_vec(scaled_bar, interact);
        this->bar_drag.y = interact.x - scaled_bar.x;
    }

    if (ekg::input::motion() && (this->flag.state || this->flag.extra_state)) {
        if (this->flag.state) {
            ekg::rect scaled_bar {this->rect_vertical_scroll_bar};
            scaled_bar.y = interact.y - this->bar_drag.x;
            scaled_bar.y -= this->rect_mother->y;

            this->scroll.w = -ekg::clamp(scaled_bar.y / (this->rect_mother->h - this->rect_vertical_scroll_bar.h), 0.0f, 1.0f) * (this->rect_child.h - this->rect_mother->h);
            this->scroll.y = this->scroll.w;
        }

        if (this->flag.extra_state) {
            ekg::rect scaled_bar {this->rect_horizontal_scroll_bar};
            scaled_bar.x = interact.x - this->bar_drag.y;
            scaled_bar.x -= this->rect_mother->x;

            this->scroll.z = -ekg::clamp(scaled_bar.x / (this->rect_mother->w - this->rect_horizontal_scroll_bar.w), 0.0f, 1.0f) * (this->rect_child.w - this->rect_mother->w);
            this->scroll.x = this->scroll.z;
        }
    }

    if (ekg::input::released()) {
        this->flag.state = false;
        this->flag.extra_state = false;
    }
}

void ekg::ui::scroll_embedded_widget::on_post_event(SDL_Event &sdl_event) {

}

void ekg::ui::scroll_embedded_widget::on_update() {
    this->scroll.x = ekg::lerp(this->scroll.x, this->scroll.z, ekg::scrollsmooth);
    this->scroll.y = ekg::lerp(this->scroll.y, this->scroll.w, ekg::scrollsmooth);

    ekg::dispatch(ekg::env::redraw);
}

void ekg::ui::scroll_embedded_widget::clamp_scroll() {
    ekg::vec2 vertical_scroll_limit {0.0f, this->rect_mother->h - this->rect_child.h};
    ekg::vec2 horizontal_scroll_limit {0.0f, this->rect_mother->w - this->rect_child.w};

    if (this->rect_child.h < this->rect_mother->h) {
        this->scroll.y = 0.0f;
    } else if (this->scroll.y < vertical_scroll_limit.y) {
        this->scroll.y = vertical_scroll_limit.y;
        this->scroll.w = this->scroll.y;
    } else if (this->scroll.y > vertical_scroll_limit.x) {
        this->scroll.y = vertical_scroll_limit.x;
        this->scroll.w = this->scroll.y;
    }

    if (this->rect_child.w < this->rect_mother->w) {
        this->scroll.x = 0.0f;
    } else if (this->scroll.x < horizontal_scroll_limit.y) {
        this->scroll.x = horizontal_scroll_limit.y;
        this->scroll.z = this->scroll.x;
    } else if (this->scroll.x > horizontal_scroll_limit.x) {
        this->scroll.x = horizontal_scroll_limit.x;
        this->scroll.z = this->scroll.x;
    }
}

void ekg::ui::scroll_embedded_widget::on_draw_refresh() {
    auto &theme {ekg::theme()};

    this->rect_vertical_scroll_bar.x = this->rect_mother->x + this->rect_mother->w - static_cast<float>(theme.scrollbar_pixel_thickness);
    this->rect_vertical_scroll_bar.w = static_cast<float>(theme.scrollbar_pixel_thickness);

    float out_of_mother_height {(this->rect_child.h - this->rect_mother->h)};
    float y_pos_factor {abs(this->scroll.y) / out_of_mother_height};

    this->rect_vertical_scroll_bar.h = this->rect_mother->h - (out_of_mother_height <= 0.0f ? this->rect_mother->h : ekg::max(out_of_mother_height, this->rect_mother->h - 30.0f));
    this->rect_vertical_scroll_bar.y = y_pos_factor * (this->rect_mother->h - this->rect_vertical_scroll_bar.h);

    ekg::draw::rect(this->rect_vertical_scroll_bar.x,
                    this->rect_mother->y + this->rect_vertical_scroll_bar.y,
                    this->rect_vertical_scroll_bar.w, this->rect_vertical_scroll_bar.h, theme.scrollbar_background);

    this->rect_horizontal_scroll_bar.x = this->rect_mother->x + this->rect_mother->w - static_cast<float>(theme.scrollbar_pixel_thickness);
    this->rect_horizontal_scroll_bar.w = static_cast<float>(theme.scrollbar_pixel_thickness);

    this->rect_horizontal_scroll_bar.y = this->rect_mother->y + this->rect_mother->h - static_cast<float>(theme.scrollbar_pixel_thickness);
    this->rect_horizontal_scroll_bar.h = static_cast<float>(theme.scrollbar_pixel_thickness);

    float out_of_mother_width {(this->rect_child.w - this->rect_mother->w)};
    float x_pos_factor {abs(this->scroll.x) / out_of_mother_width};

    this->rect_horizontal_scroll_bar.w = this->rect_mother->w - (out_of_mother_width <= 0.0f ? this->rect_mother->w : ekg::max(out_of_mother_width, this->rect_mother->w - 30.0f));
    this->rect_horizontal_scroll_bar.x = x_pos_factor * (this->rect_mother->w - this->rect_horizontal_scroll_bar.w);

    ekg::draw::rect(this->rect_mother->x + this->rect_horizontal_scroll_bar.x,
                    this->rect_horizontal_scroll_bar.y,
                    this->rect_horizontal_scroll_bar.w, this->rect_horizontal_scroll_bar.h, theme.scrollbar_background);
}