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

#include "ekg/ui/popup/ui_popup.hpp"
#include "ekg/util/gui.hpp"

int64_t ekg::ui::popup::contains(std::string_view component_name) {
    for (uint32_t it {}; it < this->component_list.size(); it++) {
        if (this->component_list.at(it).name == component_name) {
            return it;
        }
    }

    return -1;
}

ekg::ui::popup *ekg::ui::popup::append(const std::vector<std::string> &component_name_list) {
    for (const std::string &component_name : component_name_list) {
        this->append(component_name);
    }

    return this;
}

ekg::ui::popup *ekg::ui::popup::append(std::string_view component_name) {
    std::string factored_component_name {component_name};
    bool is_separator {component_name.size() >= 3 &&
                                 component_name.at(0) == '-' &&
                                 component_name.at(1) == '-' &&
                                 component_name.at(2) == '-'
    };

    if (is_separator) {
        factored_component_name = factored_component_name.substr(3, factored_component_name.size());
    }

    if (this->contains(factored_component_name) != -1) {
        return this;
    }

    ekg::ui::component component {};
    component.name = factored_component_name;
    component.attributes = is_separator ? ekg::attribute::separator : 0;
    this->component_list.push_back(component);
    return this;
}

ekg::ui::popup *ekg::ui::popup::append_linked(std::string_view component_name, ekg::ui::popup *popup_linked) {
    int64_t index {this->contains(component_name)};

    if (index == -1) {
        this->append(component_name);
    }

    auto &component {this->component_list.at(index)};
    component.linked_id = 0;

    if (popup_linked != nullptr) {
        popup_linked->set_state(ekg::state::invisible);
        component.linked_id = popup_linked->get_id();
        this->add_child(component.linked_id);
    }

    return this;
}

ekg::ui::popup *ekg::ui::popup::remove(std::string_view component_name) {
    std::vector<ekg::ui::component> new_list {};
    for (ekg::ui::component &component : this->component_list) {
        if (component.name == component_name) {
            this->remove_child(component.linked_id);
            continue;
        }

        new_list.push_back(component);
    }

    this->component_list.clear();
    this->component_list.insert(this->component_list.end(), new_list.begin(), new_list.end());

    return this;
}

ekg::ui::component &ekg::ui::popup::get(uint64_t index) {
    return this->component_list.at(index);
}

std::vector<ekg::ui::component> &ekg::ui::popup::get_component_list() {
    return this->component_list;
}

ekg::ui::popup *ekg::ui::popup::set_text_align(uint16_t flags) {
    if (this->dock_flags != flags) {
        this->dock_flags = flags;
        ekg::reload(this->id);
    }

    return this;
}

uint16_t ekg::ui::popup::get_text_align() {
    return this->dock_flags;
}

ekg::ui::popup *ekg::ui::popup::set_width(float width) {
    if (this->sync_ui.w != width) {
        this->sync_ui.w = width;

        ekg::bitwise::add(this->sync_flags, (uint16_t) ekg::uisync::dimension);
        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }

    return this;
}

float ekg::ui::popup::get_width() {
    return this->rect_widget.w;
}

ekg::ui::popup *ekg::ui::popup::set_scaled_height(int32_t scaled_height_factor) {
    if (this->scaled_height != scaled_height_factor) {
        this->scaled_height = scaled_height_factor;

        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }

    return this;
}

int32_t ekg::ui::popup::get_scaled_height() {
    return this->scaled_height;
}

float ekg::ui::popup::get_height() {
    return this->rect_widget.h;
}

ekg::ui::popup *ekg::ui::popup::set_font_size(ekg::font f_size) {
    if (this->font_size != f_size) {
        this->font_size = f_size;

        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }

    return this;
}

ekg::font ekg::ui::popup::get_font_size() {
    return this->font_size;
}

ekg::ui::popup *ekg::ui::popup::set_pos(float x, float y) {
    if (this->sync_ui.x != x || this->sync_ui.y != y) {
        this->sync_ui.x = x;
        this->sync_ui.y = y;

        ekg::bitwise::add(this->sync_flags, (uint16_t) ekg::uisync::dimension);
        ekg::reload(this->id);
    }

    return this;
}

ekg::vec2 ekg::ui::popup::get_pos() {
    return {this->rect_widget.x, this->rect_widget.y};
}