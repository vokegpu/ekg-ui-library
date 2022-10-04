#include "ekg/ui/frame/ui_frame_widget.hpp"

void ekg::ui::frame_widget::destroy() {
    abstract_widget::destroy();
}

void ekg::ui::frame_widget::on_reload() {
    abstract_widget::on_reload();
    auto ui = (ekg::ui::frame_widget*) this->data;
}

void ekg::ui::frame_widget::on_pre_event(SDL_Event &sdl_event) {
    abstract_widget::on_pre_event(sdl_event);
}

void ekg::ui::frame_widget::on_event(SDL_Event &sdl_event) {
    abstract_widget::on_event(sdl_event);
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
