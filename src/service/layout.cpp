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

#include "ekg/service/layout.hpp"
#include "ekg/ui/frame/ui_frame.hpp"
#include "ekg/ekg.hpp"
#include "ekg/ui/frame/ui_frame_widget.hpp"

void ekg::service::layout::set_preset_mask(const ekg::vec3 &offset, ekg::axis axis, float initial_respective_size) {
    this->dock_axis_mask = axis;    
    this->offset_mask = offset;
    this->respective_mask_all = initial_respective_size;
}

void ekg::service::layout::insert_into_mask(const ekg::dockrect &dockrect) {
    if ((static_cast<int32_t>(dockrect.rect->w == 0) || static_cast<int32_t>(dockrect.rect->h) == 0) || ekg::bitwise::contains(dockrect.dock, ekg::dock::none)) {
        return;
    }

    this->dockrect_list.push_back(dockrect);
}

void ekg::service::layout::process_layout_mask() {
    if (this->dockrect_list.empty()) {
        return;
    }

    /*
     * V is the the respective size (axis horizontal == width | axis vertical == height)
     */
    bool axis {this->dock_axis_mask == ekg::axis::horizontal}, left_or_right {};
    float v {this->respective_mask_all};
    float centered_dimension {this->offset_mask.z / 2};
    float opposite {}, uniform {};
    float clamped_offset {};

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
            clamped_offset = (dockrect.rect->h + this->offset_mask.y) - this->offset_mask.z > 0 ? 0 : this->offset_mask.y;
            left_or_right = ekg::bitwise::contains(dockrect.dock, ekg::dock::left) || ekg::bitwise::contains(dockrect.dock, ekg::dock::right);

            if (ekg::bitwise::contains(dockrect.dock, ekg::dock::center) && !left_or_right) {
                dockrect.rect->x = (v / 2) - (dockrect.rect->w / 2);
                dockrect.rect->y = centered_dimension - (dockrect.rect->h / 2);
                this->layout_mask.w += dockrect.rect->w + this->offset_mask.x;
            } else if (ekg::bitwise::contains(dockrect.dock, ekg::dock::center)) {
                dockrect.rect->y = centered_dimension - (dockrect.rect->h / 2);
            }

            /* when there is a opposite dock, layout should x the dock position to actual position */
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
                dockrect.rect->y = clamped_offset;
            } else if (ekg::bitwise::contains(dockrect.dock, ekg::dock::bottom)) {
                dockrect.rect->y = this->offset_mask.z - clamped_offset - dockrect.rect->h;
            }
        } else {
            clamped_offset = ekg::clamp((dockrect.rect->w + this->offset_mask.x) - this->offset_mask.z, 0, this->offset_mask.x);
            left_or_right = ekg::bitwise::contains(dockrect.dock, ekg::dock::left) || ekg::bitwise::contains(dockrect.dock, ekg::dock::right);
            
            if (ekg::bitwise::contains(dockrect.dock, ekg::dock::center) && !left_or_right) {
                dockrect.rect->y = (this->respective_mask_center) + (dockrect.rect->h / 2);
                dockrect.rect->x = centered_dimension - (dockrect.rect->w / 2);
            } else if (ekg::bitwise::contains(dockrect.dock, ekg::dock::center)) {
                dockrect.rect->x = centered_dimension - (dockrect.rect->w / 2);
            }

            /* when there is a opposite dock, layout should fix the dock position to actual position */
            if (ekg::bitwise::contains(dockrect.dock, ekg::dock::top)) {
                if (static_cast<int32_t>(opposite) != 0) {
                    this->layout_mask.h -= opposite;
                } 

                dockrect.rect->y = this->layout_mask.h;
                this->layout_mask.h += dockrect.rect->h + this->offset_mask.y + opposite;
                opposite = 0;
                uniform = dockrect.rect->h + this->offset_mask.y;
            }

            if (ekg::bitwise::contains(dockrect.dock, ekg::dock::bottom)) {
                if (static_cast<int32_t>(uniform) != 0) {
                    this->layout_mask.h -= uniform;
                }

                this->layout_mask.h += dockrect.rect->h;
                dockrect.rect->y = v - this->layout_mask.h;
                this->layout_mask.h += uniform + this->offset_mask.y;
                opposite = dockrect.rect->h + this->offset_mask.y;
                uniform = 0;
            }

            if (ekg::bitwise::contains(dockrect.dock, ekg::dock::left)) {
                dockrect.rect->x = clamped_offset;
            }

            if (ekg::bitwise::contains(dockrect.dock, ekg::dock::right)) {
                dockrect.rect->x = this->offset_mask.z - clamped_offset - dockrect.rect->w;
            }
        }
    }

    if (axis) {
        this->layout_mask.w = ekg::min(v, this->layout_mask.w);
    } else {
        this->layout_mask.h = ekg::min(v, this->layout_mask.h);
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

    float respective_size {(this->dock_axis_mask == ekg::axis::horizontal) ? this->offset_mask.x : this->offset_mask.y};
    float respective_center_size {respective_size};
    float size {};

    int32_t only_center_count {};

    for (ekg::dockrect &dockrect : this->dockrect_list) {
        if (dockrect.rect == nullptr) {
            continue;
        }

        size = (this->dock_axis_mask == ekg::axis::horizontal) ? (dockrect.rect->w + this->offset_mask.x) : (dockrect.rect->h + this->offset_mask.y);
        if (ekg::bitwise::contains(dockrect.dock, ekg::dock::center) && !(ekg::bitwise::contains(dockrect.dock, ekg::dock::left) || ekg::bitwise::contains(dockrect.dock, ekg::dock::right))) {
            respective_center_size += size;
            only_center_count++;
        }

        respective_size += size;
    }

    ekg::log() << respective_size;

    this->respective_mask_center = (only_center_count != 0 ? (respective_center_size / only_center_count) : 0.0f);
    this->respective_mask_all = ekg::min(this->respective_mask_all, respective_size);
    
    return this->respective_mask_all;
}

float ekg::service::layout::get_dimensional_extent(ekg::ui::abstract_widget *widget, uint16_t flag_ok, uint16_t flag_stop, int64_t &begin_and_count, ekg::axis axis) {
    if (widget == nullptr) {
        return 0.0f;
    }

    uint16_t flags {};
    int32_t ids {};
    int64_t n {};
    int64_t it {begin_and_count};

    if (begin_and_count > static_cast<int64_t>(this->extent_data[0]) && begin_and_count < static_cast<int64_t>(this->extent_data[1])) {
        begin_and_count = static_cast<int64_t>(this->extent_data[3]);
        return this->extent_data[2];
    }

    this->extent_data[0] = static_cast<float>(it);

    float extent {};
    ekg::ui::abstract_widget *widgets {};
    std::vector<int32_t> &child_id_list {widget->data->get_child_id_list()};
    uint64_t size {child_id_list.size()};
    bool is_last_index {};

    /*
     * The last index does not check if there is some next flag,
     * because of this, there is a last index checker.
     *
     * The extent data store the previous bounding indices,
     * in simply words, prevent useless iteration.
     *
     * The min offset is added for extent, because we need count
     * the offset position when spliting the fill width, but the
     * last extent space is not necessary, so we need to subtract.
     *
     */
    for (it = it; it < child_id_list.size(); it++) {
        ids = child_id_list.at(it);
        if ((widgets = ekg::core->get_fast_widget_by_id(ids)) == nullptr) {
            continue;
        }

        flags = widgets->data->get_place_dock();
        is_last_index = it == size - 1;

        if ((ekg::bitwise::contains(flags, flag_stop) && it != begin_and_count) || is_last_index) {
            extent -= this->min_offset;
            n += (ekg::bitwise::contains(flags, flag_ok) && is_last_index);

            this->extent_data[1] = static_cast<float>(it + is_last_index);
            this->extent_data[2] = extent;
            this->extent_data[3] = static_cast<float>(n == 0 ? 1 : n);
            break;
        }

        if (ekg::bitwise::contains(flags, flag_ok)) {
            n++;
            continue;
        }

        switch (axis) {
        case ekg::axis::horizontal:
            extent += widgets->dimension.w + this->min_offset;
            break;
        case ekg::axis::vertical:
            extent += widgets->dimension.h;
            break;
        }
    }

    begin_and_count = n == 0 ? 1 : n;
    return extent;
}

void ekg::service::layout::process(ekg::ui::abstract_widget *pwidget) {

}

void ekg::service::layout::process_scaled(ekg::ui::abstract_widget* widget_parent) {
    if (widget_parent == nullptr || widget_parent->data == nullptr) {
        return;
    }

    auto type {widget_parent->data->get_type()};
    auto is_group {type == ekg::type::frame};
    auto &parent_rect = widget_parent->data->widget();

    if (!is_group || parent_rect.w == 0 || parent_rect.h == 0) {
        return;
    }

    auto &grid {this->grid_map[widget_parent->data->get_id()]};
    float group_top_offset {this->min_offset};

    switch (type) {
        case ekg::type::frame: {
            auto widget_group {(ekg::ui::frame_widget*) widget_parent};
            auto ui_group {(ekg::ui::frame*) widget_parent->data};

            if (ekg::bitwise::contains(ui_group->get_drag_dock(), ekg::dock::top)) {
                group_top_offset = widget_group->docker_activy_drag.top.h;
            }

            break;
        }
    }

    this->scaled_width_divided = parent_rect.w / 3;
    this->scaled_width_divided = parent_rect.h / 2;
    this->enum_docks_flag = 0;

    ekg::ui::abstract_widget *widgets {};
    uint16_t flags {};
    int64_t it {};
    float dimensional_extent {};
    int64_t count {};

    /*
     * The dimension of mother/parent group is not scaled by min offsets,
     * when transforming unuscaled sizes, it can return a wrong position,
     * in simple words, non-scaled size return non aligned positions.
     */
    ekg::rect group_rect {widget_parent->dimension};
    group_rect.w -= this->min_offset * 2.0f;
    group_rect.h -= this->min_offset * 2.0f;

    ekg::rect widget_rect {};
    ekg::rect bottom_rect {};
    ekg::rect top_rect {this->min_offset, group_top_offset, 0.0f, 0.0f};
    ekg::rect prev_widget_layout {};

    uint16_t prev_flags {};

    /*
     * To find the precise fill width, then this data store
     * the previous extent finding and check the current it,
     * this prevents useless iterations and tick waste.
     */
    this->extent_data[0] = 0.0f;
    this->extent_data[1] = 0.0f;
    this->extent_data[2] = 0.0f;

    // @TODO Fix fill -> (fill | next) issue with widgets.

    /*
     * Rect == absolute position of current widget
     * Layout == position based on parent absolute widget 
     */
    for (int32_t &ids : widget_parent->data->get_child_id_list()) {
        if (ids == 0 || (widgets = ekg::core->get_fast_widget_by_id(ids)) == nullptr) {
            continue;
        }

        auto &rect {widgets->data->widget()};
        auto &layout {widgets->dimension};

        flags = widgets->data->get_place_dock();
        if (ekg::bitwise::contains(flags, ekg::dock::next) && ekg::bitwise::contains(flags, ekg::dock::fill)) {
            top_rect.h += prev_widget_layout.h + this->min_offset;
            top_rect.w = 0.0f;

            layout.x = top_rect.x + top_rect.w;
            layout.y = top_rect.y + top_rect.h;

            count = it;
            dimensional_extent = this->get_dimensional_extent(widget_parent, ekg::dock::fill, ekg::dock::next, count, ekg::axis::horizontal);
            dimensional_extent = ekg::min(((group_rect.w - dimensional_extent) - (count * this->min_offset)) / static_cast<float>(count), widgets->min_size.x);

            top_rect.w += dimensional_extent + this->min_offset;
            layout.w = dimensional_extent;
            widgets->on_reload();
        } else if (ekg::bitwise::contains(flags, ekg::dock::fill)) {
            layout.x = top_rect.x + top_rect.w;
            layout.y = top_rect.y + top_rect.h;

            count = it;
            dimensional_extent = this->get_dimensional_extent(widget_parent, ekg::dock::fill, ekg::dock::next, count, ekg::axis::horizontal);
            dimensional_extent = ekg::min(((group_rect.w - dimensional_extent) - (count * this->min_offset)) / static_cast<float>(count), widgets->min_size.x);

            top_rect.w += dimensional_extent + this->min_offset;
            layout.w = dimensional_extent;
            widgets->on_reload();
        } else if (ekg::bitwise::contains(flags, ekg::dock::next)) {
            top_rect.h += prev_widget_layout.h + this->min_offset;
            top_rect.w = 0.0f;

            layout.x = top_rect.x;
            layout.y = top_rect.y + top_rect.h;
            top_rect.w += layout.w + this->min_offset;

            count = it;
            dimensional_extent = this->get_dimensional_extent(widget_parent, ekg::dock::fill, ekg::dock::next, count, ekg::axis::horizontal);
        } else if (flags == ekg::dock::none) {
            layout.x = top_rect.x + top_rect.w;
            layout.y = top_rect.y + top_rect.h;
            top_rect.w += layout.w + this->min_offset;

            count = it;
            dimensional_extent = this->get_dimensional_extent(widget_parent, ekg::dock::fill, ekg::dock::next, count, ekg::axis::horizontal);
        }

        prev_widget_layout = layout;
        prev_flags = flags;
        it++;
    }
}

void ekg::service::layout::init() {
    this->min_factor_height = 1;
    this->min_height = ekg::core->get_f_renderer_normal().get_text_height();
    this->min_fill_width = this->min_height;
    this->min_offset = this->min_height / 3;
    this->min_height += this->min_offset;
    this->min_offset /= 2;
}

void ekg::service::layout::quit() {

}
