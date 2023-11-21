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

bool ekg::item::default_semaphore {};

ekg::item &ekg::item::operator[](uint64_t it) {
    return this->child_list.at(it);
}

ekg::item &ekg::item::at(uint64_t index) {
    return this->child_list.at(index);
}

ekg::item &ekg::item::emplace_back() {
    ekg::item &item {this->child_list.emplace_back()};
    item.p_parent = this;
    item.p_semaphore = this->p_semaphore;
    return item;
}

ekg::item &ekg::item::insert(std::string_view item_value) {
    ekg::item &item {this->emplace_back()};
    item.set_value(item_value);
    return item;
}

ekg::item &ekg::item::insert(std::string_view item_value, size_t index) {
    return this->insert({item_value}, index);
}

ekg::item &ekg::item::insert(const ekg::item &insert_item) {
    ekg_sign_item_sempahore();
    ekg::item &item {this->child_list.emplace_back()};
    item = insert_item;
    item.p_parent = this;
    item.p_semaphore = this->p_semaphore;
    return item;
}

ekg::item &ekg::item::insert(const ekg::item &insert_item, size_t index) {
    ekg_sign_item_sempahore();
    ekg::item &item {this->child_list.insert(this->child_list.begin() + index)};
    item = insert_item;
    item.p_parent = this;
    item.p_semaphore = this->p_semaphore;
    return item;
}

ekg::item &ekg::item::set_value(size_t index, std::string_view item_value) {
    ekg_sign_item_sempahore();
    ekg::item &item {this->child_list.at(index)};
    item.set_value(item_value);
    return item;
}

ekg::item &ekg::item::set_attr(size_t index, uint16_t attr_bits) {
    ekg_sign_item_sempahore();
    ekg::item &item {this->child_list.at(index)};
    item.set_attr(attr_bits);
    return item;
}

ekg::item &ekg::item::set_state(size_t index, uint16_t state_bits) {
    ekg_sign_item_sempahore();
    ekg::item &item {this->child_list.at(index)};
    item.set_attr(index, state_bits);
    return item;
}

ekg::item &ekg::item::erase(size_t begin, size_t end) {
    ekg_sign_item_sempahore();
    this->child_list.erase(this->child_list.begin() + begin, this->child_list.end() + end);    
    return *this;
}

ekg::item &ekg::item::erase(size_t index) {
    ekg_sign_item_sempahore();
    this->child_list.erase(this->child_list.begin() + index);
    return *this;
}

ekg::item &ekg::item::clear() {
    ekg_sign_item_sempahore();
    this->child_list.clear();
    return *this;
}

bool ekg::item::empty() const {
    return this->child_list.empty();
}

size_t ekg::item::size() const {
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

ekg::item() {
    this->semaphore = &ekg::item::default_semaphore;
}

ekg::item(std::string_view item_value) {
    this->set_value(item_value);
    this->p_semaphore = &ekg::item::default_semaphore;
}

ekg::item(std::string_view item_value, uint16_t attr_bits) {
    this->value = item_value;
    this->attributes = attr_bits;
    this->semaphore = &ekg::item::default_semaphore;
}

ekg::item &ekg::item::set_value(std::string_view item_value) {
    uint16_t attr_bits {};
    uint8_t start_index {ekg::check_attribute_flags(item_value, attr_bits)};

    if (start_index) {
        item_value = item_value.substr(start_index, item_value.size());
    }

    this->value = item_value;
    this->attributes = attr_flags;
    return *this;
}

std::string_view ekg::item::get_value() {
    return this->value;
}

ekg::item &ekg::item::set_state(uint16_t state_bits) {
    if (this->states != state_bits) {
        *this->p_semaphore = true;
    }

    return *this;
}

ekg::item &ekg::item::set_semaphore_address(bool *p_addres) {
    this->p_semaphore = p_addres;
    return *this;
}

ekg::item &ekg::item::set_semaphore(bool signal) {
    if (this->p_semaphore) {
        *this->p_semaphore = signal;
    }

    return *this;
}

bool &ekg::item::get_semaphore() {
    return *this->p_semaphore; 
}

bool ekg::item::has_children() {
    return !this->child_list.empty();
}

bool ekg::item::has_parent() {
    return this->p_parent != nullptr;
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