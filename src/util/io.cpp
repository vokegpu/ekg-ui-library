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

#include "ekg/ekg.hpp"
#include "ekg/util/io.hpp"

#include <fstream>

ekg::item &ekg::item::operator=(std::string_view _value) {
    this->set_value(_value);
    return *this;
}

ekg::item &ekg::item::operator=(const std::vector<std::string_view> &item_value_list) {
    this->child_list.clear();
    for (const std::string_view &values : item_value_list) {
        ekg::item &item {this->child_list.emplace_back()};
        item.set_value(values);
        item.p_item_parent = this;
    }

    return *this;
}

ekg::item &ekg::item::operator[](uint64_t it) {
    return this->child_list.at(it);
}

ekg::item::item(std::string_view _value) {
    this->set_value(_value);
}

void ekg::item::clear() {
    if (this->p_semaphore) *this->p_semaphore = true;
    return this->child_list.clear();
}

ekg::item &ekg::item::emplace_back() {
    ekg::item &item {this->child_list.emplace_back()};
    item.p_item_parent = this;
    item.p_semaphore = this->p_semaphore;
    return item;
}

void ekg::item::push_back(const ekg::item &item_copy) {
    ekg::item &item {this->child_list.emplace_back()};
    item = item_copy;
    item.p_item_parent = this;
    item.p_semaphore = this->p_semaphore;
}

void ekg::item::push_back(std::string_view item_value) {
    ekg::item &item {this->child_list.emplace_back()};
    item.set_value(item_value);
    item.p_item_parent = this;
    item.p_semaphore = this->p_semaphore;
}

void ekg::item::insert(const std::vector<std::string_view> &item_value_list) {
    for (const std::string_view &values : item_value_list) {
        ekg::item &item {this->child_list.emplace_back()};
        item.set_value(values);
        item.p_item_parent = this;
        item.p_semaphore = this->p_semaphore;
    }
}

void ekg::item::insert(uint16_t predicate, uint64_t index, const std::vector<std::string_view> &value_list) {
    if (this->child_list.size() <= index) {
        this->child_list.resize(index);
    }

    uint64_t it {};
    for (ekg::item &items : this->child_list) {
        if (it >= value_list.size()) {
            break;
        }

        if (ekg::bitwise::contains(items.attr, predicate)) {
            items.insert(index, value_list.at(it));
            it++;
        }
    }
}

ekg::item &ekg::item::insert(std::string_view item_value) {
    ekg::item &item {this->child_list.emplace_back()};
    item.set_value(item_value);
    item.p_item_parent = this;
    item.p_semaphore = this->p_semaphore;
    return item;
}

ekg::item &ekg::item::insert(const ekg::item &item_copy) {
    ekg::item &item {this->child_list.emplace_back()};
    item = item_copy;
    item.p_item_parent = this;
    item.p_semaphore = this->p_semaphore;
    return item;
}

ekg::item &ekg::item::insert(uint64_t index, std::string_view _value) {
    if (this->child_list.size() <= index) {
        this->child_list.resize(index + 1);
    }

    ekg::item &item {this->child_list.at(index)};
    item.set_value(_value);
    item.p_item_parent = this;
    item.p_semaphore = this->p_semaphore;
    return item;
}

ekg::item &ekg::item::insert(uint64_t index, const ekg::item &copy_item) {
    if (this->child_list.size() <= index) {
        this->child_list.resize(index + 1);
    }

    ekg::item &item {this->child_list.at(index)};
    item = copy_item;
    item.p_item_parent = this;
    item.p_semaphore = this->p_semaphore;
    return item;
}

ekg::item &ekg::item::at(uint64_t index) {
    return this->child_list.at(index);
}

bool ekg::item::empty() const {
    return this->child_list.empty();
}

uint64_t ekg::item::size() const {
    return this->child_list.size();
}

std::vector<ekg::item>::iterator ekg::item::begin() {
    return this->child_list.begin();
}

std::vector<ekg::item>::iterator ekg::item::end() {
    return this->child_list.end();
}

std::vector<ekg::item>::const_iterator ekg::item::cbegin() const {
    return this->child_list.cbegin();
}

std::vector<ekg::item>::const_iterator ekg::item::cend() const {
    return this->child_list.cend();
}

void ekg::item::set_value(std::string_view _value) {
    uint16_t attr_flags {};
    uint8_t start_index {ekg::check_attribute_flags(_value, attr_flags)};

    if (start_index) {
        _value = _value.substr(start_index, _value.size());
    }

    this->value = _value;
    this->attr = attr_flags;
}

bool ekg::item::has_children() {
    return !this->child_list.empty();
}

std::ostringstream ekg::log::buffer {};
bool ekg::log::buffered {};
bool ekg::log::tracked {};

bool ekg::bitwise::contains(uint16_t target, uint16_t flags) {
    return target & flags;
}

uint16_t &ekg::bitwise::remove(uint16_t &target, uint16_t flags) {
    target &= ~(flags);
    return target;
}

uint16_t &ekg::bitwise::add(uint16_t &target, uint16_t flags) {
    target |= flags;
    return target;
}

bool ekg::reach(ekg::timing &timing, uint64_t ms) {
    timing.ticks_going_on = SDL_GetTicks64();
    timing.current_ticks = timing.ticks_going_on - timing.elapsed_ticks;
    return timing.current_ticks > ms;
}

bool ekg::reset(ekg::timing &timing) {
    timing.elapsed_ticks = timing.ticks_going_on;
    return true;
}

bool ekg::file_to_string(std::string &file_content, std::string_view path) {
    std::ifstream ifs {path.data()};
    if (ifs.is_open()) {
        std::string line_content {};
        while (std::getline(ifs, line_content)) {
            file_content += line_content;
            file_content += '\n';
        }

        ifs.close();
        return false;
    } else {
        ekg::log() << "Failed to read file '" << path << "\'!";
    }

    return true;
}

bool ekg::input::action(std::string_view key_input) {
    return ekg::core->service_input.pressed(key_input);
}

void ekg::input::bind(std::string_view key_input, std::string_view value_input) {
    return ekg::core->service_input.bind(key_input, value_input);
}

void ekg::input::fire(std::string_view key_input) {
    ekg::core->service_input.fire(key_input);
}

void ekg::input::clipboard(ekg::clipboard clipboard_op) {
    SDL_Event sdl_event {};
    sdl_event.type = SDL_KEYDOWN;
    sdl_event.key.type = SDL_KEYDOWN;
    sdl_event.key.keysym.sym = static_cast<uint64_t>(clipboard_op);
    SDL_PushEvent(&sdl_event);

    // should push an event to keyup the previous clipboard operator?
}

bool ekg::input::motion() {
    return ekg::core->service_input.has_motion;
}

bool ekg::input::released() {
    return ekg::core->service_input.was_released;
}

bool ekg::input::pressed() {
    return ekg::core->service_input.was_pressed;
}

bool ekg::input::wheel() {
    return ekg::core->service_input.was_wheel;
}

bool ekg::input::receive(std::string_view tag) {
    return ekg::core->service_input.receive(tag);
}

bool ekg::input::typed() {
    return ekg::core->service_input.was_typed;
}

ekg::vec4 &ekg::input::interact() {
    return ekg::core->service_input.interact;
}