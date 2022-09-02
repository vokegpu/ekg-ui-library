#include "ekg/ekg.hpp"

#include "ekg/impl/ekg_ui_element_tab.hpp"


void ekg_tab::set_size(float width, float height) {
    ekg_element::set_size(width, height);
}

void ekg_tab::set_pos(float x, float y) {
    ekg_element::set_pos(x, y);
}

void ekg_tab::on_sync() {
    ekg_element::on_sync();
}

void ekg_tab::on_pre_event_update(SDL_Event &sdl_event) {
    ekg_element::on_pre_event_update(sdl_event);
}

void ekg_tab::on_event(SDL_Event &sdl_event) {
    ekg_element::on_event(sdl_event);
}

void ekg_tab::on_post_event_update(SDL_Event &sdl_event) {
    ekg_element::on_post_event_update(sdl_event);
}

void ekg_tab::on_update() {
    ekg_element::on_update();
}

void ekg_tab::on_draw_refresh() {
    ekg_element::on_draw_refresh();
}

