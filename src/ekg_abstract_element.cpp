#include <ekg/ekg_abstract_element.hpp>

void ekg_abstract_element::on_pre_event_update(SDL_Event &sdl_event) {

}

void ekg_abstract_element::on_event(SDL_Event &sdl_event) {

}

void ekg_abstract_element::on_post_event_update(SDL_Event &sdl_event) {

}

void ekg_abstract_element::on_update() {

}

void ekg_abstract_element::on_reload_draw() {
    utility::log("sou linda! qwdiuwqiu");
}

void ekg_abstract_element::set_element_id(uint32_t new_element_id) {
    this->element_id = new_element_id;
}

uint32_t ekg_abstract_element::get_element_id() {
    return this->element_id;
}

uint8_t ekg_abstract_element::get_element_type() {
    return this->element_type;
}

void ekg_abstract_element::set_tag(std::string &tag) {
    this->element_tag = tag;
}

std::string ekg_abstract_element::get_tag() {
    return this->element_tag;
}

utility::flag &ekg_abstract_element::get_flag() {
    return this->flag;
}

void ekg_abstract_element::set_visibility(utility::visibility enum_visibility) {
    this->visibility = enum_visibility;
}

utility::visibility ekg_abstract_element::get_visibility() {
    return this->visibility;
}

utility::stack &ekg_abstract_element::get_children_stack() {
    return this->children_stack;
}
