#include <ekg/ekg_element_button.hpp>

ekg_button::ekg_button() {

}

ekg_button::~ekg_button() {

}

void ekg_button::on_killed() {
    ekg_abstract_element::on_killed();
}

void ekg_button::on_sync() {
    ekg_abstract_element::on_sync();
}

void ekg_button::on_pre_event_update(SDL_Event &sdl_event) {
    ekg_abstract_element::on_pre_event_update(sdl_event);
}

void ekg_button::on_event(SDL_Event &sdl_event) {
    ekg_abstract_element::on_event(sdl_event);
}

void ekg_button::on_post_event_update(SDL_Event &sdl_event) {
    ekg_abstract_element::on_post_event_update(sdl_event);
}

void ekg_button::on_draw_refresh() {
    ekg_abstract_element::on_draw_refresh();
}