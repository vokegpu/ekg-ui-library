#include <ekg/impl/ekg_ui_element_frame.hpp>
#include <ekg/api/ekg_gpu.hpp>
#include <ekg/ekg.hpp>

void ekg_frame::on_sync() {
    ekg_element::on_sync();

    this->rect.w = this->rect.w < this->min_width ? this->min_width : this->rect.w;
    this->rect.h = this->rect.h < this->min_height ? this->min_height : this->rect.h;

    ekg::core::instance.dispatch_todo_event(ekgutil::action::REFRESH);
}

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

    ekgmath::vec4f color(255, 255, 255, 200);
    ekggpu::rectangle(this->rect, color);
}

void ekg_frame::set_min_width(float min) {
    this->min_width = min < 30 ? 30 : min;
}

float ekg_frame::get_min_width() {
    return this->min_width;
}

void ekg_frame::set_min_height(float min) {
    this->min_height = min < 30 ? 30 : min;
}

float ekg_frame::get_min_height() {
    return this->min_height;
}

void ekg_frame::set_width(float width) {
    width = width < this->min_width ? this->min_width : width;

    if (this->rect.w != width) {
        this->rect.w = width;
        this->on_sync();
    }
}

void ekg_frame::set_height(float height) {
    height = height < this->min_height ? this->min_height : height;

    if (this->rect.h != height) {
        this->rect.h = height;
        this->on_sync();
    }
}

void ekg_frame::set_tag(std::string &string) {
    this->tag = string;
}

std::string ekg_frame::get_tag() {
    return this->tag;
}

void ekg_frame::set_size(float width, float height) {
    width = width < this->min_width ? this->min_width : width;
    height = height < this->min_height ? this->min_height : height;

    if (this->rect.w != width || this->rect.h != height) {
        this->rect.w = width;
        this->rect.h = height;
        this->on_sync();
    }
}

void ekg_frame::set_pos(float x, float y) {
    if (this->rect.x != x || this->rect.y != y) {
        this->rect.x = x;
        this->rect.y = y;
        this->on_sync();
    }
}
