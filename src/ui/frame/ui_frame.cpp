/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, free to use in anything (utility, hacking, cheat, game, software).
 * 5- Malware, rat and others virus. We do not care.
 * 6- Do not modify this license under any instance.
 *
 * @VokeGpu 2023 all rights reserved.
 */

#include "ekg/ui/frame/ui_frame.hpp"
#include "ekg/util/util_event.hpp"
#include "ekg/util/util_ui.hpp"

void ekg::ui::frame::set_place(uint16_t _dock) {
    if (this->dock_flags != _dock) {
        this->dock_flags = _dock;

        ekg::reload(this->id);
        ekg::synclayout(this->id);
    }
}

void ekg::ui::frame::set_scale_factor(float x, float y) {
    this->scale_factor.x = x;
    this->scale_factor.y = y;
}

ekg::vec2 ekg::ui::frame::get_scale_factor() {
    return this->scale_factor;
}

ekg::ui::frame *ekg::ui::frame::set_drag(uint16_t dock) {
	this->dock_drag = dock;
    return this;
}

uint16_t ekg::ui::frame::get_drag_dock() {
	return this->dock_drag;
}

ekg::ui::frame *ekg::ui::frame::set_resize(uint16_t dock) {
	this->dock_resize = dock;
    return this;
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

void ekg::ui::frame::set_size(float w, float h) {
    if (this->sync_ui.w != w || this->sync_ui.h != h) {
        this->sync_ui.w = w;
        this->sync_ui.h = h;

        ekg::bitwise::add(this->sync_flags, (uint16_t) ekg::uisync::dimension);
        ekg::reload(this->id);
        ekg::synclayout(this->id);
        ekg::dispatch(ekg::env::redraw);
    }
}

ekg::vec2 ekg::ui::frame::get_size() {
	return {this->rect_widget.w, rect_widget.h};
}

void ekg::ui::frame::set_pos(float x, float y) {
    if (this->sync_ui.x != x || this->sync_ui.y != y) {
        this->sync_ui.x = x;
        this->sync_ui.y = y;

        ekg::bitwise::add(this->sync_flags, (uint16_t) ekg::uisync::dimension);
        ekg::reload(this->id);
        ekg::dispatch(ekg::env::redraw);
    }
}

ekg::vec2 ekg::ui::frame::get_pos() {
	return {this->rect_widget.x, this->rect_widget.y};
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

        ekg::bitwise::add(this->sync_flags, (uint16_t) ekg::uisync::dimension);
        ekg::reload(this->id);
        ekg::synclayout(this->id);
        ekg::dispatch(ekg::env::redraw);
    }
}

float ekg::ui::frame::get_width() {
    return this->rect_widget.w;
}

void ekg::ui::frame::set_height(float height) {
    if (this->sync_ui.h != height) {
        this->sync_ui.h = height;

        ekg::bitwise::add(this->sync_flags, (uint16_t) ekg::uisync::dimension);
        ekg::reload(this->id);
        ekg::synclayout(this->id);
        ekg::dispatch(ekg::env::redraw);
    }
}

float ekg::ui::frame::get_height() {
    return this->rect_widget.h;
}