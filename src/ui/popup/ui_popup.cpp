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
 * @VokeGpu 2022 all rights reserved.
 */

#include "ekg/ui/popup/ui_popup.hpp"
#include "ekg/util/util_ui.hpp"

bool ekg::ui::popup::contains(std::string_view component_name) {
    return this->registered_component_map[component_name.data()];
}

void ekg::ui::popup::append(const std::vector<std::string> &component_name_list) {
    for (const std::string &component_name : component_name_list) {
        if (this->contains(component_name)) {
            continue;
        }

        ekg::component component {};
        component.id = ++this->token_id;
        component.name = component_name;

        this->registered_component_map[component_name] = this->component_list.size();
        this->component_list.push_back(component);
    }
}

void ekg::ui::popup::append(std::string_view component_name) {
    if (this->contains(component_name)) {
        return;
    }

    ekg::component component {};
    component.id = this->token_id;
    component.name = component_name;

    this->registered_component_map[component_name.data()] = true;
    this->component_list.push_back(component);
}

void ekg::ui::popup::append_linked(std::string_view component_name, ekg::ui::popup *popup_linked) {
    int32_t index {this->registered_component_map[component_name.data()]};

    if (!index) {
        this->append(component_name);
        index = this->registered_component_map[component_name.data()];
    }

    auto &component {this->component_list.at(index)};
    component.linked_id = popup_linked == nullptr ? 0 : popup_linked->get_id();
}

void ekg::ui::popup::remove(std::string_view component_name) {
    std::vector<ekg::component> new_list {};
    this->registered_component_map.clear();
    this->token_id = 0;

    for (ekg::component &component : this->component_list) {
        if (component.name == component_name) {
            continue;
        }

        component.id = ++this->token_id;
        new_list.push_back(component);
        this->registered_component_map[component.name] = this->token_id;
    }
}

void ekg::ui::popup::set_place(uint16_t flags) {
    if (this->dock_flags != flags) {
        this->dock_flags = flags;
        ekg::synclayout(this->id);
    }
}

void ekg::ui::popup::set_text_align(uint16_t flags) {
    if (this->dock_flags != flags) {
        this->dock_flags = flags;
        ekg::reload(this->id);
    }
}

uint16_t ekg::ui::popup::get_text_align() {
    return this->dock_flags;
}

void ekg::ui::popup::set_width(float width) {
    if (this->sync_ui.w != width) {
        this->sync_ui.w = width;

        ekg::bitwise::add(this->sync_flags, (uint16_t) ekg::uisync::dimension);
        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
        ekg::scissor(this->parent_id);
    }
}

float ekg::ui::popup::get_width() {
    return this->rect_widget.w;
}

void ekg::ui::popup::set_scaled_height(int32_t scaled_height_factor) {
    if (this->scaled_height != scaled_height_factor) {
        this->scaled_height = scaled_height_factor;

        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
        ekg::scissor(this->parent_id);
    }
}

int32_t ekg::ui::popup::get_scaled_height() {
    return this->scaled_height;
}

float ekg::ui::popup::get_height() {
    return this->rect_widget.h;
}