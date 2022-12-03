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

void ekg::service::layout::set_preset_mask(const ekg::vec3 &offset, ekg::dock axis, float initial_respective_size) {
    this->dock_axis_mask = axis;    
    this->offset_mask = offset;
    this->respective_mask_all = initial_respective_size;
}

void ekg::service::layout::insert_into_mask(const ekg::dockrect &dockrect) {
    this->dockrect_list.push_back(dockrect);
}

void ekg::service::layout::process_layout_mask() {
    if (this->dockrect_list.empty()) {
        return;
    }

    bool axis {this->dock_axis_mask == ekg::dock::left || this->dock_axis_mask == ekg::dock::right};
    float v {this->respective_mask_all}, centered_dimension {this->offset_mask.z / 2}, opposite {}, uniform {};

    /* offset z is the dimension respective (width if height else height) size */
    this->layout_mask.w = axis ? this->offset_mask.x : this->offset_mask.z;
    this->layout_mask.h = axis ? this->offset_mask.z : this->offset_mask.y;

    /* check for opposite dock and get the full size respective for the axis dock */
    if (v == 0) {
        v = this->get_respective_mask_size();
    }

    /* axis false is equals X else is equals Y */
    for (ekg::dockrect &dockrect : this->dockrect_list) {
        if (dockrect.rect == nullptr) {
            continue;
        }

        if (axis) {
            if (ekg::bitwise::contains(dockrect.dock, ekg::dock::center)) {
                dockrect.rect->y = centered_dimension - (dockrect.rect->h / 2);
            }
            
            if (dockrect.dock == ekg::dock::center) {
                dockrect.rect->x = (this->respective_mask_center / 2) - (dockrect.rect->w / 2);
                dockrect.rect->y = centered_dimension - (dockrect.rect->h / 2);
            }

            /* when there is a opposite dock, layout should fix the dock position to actual position */
            if (ekg::bitwise::contains(dockrect.dock, ekg::dock::left)) {
                if (static_cast<int32_t>(opposite) != 0) {
                    this->layout_mask.w -= opposite;
                } 

                dockrect.rect->x = this->layout_mask.w;
                this->layout_mask.w += dockrect.rect->w + this->offset_mask.x + opposite;
                opposite = 0;
                uniform = dockrect.rect->w + this->offset_mask.x;
            }

            if (ekg::bitwise::contains(dockrect.dock, ekg::dock::right)) {
                if (static_cast<int32_t>(uniform) != 0) {
                    this->layout_mask.w -= uniform;
                } 

                this->layout_mask.w += dockrect.rect->w;
                dockrect.rect->x = v - this->layout_mask.w;
                this->layout_mask.w += uniform + this->offset_mask.x;
                opposite = dockrect.rect->w + this->offset_mask.x;
                uniform = 0;
            }

            if (ekg::bitwise::contains(dockrect.dock, ekg::dock::top)) {
                dockrect.rect->y = this->offset_mask.y;
            }

            if (ekg::bitwise::contains(dockrect.dock, ekg::dock::bottom)) {
                dockrect.rect->y = this->offset_mask.z - this->offset_mask.y - dockrect.rect->h;
            }
        } else {
            if (ekg::bitwise::contains(dockrect.dock, ekg::dock::center)) {
                dockrect.rect->x = centered_dimension - (dockrect.rect->w / 2);
            }

            if (ekg::bitwise::contains(dockrect.dock, ekg::dock::top)) {
                dockrect.rect->y = this->layout_mask.h;
                this->layout_mask.h += dockrect.rect->h + this->offset_mask.y;
            }

            if (ekg::bitwise::contains(dockrect.dock, ekg::dock::bottom)) {
                this->layout_mask.h += dockrect.rect->h;
                dockrect.rect->y = v - this->layout_mask.h;
                this->layout_mask.h += + this->offset_mask.y;
            }

            if (ekg::bitwise::contains(dockrect.dock, ekg::dock::left)) {
                dockrect.rect->x = this->offset_mask.x;
            }

            if (ekg::bitwise::contains(dockrect.dock, ekg::dock::bottom)) {
                dockrect.rect->x = this->offset_mask.z - this->offset_mask.x - dockrect.rect->w;
            }
        }
    }

    this->dockrect_list.clear();
}

ekg::rect &ekg::service::layout::get_layout_mask() {
    return this->layout_mask;
}

float ekg::service::layout::get_respective_mask_size() {
    if (this->dockrect_list.empty()) {
        return 0;
    }

    float respective_size {(this->dock_axis_mask == ekg::dock::left  || this->dock_axis_mask == ekg::dock::right) ? this->offset_mask.x : this->offset_mask.y}, respective_center_size {}, size {};
    int32_t only_center_count {};

    for (ekg::dockrect &dockrect : this->dockrect_list) {
        if (dockrect.rect == nullptr) {
            continue;
        }

        size = (this->dock_axis_mask == ekg::dock::left  || this->dock_axis_mask == ekg::dock::right) ? (dockrect.rect->w + this->offset_mask.x) : (dockrect.rect->h + this->offset_mask.y);
        if (dockrect.dock == ekg::dock::center) {
            respective_center_size += size;
            only_center_count++;
        }

        respective_size +=size;
    }

    this->respective_mask_center = (only_center_count != 0 ?  respective_center_size  / only_center_count : 0);
    this->respective_mask_all = ekg::min(this->respective_mask_all, respective_size);

    return this->respective_mask_all;
}

void ekg::service::layout::process(ekg::ui::abstract_widget *pwidget) {

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
    this->prev_docking_top = {};
    this->prev_docking_bottom = {};
    this->curr_docking = {};
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

        this->curr_docking.top    = ekg::bitwise::contains(enum_docks_flag, ekg::dock::top);
        this->curr_docking.bottom = ekg::bitwise::contains(enum_docks_flag, ekg::dock::bottom);
        this->curr_docking.left   = ekg::bitwise::contains(enum_docks_flag, ekg::dock::left);
        this->curr_docking.right  = ekg::bitwise::contains(enum_docks_flag, ekg::dock::right);
        this->curr_docking.center = ekg::bitwise::contains(enum_docks_flag, ekg::dock::center);
        this->curr_docking.full   = ekg::bitwise::contains(enum_docks_flag, ekg::dock::full);
        this->curr_docking.free   = ekg::bitwise::contains(enum_docks_flag, ekg::dock::free);
        this->curr_docking.none   = ekg::bitwise::contains(enum_docks_flag, ekg::dock::none);
        this->curr_docking.next   = ekg::bitwise::contains(enum_docks_flag, ekg::dock::next);

        if (this->curr_docking.top) {
            if (this->curr_docking.left) {
                if (this->prev_docking_bottom.bottom || this->curr_docking.next) {
                    this->curr_top.x = 0;
                    this->curr_top.y += prev_rect.h + this->min_offset;
                }

                this->curr_top.x += this->min_offset;
                layout.x = this->curr_top.x;
                layout.y = this->curr_top.y;
                this->curr_top.x += layout.w;
                this->prev_docking_top.left = true;

                if (this->curr_top.w > this->curr_grid.top_left.w) {
                    this->curr_grid.top_left.w = this->curr_top.w;
                }

                if (this->curr_top.h > this->curr_grid.top_left.h) {
                    this->curr_grid.top_left.h = this->curr_top.h;
                }
            }

            this->prev_docking_top.top = true;
            this->prev_docking_bottom.bottom = false;
            this->prev_docking_top.next = this->curr_docking.next;
        } else if (this->curr_docking.bottom) {
            this->prev_docking_top.top = false;
            this->prev_docking_bottom.bottom = true;
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
