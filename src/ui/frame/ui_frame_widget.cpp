#include "ekg/ui/frame/ui_frame_widget.hpp"
#include "ekg/ui/frame/ui_frame.hpp"
#include "ekg/util/env.hpp"

void ekg::ui::frame_widget::destroy() {
    abstract_widget::destroy();
}

void ekg::ui::frame_widget::on_reload() {
    abstract_widget::on_reload();

    auto ui = (ekg::ui::frame*) this->data;
    auto drag_dock = ui->get_drag_dock();
    auto resize_dock = ui->get_resize_dock();

    /* Start of drag metrics reload. */
    if (ekg::bitwise::contains(drag_dock, ekg::dock::left)) {

    }

    if (ekg::bitwise::contains(drag_dock, ekg::dock::right)) {

    }

    if (ekg::bitwise::contains(drag_dock, ekg::dock::top)) {

    }

    if (ekg::bitwise::contains(drag_dock, ekg::dock::bottom)) {

    }
    /* End of drag metrics reload. */

    /* Start of resize metrics reload. */
    if (ekg::bitwise::contains(resize_dock, ekg::dock::left)) {

    }

    if (ekg::bitwise::contains(resize_dock, ekg::dock::right)) {

    }

    if (ekg::bitwise::contains(resize_dock, ekg::dock::top)) {

    }

    if (ekg::bitwise::contains(resize_dock, ekg::dock::bottom)) {

    }
    /* End of resize metrics reload. */
}

void ekg::ui::frame_widget::on_pre_event(SDL_Event &sdl_event) {
    abstract_widget::on_pre_event(sdl_event);
}

void ekg::ui::frame_widget::on_event(SDL_Event &sdl_event) {
    abstract_widget::on_event(sdl_event);

    auto ui = (ekg::ui::frame*) this->data;
    bool keep_process_activy = this->flag.hovered && (!(ekg::bitwise::contains(ui->get_drag_dock(), ekg::dock::none) && ekg::input("frame-drag-activy")) || (!(ekg::bitwise::contains(ui->get_resize_dock(), ekg::dock::none) && ekg::input("frame-resize-activy"))));

    if (keep_process_activy) {
        this->target_dock_drag = 0;
        this->target_dock_resize = 0;
    }

    if (keep_process_activy) {

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
