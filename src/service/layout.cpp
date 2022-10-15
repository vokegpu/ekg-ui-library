#include "ekg/service/layout.hpp"
#include "ekg/ui/frame/ui_frame.hpp"
#include "ekg/ekg.hpp"

void ekg::service::layout::process(ekg::ui::abstract_widget *widget) {

}

void ekg::service::layout::process_scaled(ekg::ui::abstract_widget* widget) {
    auto type {widget->data->get_type()};
    auto is_group {type == ekg::type::frame};
    auto &parent_rect = widget->data->widget();

    if (!is_group || parent_rect.w == 0 || parent_rect.h == 0) {
        return;
    }

    auto &grid {this->grid_map[widget->data->get_id()]};
    ekg::ui::abstract_widget* widgets {};

    this->scaled_width_divided = parent_rect.w / 3;
    this->scaled_width_divided = parent_rect.h / 2;
    this->enum_docks_flag = 0;

    this->curr_top = {0, this->min_offset, 0, 0};
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

    for (uint32_t &ids : widget->data->get_parent_id_list()) {
        widgets = ekg::core->get_fast_widget_by_id(ids);

        if (widgets == nullptr) {
            continue;
        }

        auto &rect {widgets->data->widget()};
        auto &layout {widgets->layout};

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
                if (!this->prev_flag_top.left || this->prev_flag_bottom.bottom || this->prev_flag_top.next) {
                    this->curr_top.x = 0;
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
    }
}

void ekg::service::layout::init() {
    this->min_factor_height = 1;
    this->min_height = ekg::core->get_f_renderer_normal().get_text_height();
    this->min_offset = this->min_height / 3;
    this->min_height += this->min_offset;
}

void ekg::service::layout::quit() {

}
