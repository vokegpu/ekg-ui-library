#include <ekg/impl/ekg_ui_element_abstract.hpp>

ekg_element::ekg_element() {

}

ekg_element::~ekg_element() {

}

void ekg_element::on_pre_event_update(SDL_Event &sdl_event) {

}

void ekg_element::on_event(SDL_Event &sdl_event) {

}

void ekg_element::on_post_event_update(SDL_Event &sdl_event) {

}

void ekg_element::on_update() {

}

void ekg_element::on_draw_refresh() {

}

void ekg_element::on_killed() {

}

void ekg_element::on_sync() {

}

void ekg_element::set_id(uint32_t element_id) {
    this->id = element_id;
}

uint32_t ekg_element::get_type() {
    return this->id;
}

void ekg_element::set_visibility(uint32_t visible) {
    this->visibility = visible;
}

uint32_t ekg_element::get_visibility() {
    return this->visibility;
}

void ekg_element::set_master_id(uint32_t element_id) {
    this->master_id = element_id;
}

uint32_t ekg_element::get_id() {
    return this->id;
}

ekgutil::flag &ekg_element::access_flag() {
    return this->flag;
}

ekgutil::stack &ekg_element::access_children_stack() {
    return this->children_stack;
}

float ekg_element::get_x() {
    return this->rect.x;
}

float ekg_element::get_y() {
    return this->rect.y;
}

float ekg_element::get_width() {
    return this->rect.w;
}

float ekg_element::get_height() {
    return this->rect.h;
}
