#include <ekg/ekg.hpp>
#include "ekg/impl/ekg_ui_element_slider.hpp"


float ekg_slider::get_min_text_width() {
    return this->min_text_width;
}

float ekg_slider::get_min_text_height() {
    return this->min_text_height;
}

void ekg_slider::set_text_dock(uint16_t flags) {
    if (flags == NULL) {
        this->enum_flags_text_dock = ekg::dock::UNDEFINED;
    }

    if (this->enum_flags_text_dock != flags) {
        this->enum_flags_text_dock = flags;
        this->on_sync();
    }
}

uint16_t ekg_slider::get_text_dock() {
    return this->enum_flags_text_dock;
}

void ekg_slider::set_width(float width) {
    this->set_size(width, this->rect.h);
}

void ekg_slider::set_height(float height) {
    this->set_size(this->rect.w, height);
}

void ekg_slider::set_value(float val) {
    if (this->value != val) {
        this->value = val;
        this->on_sync();
    }
}

float ekg_slider::get_value() {
    return this->value;
}

void ekg_slider::set_min_value(float min) {
    if (this->min_value != min) {
        this->min_value = min;
        this->on_sync();
    }
}

float ekg_slider::get_min_value() {
    return 0;
}

void ekg_slider::set_max_value(float max) {

}

float ekg_slider::get_max_value() {
    return 0;
}

void ekg_slider::set_size(float width, float height) {
    ekg_element::set_size(width, height);

    if (this->rect.w != width || this->rect.h != height) {
        this->rect.w = width;
        this->rect.h = height;

        this->on_sync();
    }
}

void ekg_slider::set_pos(float x, float y) {
    ekg_element::set_pos(x, y);
}

void ekg_slider::on_killed() {
    ekg_element::on_killed();
}

void ekg_slider::on_sync() {
    ekg_element::on_sync();
    ekg::core::instance.dispatch_todo_event(ekgutil::action::REFRESH);

    this->min_text_width = ekgfont::get_text_width("");
    this->min_text_height = ekgfont::get_text_height(std::to_string(this->value));
}

void ekg_slider::on_pre_event_update(SDL_Event &sdl_event) {
    ekg_element::on_pre_event_update(sdl_event);
}

void ekg_slider::on_event(SDL_Event &sdl_event) {
    ekg_element::on_event(sdl_event);
}

void ekg_slider::on_post_event_update(SDL_Event &sdl_event) {
    ekg_element::on_post_event_update(sdl_event);
}

void ekg_slider::on_draw_refresh() {
    ekg_element::on_draw_refresh();
}
