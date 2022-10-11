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
    if (this->initial_pos.x != x || this->initial_size.y != y) {
        this->initial_pos.x = x;
        this->initial_pos.y = y;

        ekg::reset(this->id);
        ekg::update(this->id);
    }
}

ekg::vec2 ekg::ui::frame::get_pos_initial() {
	return this->initial_pos;
}

void ekg::ui::frame::set_size_initial(float w, float h) {
    if (this->initial_size.x != w || this->initial_size.y != h) {
        this->initial_size.x = w;
        this->initial_size.y = h;

        ekg::reset(this->id);
        ekg::update(this->id);
    }
}

ekg::vec2 ekg::ui::frame::get_size_initial() {
	return this->initial_size;
}

void ekg::ui::frame::set_embed(uint16_t dock) {
	this->dock_embed = dock;
}

uint16_t ekg::ui::frame::get_embed_dock() {
	return this->dock_embed;
}

void ekg::ui::frame::set_size(float w, float h) {
    if (this->size.x != w || this->size.y != h) {
        this->size.x = w;
        this->size.y = h;
        ekg::update(this->id);
    }
}

ekg::vec2 ekg::ui::frame::get_size() {
	return this->size;
}

void ekg::ui::frame::set_pos(float x, float y) {
    if (this->pos.x != x || this->pos.y != y) {
        this->pos.x = x;
        this->pos.y = y;
        ekg::update(this->id);
    }
}

ekg::vec2 ekg::ui::frame::get_pos() {
	return this->pos;
}

void ekg::ui::frame::set_tag(std::string_view new_tag) {
    this->tag = new_tag;
}

std::string_view ekg::ui::frame::get_tag() {
    return this->tag;
}
