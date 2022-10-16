#include "ekg/ui/frame/ui_frame.hpp"
#include "ekg/util/util_ui.hpp"
#include "ekg/util/util_event.hpp"

void ekg::ui::frame::set_drag(uint16_t dock) {
	this->dock_drag = dock;
}

uint16_t ekg::ui::frame::get_drag_dock() {
	return this->dock_drag;
}

void ekg::ui::frame::set_resize(uint16_t dock) {
	this->dock_resize = dock;
}

uint16_t ekg::ui::frame::get_resize_dock() {
	return this->dock_resize;
}

void ekg::ui::frame::set_pos_initial(float x, float y) {
    this->rect_initial.x = x;
    this->rect_initial.y = y;
}

ekg::vec2 ekg::ui::frame::get_pos_initial() {
	return {this->rect_initial.x, this->rect_initial.y};
}

void ekg::ui::frame::set_size_initial(float w, float h) {
    this->rect_initial.w = w;
    this->rect_initial.h = h;
}

ekg::vec2 ekg::ui::frame::get_size_initial() {
	return {this->rect_initial.w, this->rect_initial.h};
}

void ekg::ui::frame::set_embed(uint16_t dock) {
	this->dock_embed = dock;
}

uint16_t ekg::ui::frame::get_embed_dock() {
	return this->dock_embed;
}

void ekg::ui::frame::set_size(float w, float h) {
    if (this->sync_ui.w != w || this->sync_ui.h != h) {
        this->sync_ui.w = w;
        this->sync_ui.h = h;
        this->set_sync_with_ui(true);
    }
}

ekg::vec2 ekg::ui::frame::get_size() {
	return {this->rect_widget.w, rect_widget.h};
}

void ekg::ui::frame::set_pos(float x, float y) {
    if (this->sync_ui.x != x || this->sync_ui.y != y) {
        this->sync_ui.x = x;
        this->sync_ui.y = y;
        this->set_sync_with_ui(true);
    }
}

ekg::vec2 ekg::ui::frame::get_pos() {
	return {this->rect_widget.x, this->rect_widget.y};
}

void ekg::ui::frame::set_tag(std::string_view new_tag) {
    this->tag = new_tag;
}

std::string_view ekg::ui::frame::get_tag() {
    return this->tag;
}

void ekg::ui::frame::set_initial_width(float width) {
    this->rect_initial.w = width;
}

float ekg::ui::frame::get_initial_width() {
    return this->rect_initial.w;
}

void ekg::ui::frame::set_initial_height(float height) {
    this->rect_initial.h = height;
}

float ekg::ui::frame::get_initial_height() {
    return this->rect_initial.h;
}

void ekg::ui::frame::set_width(float width) {
    if (this->sync_ui.w != width) {
        this->sync_ui.w = width;
        this->set_sync_with_ui(true);
    }
}

float ekg::ui::frame::get_width() {
    return this->rect_widget.w;
}

void ekg::ui::frame::set_height(float height) {
    if (this->sync_ui.h != height) {
        this->sync_ui.h = height;
        this->set_sync_with_ui(true);
    }
}

float ekg::ui::frame::get_height() {
    return this->rect_widget.h;
}
