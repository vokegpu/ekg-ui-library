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

    if (element_id == 0) {
        this->scaled.x = 0;
        this->scaled.y = 0;
        this->scaled.w = 0;
        this->scaled.h = 0;
    }
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

ekgmath::rect &ekg_element::access_scaled() {
    return this->scaled;
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

void ekg_element::collect_stack(ekgutil::stack &stack) {
    if (stack.contains(this->id)) {
        return;
    }

    stack.add(this->id);

    for (uint32_t &ids : this->children_stack.ids) {
        stack.add(ids);
    }
}
