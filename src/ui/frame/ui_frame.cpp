#include "ekg/ui/frame/ui_frame.hpp"

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
	this->initial_pos.x = x;
	this->initial_pos.y = y;
}

ekg::vec2 ekg::ui::frame::get_pos_initial() {
	return this->initial_pos;
}

void ekg::ui::frame::set_size_initial(float w, float h) {
	this->initial_size.x = w;
	this->initial_size.y = h;
}

ekg::vec2 ekg::ui::frame::get_size_initial() {
	return this->initial_size;
}

void ekg::ui::frame::parent(uint32_t token) {
	bool not_contains {true};

	for (uint32_t &tokens : this->parent_id_list) {
		if (!(not_contains = tokens != token)) {
			break;
		}
	}

	if (not_contains) {
		this->parent_id_list.push_back(token);
	} 
}

std::vector<uint32_t> &ekg::ui::frame::get_parent_id_list() {
	return this->parent_id_list;
}

void ekg::ui::frame::set_embed(uint16_t dock) {
	this->dock_embed = dock;
}

uint16_t ekg::ui::frame::get_embed_dock() {
	return this->dock_embed;
}

void ekg::ui::frame::set_size(float w, float h) {
	this->size.x = w;
	this->size.y = h;
}

ekg::vec2 ekg::ui::frame::get_size() {
	return this->size;
}

void ekg::ui::frame::set_pos(float x, float y) {
	this->pos.x = x;
	this->pos.y = y;
}

ekg::vec2 ekg::ui::frame::get_pos() {
	return this->pos;
}