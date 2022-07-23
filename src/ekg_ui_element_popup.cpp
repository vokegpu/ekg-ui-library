/**
 * EKG-LICENSE - this software/library LICENSE can not be modified in any instance.
 *
 * --
 * ANY NON-OFFICIAL MODIFICATION IS A CRIME.
 * DO NOT SELL THIS CODE SOFTWARE, FOR USE EKG IN A COMMERCIAL PRODUCT ADD EKG-LICENSE TO PROJECT,
 * RESPECT THE COPYRIGHT TERMS OF EKG, NO SELL WITHOUT EKG-LICENSE (IT IS A CRIME).
 * DO NOT FORK THE PROJECT SOURCE WITHOUT EKG-LICENSE.
 *
 * END OF EKG-LICENSE.
 **/
#include <ekg/impl/ekg_ui_element_popup.hpp>
#include <ekg/api/ekg_api.hpp>

void ekg_popup::set_opened(bool opened) {
    ekgapi::set(this->flag.old_focused, this->flag.focused, opened);
}

bool ekg_popup::is_opened() {
    return this->flag.focused;
}

float ekg_popup::get_min_text_width() {
    return this->component_text_min_width;
}

float ekg_popup::get_min_text_height() {
    return this->component_text_min_height;
}

void ekg_popup::set_width(float width) {
    this->set_size(width, this->rect.h);
}

void ekg_popup::set_height(float height) {
    this->set_size(this->rect.w, height);
}

void ekg_popup::set_text_dock(uint16_t dock) {
    if (this->enum_flags_text_component_dock != dock) {
        this->enum_flags_text_component_dock = dock;
        this->on_sync();
    }
}

uint16_t ekg_popup::get_text_dock() {
    return this->enum_flags_text_component_dock;
}

void ekg_popup::add(const std::vector<std::string> &vec) {
    ekgutil::component component;

    for (const std::string &components_text : vec) {
        component.text = components_text;
        this->component_list.push_back(component);
    }

}

void ekg_popup::remove(const std::string &pattern) {
    const std::vector<ekgutil::component> copy = this->component_list;
    this->component_list.clear();

    for (const ekgutil::component &component : copy) {
        if (ekgutil::contains(pattern, component.text)) {
            continue;
        }

        this->component_list.push_back(component);
    }
}

void ekg_popup::state(const std::string &pattern, bool enabled) {
    for (ekgutil::component &component : this->component_list) {
        if (ekgutil::contains(pattern, component.text)) {
            component.enabled = enabled;
        }
    }
}

void ekg_popup::set_size(float width, float height) {
    ekg_element::set_size(width, height);

    if (this->rect.w != width || this->rect.h != height) {
        this->rect.w = width;
        this->rect.h = height;
        this->on_sync();
    }
}

void ekg_popup::set_pos(float x, float y) {
    ekg_element::set_pos(x, y);
}

void ekg_popup::on_sync() {
    ekg_element::on_sync();
}

void ekg_popup::on_pre_event_update(SDL_Event &sdl_event) {
    ekg_element::on_pre_event_update(sdl_event);
}

void ekg_popup::on_event(SDL_Event &sdl_event) {
    ekg_element::on_event(sdl_event);
}

void ekg_popup::on_post_event_update(SDL_Event &sdl_event) {
    ekg_element::on_post_event_update(sdl_event);
}

void ekg_popup::on_update() {
    ekg_element::on_update();
}

void ekg_popup::on_draw_refresh() {
    ekg_element::on_draw_refresh();
}
