#include "ekg/ui/frame/ui_frame_widget.hpp"
#include "ekg/ui/frame/ui_frame.hpp"
#include "ekg/ekg.hpp"

void ekg::ui::frame_widget::destroy() {
    abstract_widget::destroy();
}

void ekg::ui::frame_widget::on_reload() {
    abstract_widget::on_reload();

    auto ui = (ekg::ui::frame*) this->data;
    auto drag_dock = ui->get_drag_dock();
    auto resize_dock = ui->get_resize_dock();

    ekg::set_dock_scaled(this->rect, ekg::theme().frame_activy_offset, this->docker_activy_drag);
    ekg::set_dock_scaled(this->rect, ekg::theme().frame_activy_offset / 2, this->docker_activy_resize);
}

void ekg::ui::frame_widget::on_pre_event(SDL_Event &sdl_event) {
    abstract_widget::on_pre_event(sdl_event);
}

void ekg::ui::frame_widget::on_event(SDL_Event &sdl_event) {
    abstract_widget::on_event(sdl_event);

    auto interact = ekg::interact();
    auto ui = (ekg::ui::frame*) this->data;

    if ((ui->get_drag_dock() != ekg::dock::none || ui->get_resize_dock() != ekg::dock::none) && ekg::was_pressed() && this->flag.hovered && !this->flag.activy && (ekg::interact("frame-drag-activy") || ekg::interact("frame-resize-activy"))) {
        if (ui->get_resize_dock() != ekg::dock::none) {
            this->target_dock_drag = ekg::docker_collide_vec_docks(this->docker_activy_drag, interact);
        }

        if (ui->get_drag_dock() != ekg::dock::none) {
            this->target_dock_drag = ekg::docker_collide_vec_docks(this->docker_activy_drag, interact);

            this->extra.x = interact.x - this->rect.x;
            this->extra.y = interact.y - this->rect.y;
        }

        this->flag.activy = (this->target_dock_drag != ekg::dock::none || this->target_dock_resize != ekg::dock::none);
    } else if (ekg::was_motion() && this->flag.activy) {

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
}
