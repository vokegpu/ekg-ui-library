#include <ekg/impl/ekg_ui_element_check_box.hpp>

ekg_check_box::ekg_check_box() {

}

ekg_check_box::~ekg_check_box() {

}

void ekg_check_box::set_text(const std::string &string) {

}

std::string ekg_check_box::get_text() {
    return std::string();
}

void ekg_check_box::set_callback_flag(bool val) {

}

bool ekg_check_box::get_callback_flag() {
    return false;
}

float ekg_check_box::get_min_text_width() {
    return 0;
}

float ekg_check_box::get_min_text_height() {
    return 0;
}

void ekg_check_box::set_text_dock(uint16_t flags) {

}

uint16_t ekg_check_box::get_text_dock() {
    return 0;
}

void ekg_check_box::set_width(float width) {

}

void ekg_check_box::set_height(float height) {

}

void ekg_check_box::set_size(float width, float height) {
    ekg_element::set_size(width, height);
}

void ekg_check_box::set_pos(float x, float y) {
    ekg_element::set_pos(x, y);
}

void ekg_check_box::on_killed() {
    ekg_element::on_killed();
}

void ekg_check_box::on_sync() {
    ekg_element::on_sync();
}

void ekg_check_box::on_pre_event_update(SDL_Event &sdl_event) {
    ekg_element::on_pre_event_update(sdl_event);
}

void ekg_check_box::on_event(SDL_Event &sdl_event) {
    ekg_element::on_event(sdl_event);
}

void ekg_check_box::on_post_event_update(SDL_Event &sdl_event) {
    ekg_element::on_post_event_update(sdl_event);
}

void ekg_check_box::on_draw_refresh() {
    ekg_element::on_draw_refresh();
}
