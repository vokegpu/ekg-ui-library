/*
 * MIT License
 * 
 * Copyright (c) 2022-2023 Rina Wilk / vokegpu@gmail.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "ekg/ui/abstract/ui_abstract.hpp"
#include "ekg/ekg.hpp"

ekg::ui::abstract::abstract() {

}

ekg::ui::abstract::~abstract() {
}

ekg::ui::abstract *ekg::ui::abstract::add_child(int32_t element_id) {
    bool contains {};
    ekg::ui::abstract_widget *widget {};

    for (int32_t &element_ids : this->child_id_list) {
        if ((contains = element_ids == element_id)) {
            break;
        }
    }

    if (contains == false && (widget = ekg::core->get_fast_widget_by_id(element_id)) != nullptr) {        
        this->child_id_list.push_back(element_id);
        widget->data->set_parent_id(this->id);
        widget->parent = &this->rect_widget;
    }

    return this;
}

std::vector<int32_t> &ekg::ui::abstract::get_child_id_list() {
    return this->child_id_list;
}

ekg::ui::abstract *ekg::ui::abstract::remove_child(int32_t element_id) {
    bool contains {};
    ekg::ui::abstract_widget *widget {nullptr};
    uint64_t it {};

    for (it = 0; it < this->child_id_list.size(); it++) {
        if ((contains = this->child_id_list.at(it) == element_id)) {
            break;
        }
    }

    if (contains == false && (widget = ekg::core->get_fast_widget_by_id(element_id)) != nullptr) {
        this->child_id_list.erase(this->child_id_list.begin() + it);
        widget->data->set_parent_id(0);
        widget->parent = &widget->empty_parent;
    }

    return this;
}

ekg::ui::abstract *ekg::ui::abstract::set_id(int32_t element_id) {
    this->id = element_id;\
    return this;
}

int32_t ekg::ui::abstract::get_id() {
    return this->id;
}

ekg::ui::abstract *ekg::ui::abstract::set_parent_id(int32_t element_id) {
    this->parent_id = element_id;
    return this;
}

int32_t ekg::ui::abstract::get_parent_id() {
    return this->parent_id;
}

ekg::ui::abstract *ekg::ui::abstract::set_alive(bool element_alive) {
    this->alive = element_alive;
    return this;
}

bool ekg::ui::abstract::is_alive() {
    return this->alive;
}

void ekg::ui::abstract::destroy() {
    this->set_alive(false);
    ekg::refresh(this->id);

    for (int32_t &ids : this->child_id_list) {
        auto widget {ekg::core->get_fast_widget_by_id(ids)};
        if (widget != nullptr && widget->data != nullptr) widget->data->destroy();
    }
}

ekg::ui::abstract *ekg::ui::abstract::set_state(const ekg::state &enum_state) {
    this->state = enum_state;
    return this;
}

ekg::state ekg::ui::abstract::get_state() {
    return this->state;
}

ekg::ui::abstract *ekg::ui::abstract::set_type(const ekg::type &enum_type) {
    this->type = enum_type;
    return this;
}

ekg::type ekg::ui::abstract::get_type() {
    return this->type;
}

uint16_t ekg::ui::abstract::get_place_dock() {
    return this->dock_flags;
}

ekg::rect &ekg::ui::abstract::widget() {
    return this->rect_widget;
}

ekg::rect &ekg::ui::abstract::ui() {
    return this->sync_ui;
}

ekg::ui::abstract *ekg::ui::abstract::set_tag(std::string_view string) {
    this->tag = string;
    return this;
}

std::string_view ekg::ui::abstract::get_tag() {
    return this->tag;
}

uint16_t &ekg::ui::abstract::get_sync() {
    return this->sync_flags;
}

void ekg::ui::abstract::reset() {
    ekg::bitwise::add(this->sync_flags, (uint16_t) ekg::uisync::reset);
    ekg::reload(this->id);
}

ekg::ui::abstract *ekg::ui::abstract::set_scaled_height_layout(int32_t scaled_height_int) {
    this->scaled_height = scaled_height_int;
    return this;
}

bool ekg::ui::abstract::has_parent() {
    return this->parent_id != 0;
}

bool ekg::ui::abstract::has_children() {
    return !this->child_id_list.empty();
}
