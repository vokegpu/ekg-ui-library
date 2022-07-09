#include <ekg/impl/ekg_ui_element_frame.hpp>

void ekg_frame::on_pre_event_update(SDL_Event &sdl_event) {
    ekg_element::on_pre_event_update(sdl_event);
}

void ekg_frame::on_event(SDL_Event &sdl_event) {
    ekg_element::on_event(sdl_event);
}

void ekg_frame::on_post_event_update(SDL_Event &sdl_event) {
    ekg_element::on_post_event_update(sdl_event);
}

void ekg_frame::on_update() {
    ekg_element::on_update();
}

void ekg_frame::on_draw_refresh() {
    ekg_element::on_draw_refresh();
}
