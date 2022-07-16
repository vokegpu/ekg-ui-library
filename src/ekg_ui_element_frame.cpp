#include <ekg/impl/ekg_ui_element_frame.hpp>
#include <ekg/api/ekg_gpu.hpp>
#include <ekg/ekg.hpp>

void ekg_frame::on_sync() {
    ekg_element::on_sync();

    this->rect.w = this->rect.w < this->min_width ? this->min_width : this->rect.w;
    this->rect.h = this->rect.h < this->min_height ? this->min_height : this->rect.h;

    ekg::core::instance.dispatch_todo_event(ekgutil::action::REFRESH);
    ekg_element* element;

    for (uint32_t &ids : this->children_stack.ids) {
        if (!ekg::core::instance.find_element(element, ids) || element->access_flag().dead) {
            continue;
        }

        element->access_scaled_rect().copy(this->rect);
        element->set_pos(element->get_sync_x(), element->get_sync_y());
    }
}

void ekg_frame::on_pre_event_update(SDL_Event &sdl_event) {
    ekg_element::on_pre_event_update(sdl_event);

    float x = 0;
    float y = 0;

    if (ekgapi::motion(sdl_event, x, y) || ekgapi::any_input_down(sdl_event, x, y)) {
        ekgapi::set_direct(this->flag.old_over, this->flag.over, this->rect.collide_aabb_with_point(x, y));
    }
}

void ekg_frame::on_event(SDL_Event &sdl_event) {
    ekg_element::on_event(sdl_event);

    float mx = 0;
    float my = 0;

    if (ekgapi::input_down_left(sdl_event, mx, my)) {
        if (this->flag.over && !this->no_draggable && (ekgutil::find_axis_dock(this->enum_target_drag_dock, mx, my, this->offset_drag_dock, this->rect) && ekgutil::contains(this->enum_flags_drag_dock, this->enum_target_drag_dock)) || ekgutil::contains(this->enum_flags_drag_dock, ekg::dock::FULL)) {
            this->cache.x = mx - this->rect.x;
            this->cache.y = my - this->rect.y;
            this->dragging = true;
        }

        if (this->flag.over && !this->no_resizable && (ekgutil::find_axis_dock(this->enum_target_resize_dock, mx, my, this->offset_resize_dock, this->rect) && ekgutil::contains(this->enum_flags_resize_dock, this->enum_target_resize_dock) || ekgutil::contains(this->enum_flags_drag_dock, ekg::dock::FULL))) {
            this->cache.x = mx - this->rect.x;
            this->cache.y = my - this->rect.y;
            this->cache.w = this->rect.x + this->rect.w;
            this->cache.h = this->rect.y + this->rect.h;

            this->resizing = true;
        }
    } else if (ekgapi::any_input_up(sdl_event)) {
        this->resizing = false;
        this->dragging = false;
    } else if (ekgapi::motion(sdl_event, mx, my)) {
        if (!this->no_draggable && !this->resizing && this->dragging && this->enum_target_drag_dock != ekg::dock::UNDEFINED) {
            this->set_pos((mx - this->cache.x) - this->scaled.x, (my - this->cache.y) - this->scaled.y);
        }

        if (!this->no_resizable && !this->dragging && this->resizing && this->enum_target_resize_dock != ekg::dock::UNDEFINED) {
            bool top    = ekgutil::contains(this->enum_target_resize_dock, ekg::dock::TOP);
            bool bottom = ekgutil::contains(this->enum_target_resize_dock, ekg::dock::BOTTOM);
            bool left   = ekgutil::contains(this->enum_target_resize_dock, ekg::dock::LEFT);
            bool right  = ekgutil::contains(this->enum_target_resize_dock, ekg::dock::RIGHT);

            // p (position) s (size)
            float px = this->rect.x;
            float py = this->rect.y;
            float sw = this->rect.w;
            float sh = this->rect.h;

            if (left) {
                px = mx - this->cache.x;
                sw = this->cache.w - px;
            }

            if (right) {
                sw = (mx - this->cache.x) - px + (this->cache.w - px);
            }

            if (top) {
                py = my - this->cache.y;
                sh = this->cache.h - py;
            }

            if (bottom) {
                sh = (my - this->cache.y) - py + (this->cache.h - py);
            }

            set(px - this->scaled.x, py - this->scaled.y, sw, sh);
        }
    }
}

void ekg_frame::on_post_event_update(SDL_Event &sdl_event) {
    ekg_element::on_post_event_update(sdl_event);
    ekgapi::set_direct(this->flag.old_over, this->flag.over, false);
}

void ekg_frame::on_update() {
    ekg_element::on_update();
}

void ekg_frame::on_draw_refresh() {
    ekg_element::on_draw_refresh();

    ekggpu::rectangle(this->rect, ekg::theme().frame_background);
}

void ekg_frame::set_min_width(float min) {
    this->min_width = min < 30 ? 30 : min;
}

float ekg_frame::get_min_width() {
    return this->min_width;
}

void ekg_frame::set_min_height(float min) {
    this->min_height = min < 30 ? 30 : min;
}

float ekg_frame::get_min_height() {
    return this->min_height;
}

void ekg_frame::set_width(float width) {
    width = width < this->min_width ? this->min_width : width;

    if (this->rect.w != width) {
        this->rect.w = width;
        this->on_sync();
    }
}

void ekg_frame::set_height(float height) {
    height = height < this->min_height ? this->min_height : height;

    if (this->rect.h != height) {
        this->rect.h = height;
        this->on_sync();
    }
}

void ekg_frame::set_tag(std::string &string) {
    this->tag = string;
}

std::string ekg_frame::get_tag() {
    return this->tag;
}

void ekg_frame::set_size(float width, float height) {
    width = width < this->min_width ? this->min_width : width;
    height = height < this->min_height ? this->min_height : height;

    if (this->rect.w != width || this->rect.h != height) {
        this->rect.w = width;
        this->rect.h = height;
        this->on_sync();
    }
}

void ekg_frame::set_pos(float x, float y) {
    ekg_element::set_pos(x, y);
}

void ekg_frame::set(float x, float y, float width, float height) {
    set_pos(x, y);
    set_size(width, height);
}

void ekg_frame::place(ekg_element* element, float x, float y) {
    if (element->get_id() == 0 || element->get_id() == this->id || this->children_stack.contains(element->get_id())) {
        return;
    }

    this->children_stack.add(element->get_id());

    element->set_master_id(this->id);
    element->access_scaled_rect().copy(this->rect);
    element->set_pos(x, y);

    // We need to fix the stack after put the element into frame.
    ekg::core::instance.dispatch_todo_event(ekgutil::action::FIXSTACK);
    ekg::core::instance.dispatch_todo_event(ekgutil::action::REFRESH);
}

void ekg_frame::set_resize_dock(uint16_t dock) {
    this->no_resizable = false;

    if (dock == NULL) {
        this->no_resizable = true;
        return;
    }

    if (this->enum_flags_resize_dock != dock) {
        this->enum_flags_resize_dock = dock;
        this->on_sync();
    }
}

uint16_t ekg_frame::get_resize_dock() {
    return this->enum_flags_resize_dock;
}

void ekg_frame::set_resize_offset(float offset) {
    if  (this->offset_resize_dock != offset) {
        this->offset_resize_dock = offset;
        this->on_sync();
    }
}

float ekg_frame::get_resize_offset() {
    return this->offset_resize_dock;
}

void ekg_frame::set_drag_dock(uint16_t dock) {
    this->no_draggable = false;

    if (dock == NULL) {
        this->no_draggable = true;
        return;
    }

    if (this->enum_flags_drag_dock != dock) {
        this->enum_flags_drag_dock = dock;
        this->on_sync();
    }
}

uint16_t ekg_frame::get_drag_dock() {
    return this->enum_flags_drag_dock;
}

void ekg_frame::set_drag_offset(float offset) {
    if (this->offset_drag_dock != offset) {
        this->offset_drag_dock = offset;
        this->on_sync();
    }
}

float ekg_frame::get_drag_offset() {
    return this->offset_drag_dock;
}
