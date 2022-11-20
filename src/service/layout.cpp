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
#include "ekg/service/layout.hpp"
#include "ekg/ui/frame/ui_frame.hpp"
#include "ekg/ekg.hpp"
#include "ekg/ui/frame/ui_frame_widget.hpp"

void ekg::service::layout::process(ekg::ui::abstract_widget *widget) {

}

void ekg::service::layout::process_scaled(ekg::ui::abstract_widget* widget_parent) {
    auto type {widget_parent->data->get_type()};
    auto is_group {type == ekg::type::frame};
    auto &parent_rect = widget_parent->data->widget();

    if (!is_group || parent_rect.w == 0 || parent_rect.h == 0) {
        return;
    }

    auto &grid {this->grid_map[widget_parent->data->get_id()]};
    ekg::ui::abstract_widget* widgets {};
    float group_top_offset {this->min_offset};

    switch (type) {
        case ekg::type::frame: {
            auto widget_group = (ekg::ui::frame_widget*) widget_parent;
            auto ui_group = (ekg::ui::frame*) widget_parent->data;

            if (ekg::bitwise::contains(ui_group->get_drag_dock(), ekg::dock::top)) {
                group_top_offset = widget_group->docker_activy_drag.top.h;
            }

            break;
        }
    }

    this->scaled_width_divided = parent_rect.w / 3;
    this->scaled_width_divided = parent_rect.h / 2;
    this->enum_docks_flag = 0;

    this->curr_top = {0, group_top_offset, 0, 0};
    this->curr_bottom = {0, parent_rect.h - this->min_offset, 0, 0};
    this->prev_flag_top = {};
    this->prev_flag_bottom = {};
    this->curr_flag = {};
    this->curr_grid = {
            // top left | top center | top right
            {0, 0, 0, 0},
            {0, 0, this->scaled_width_divided, this->scaled_height_divided},
            {this->scaled_width_divided, 0, this->scaled_width_divided, this->scaled_height_divided},
            {parent_rect.w - this->scaled_width_divided, 0, this->scaled_width_divided, this->scaled_height_divided},

            // bottom left | bottom center | bottom right
            {0, parent_rect.h - this->scaled_height_divided, 0, 0},
            {0, parent_rect.h - this->scaled_height_divided, this->scaled_width_divided, this->scaled_height_divided},
            {this->scaled_width_divided, parent_rect.h - this->scaled_height_divided, this->scaled_width_divided, this->scaled_height_divided},
            {parent_rect.w - this->scaled_width_divided, parent_rect.h - this->scaled_height_divided, this->scaled_width_divided, this->scaled_height_divided}
    };

    ekg::rect prev_rect {};

    for (int32_t &ids : widget_parent->data->get_parent_id_list()) {
        widgets = ekg::core->get_fast_widget_by_id(ids);

        if (widgets == nullptr) {
            continue;
        }

        auto &rect {widgets->data->widget()};
        auto &layout {widgets->dimension};

        layout.w = rect.w;
        layout.h = rect.h;
        enum_docks_flag = widgets->data->get_dock();

        this->curr_flag.top    = ekg::bitwise::contains(enum_docks_flag, ekg::dock::top);
        this->curr_flag.bottom = ekg::bitwise::contains(enum_docks_flag, ekg::dock::bottom);
        this->curr_flag.left   = ekg::bitwise::contains(enum_docks_flag, ekg::dock::left);
        this->curr_flag.right  = ekg::bitwise::contains(enum_docks_flag, ekg::dock::right);
        this->curr_flag.center = ekg::bitwise::contains(enum_docks_flag, ekg::dock::center);
        this->curr_flag.full   = ekg::bitwise::contains(enum_docks_flag, ekg::dock::full);
        this->curr_flag.free   = ekg::bitwise::contains(enum_docks_flag, ekg::dock::free);
        this->curr_flag.none   = ekg::bitwise::contains(enum_docks_flag, ekg::dock::none);
        this->curr_flag.next   = ekg::bitwise::contains(enum_docks_flag, ekg::dock::next);

        if (this->curr_flag.top) {
            if (this->curr_flag.left) {
                if (this->prev_flag_bottom.bottom || this->curr_flag.next) {
                    this->curr_top.x = 0;
                    this->curr_top.y += prev_rect.h + this->min_offset;
                }

                this->curr_top.x += this->min_offset;
                layout.x = this->curr_top.x;
                layout.y = this->curr_top.y;
                this->curr_top.x += layout.w;
                this->prev_flag_top.left = true;

                if (this->curr_top.w > this->curr_grid.top_left.w) {
                    this->curr_grid.top_left.w = this->curr_top.w;
                }

                if (this->curr_top.h > this->curr_grid.top_left.h) {
                    this->curr_grid.top_left.h = this->curr_top.h;
                }
            }

            this->prev_flag_top.top = true;
            this->prev_flag_bottom.bottom = false;
            this->prev_flag_top.next = this->curr_flag.next;
        } else if (this->curr_flag.bottom) {
            this->prev_flag_top.top = false;
            this->prev_flag_bottom.bottom = true;
        }

        prev_rect = rect;
    }
}

void ekg::service::layout::init() {
    this->min_factor_height = 1;
    this->min_height = ekg::core->get_f_renderer_normal().get_text_height();
    this->min_offset = this->min_height / 3;
    this->min_height += this->min_offset;
    this->min_offset /= 2;
}

void ekg::service::layout::quit() {

}
