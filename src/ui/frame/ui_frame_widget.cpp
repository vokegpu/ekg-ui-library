#include "ekg/ui/frame/ui_frame_widget.hpp"
#include "ekg/ui/frame/ui_frame.hpp"
#include "ekg/ekg.hpp"
#include "ekg/draw/draw.hpp"
#include "ekg/util/util_ui.hpp"

void ekg::ui::frame_widget::destroy() {
    abstract_widget::destroy();
}

void ekg::ui::frame_widget::on_reload() {
    abstract_widget::on_reload();

    ekg::set_rect_clamped(this->rect, ekg::theme().min_widget_size);
    ekg::set_dock_scaled(this->rect, static_cast<float>(ekg::theme().frame_activy_offset), this->docker_activy_drag);
    ekg::set_dock_scaled(this->rect, static_cast<float>(ekg::theme().frame_activy_offset) / 4, this->docker_activy_resize);
}

void ekg::ui::frame_widget::on_pre_event(SDL_Event &sdl_event) {
    abstract_widget::on_pre_event(sdl_event);
}

void ekg::ui::frame_widget::on_event(SDL_Event &sdl_event) {
    abstract_widget::on_event(sdl_event);

    auto interact = ekg::interact();
    auto ui = (ekg::ui::frame*) this->data;

    if ((ui->get_drag_dock() != ekg::dock::none || ui->get_resize_dock() != ekg::dock::none) && ekg::was_pressed() && this->flag.hovered && !this->flag.activy && (ekg::input("frame-drag-activy") || ekg::input("frame-resize-activy"))) {
        this->target_dock_drag = ekg::find_collide_dock(this->docker_activy_drag, ui->get_drag_dock(), interact);
        this->target_dock_resize = ekg::find_collide_dock(this->docker_activy_resize, ui->get_resize_dock(), interact);

        this->extra.x = interact.x - this->rect.x;
        this->extra.y = interact.y - this->rect.y;
        this->extra.w = this->rect.x + this->rect.w;
        this->extra.h = this->rect.y + this->rect.h;

        this->flag.activy = this->target_dock_drag != ekg::dock::none || this->target_dock_resize != ekg::dock::none;
    } else if (ekg::was_motion() && this->flag.activy) {
        ekg::rect new_rect {this->rect};

        if (this->target_dock_drag != ekg::dock::none && this->target_dock_resize == ekg::dock::none) {
            new_rect.x = interact.x - this->extra.x;
            new_rect.y = interact.y - this->extra.y;
        }

        if (this->target_dock_resize != ekg::dock::none) {
            if (ekg::bitwise::contains(this->target_dock_resize, ekg::dock::left)) {
                new_rect.x = interact.x - this->extra.x;
                new_rect.w = this->extra.w - new_rect.x;
            }

            if (ekg::bitwise::contains(this->target_dock_resize, ekg::dock::right)) {
                new_rect.w = (interact.x - this->extra.x) - new_rect.x + (this->extra.w - new_rect.x);
            }

            if (ekg::bitwise::contains(this->target_dock_resize, ekg::dock::top)) {
                new_rect.y = interact.y - this->extra.y;
                new_rect.h = this->extra.h - new_rect.y;
            }

            if (ekg::bitwise::contains(this->target_dock_resize, ekg::dock::bottom)) {
                new_rect.h = (interact.y - this->extra.y) - new_rect.y + (this->extra.h - new_rect.y);
            }
        }

        if (this->rect != new_rect) {
            this->rect = new_rect;
            ekg::update(this);
        }
    }

    if (ekg::was_released()) {
        this->target_dock_resize = ekg::dock::none;
        this->target_dock_drag = ekg::dock::none;
        this->flag.activy = false;
    }
}

void ekg::ui::frame_widget::on_post_event(SDL_Event &sdl_event) {
    abstract_widget::on_post_event(sdl_event);
}

void ekg::ui::frame_widget::on_update() {
    abstract_widget::on_update();
}

void ekg::ui::frame_widget::on_draw_refresh() {
    abstract_widget::on_draw_refresh();
    ekg::draw::rect(this->rect, ekg::theme().frame_background);
    ekg::draw::rect(this->rect, {0, 0, 0, 255}, 1);
}