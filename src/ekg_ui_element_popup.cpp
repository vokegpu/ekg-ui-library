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
#include <ekg/ekg.hpp>

ekg_popup::ekg_popup() {
    this->set_opened(true);
    this->type = ekg::ui::POPUP;
}

ekg_popup::~ekg_popup() {

}

void ekg_popup::set_opened(bool opened) {
    ekgapi::set(this->flag.focused, opened);
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
    this->set_size(width, this->component_height);
}

void ekg_popup::set_height(float height) {
    if (this->component_height != height) {
        this->component_height = height;
        this->on_sync();
    }
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
        component.enabled = true;
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

    if (this->rect.w != width || this->component_height != height) {
        this->rect.w = width;
        this->component_height = height;
        this->on_sync();
    }
}

void ekg_popup::set_pos(float x, float y) {
    ekg_element::set_pos(x, y);
}

void ekg_popup::on_sync() {
    ekg_element::on_sync();
    the_ekg_core->dispatch_todo_event(ekgutil::action::REFRESH);

    float max_width = 0.0f;
    float max_height = 0.0f;

    bool center = ekgutil::contains(this->enum_flags_text_component_dock, ekg::dock::CENTER);
    bool top = ekgutil::contains(this->enum_flags_text_component_dock, ekg::dock::TOP);
    bool bottom = ekgutil::contains(this->enum_flags_text_component_dock, ekg::dock::BOTTOM);
    bool left = ekgutil::contains(this->enum_flags_text_component_dock, ekg::dock::LEFT);
    bool right = ekgutil::contains(this->enum_flags_text_component_dock, ekg::dock::RIGHT);

    this->full_height = this->offset_separator;

    // First iteration for get real the most large string width.
    for (ekgutil::component &component : this->component_list) {
        max_width = ekgfont::get_text_width(component.text);
        max_height = ekgfont::get_text_height(component.text);

        if (max_width > this->component_text_min_width) {
            this->component_text_min_width = max_width;
        }

        if (max_height > this->component_text_min_height) {
            this->component_text_min_height = max_height;
        }

        component.w = max_width;
        component.h = max_height;
    }

    this->rect.w = this->rect.w < this->component_text_min_width ? this->component_text_min_width : this->rect.w;
    this->rect.h = this->rect.h < this->component_text_min_height ? this->component_text_min_height : this->rect.h;

    // Second iteration to set the real offset positions.
    for (ekgutil::component &component : this->component_list) {
        max_width = component.w;
        max_height = component.h;

        if (center) {
            this->component_text_offset_x = (this->rect.w / 2) - (max_width / 2);
            this->component_text_offset_y = (this->component_height / 2) - (max_height / 2);
        }

        if (top) {
            this->component_text_offset_y = max_height / 2;
        }

        if (left) {
            this->component_text_offset_x = max_width / 2;
        }

        if (right) {
            this->component_text_offset_x = this->rect.w - max_width - (max_width / 4);
        }

        if (bottom) {
            this->component_text_offset_y = this->component_height - max_height - (max_height / 4);
        }

        component.x = this->rect.x + this->component_text_offset_x;
        component.y = this->rect.y + this->full_height + this->component_text_offset_y;
        component.w = this->rect.w;
        component.h = this->component_height;

        this->full_height += this->component_height + this->offset_separator;
    }
}

void ekg_popup::on_pre_event_update(SDL_Event &sdl_event) {
    ekg_element::on_pre_event_update(sdl_event);

    float mx = 0;
    float my = 0;

    if (ekgapi::motion(sdl_event, mx, my)) {
        ekgapi::set_direct(this->flag.over, this->rect.collide_aabb_with_point(mx, my));
    }
}

void ekg_popup::on_event(SDL_Event &sdl_event) {
    ekg_element::on_event(sdl_event);

    float mx = 0;
    float my = 0;

    bool highlight = false;

    if (ekgapi::motion(sdl_event, mx, my)) {
        highlight = this->flag.over;
    } else if (ekgapi::input_down_left(sdl_event, mx, my)) {
        bool flag = !this->flag.over;
        this->contains(flag, the_ekg_core->get_hovered_element_id());

        if (flag) {
            ekgapi::set(this->flag.focused, false);
        }

        ekgapi::set_direct(this->flag.activy, this->flag.highlight);

        if (this->flag.highlight && this->flag.over) {
            this->activy_component = this->focused_component;
        }

        highlight = this->flag.over;
    } else if (ekgapi::any_input_up(sdl_event, mx, my)) {
        if (this->flag.activy && this->flag.over) {
            this->activy_component = this->focused_component;
            ekgapi::set(this->flag.activy, false);
        }
    }

    if (highlight) {
        this->full_height = this->offset_separator;

        for (ekgutil::component &component : this->component_list) {
            if (ekgmath::collide_aabb_with_point(this->rect.x, this->rect.y + this->full_height, component.w, component.h) && component.enabled) {
                ekgapi::set(this->flag.highlight, true);
                ekgapi::set(this->focused_component, component.text);
            }

            this->full_height += component.h + this->offset_separator;
        }
    }
}

void ekg_popup::on_post_event_update(SDL_Event &sdl_event) {
    ekg_element::on_post_event_update(sdl_event);

    float mx = 0;
    float my = 0;

    if (ekgapi::motion(sdl_event, mx, my)) {
        ekgapi::set_direct(this->flag.over, false);
    }
}

void ekg_popup::on_update() {
    ekg_element::on_update();
}

void ekg_popup::on_draw_refresh() {
    ekg_element::on_draw_refresh();
    float height_scaled = this->offset_separator;

    // TODO open animation.
    if (this->rect.h != this->full_height && this->flag.focused) {
        this->rect.h = this->full_height;
        this->on_sync();
    } else if (!this->flag.focused && this->visibility != ekg::visibility::LOW_PRIORITY) {
        this->kill();
    }

    // Background.
    ekggpu::rectangle(this->rect, ekg::theme().popup_background);

    for (ekgutil::component &component : this->component_list) {
        if (this->flag.highlight && component.text == this->focused_component) {
            ekggpu::rectangle(this->rect.x, this->rect.y + height_scaled, component.w, component.h, ekg::theme().popup_highlight);
        }

        // Render text.
        ekgfont::render(component.text, component.x,component.y, component.enabled ? ekg::theme().string_enabled_color : ekg::theme().string_disabled_color);

        // Add height by iteration.
        height_scaled += component.h + this->offset_separator;
    }
}

void ekg_popup::set_offset_separator(float offset) {
    if (this->offset_separator != offset) {
        this->offset_separator = offset;
        this->on_sync();
    }
}

float ekg_popup::get_offset_separator() {
    return this->offset_separator;
}

void ekg_popup::set_tag(const std::string &str) {
    this->tag = str;
}

std::string ekg_popup::get_tag() {
    return this->tag;
}

void ekg_popup::place(const std::string &component_name, ekg_popup *popup) {
    if (popup == nullptr || popup->get_id() == this->id || this->children_stack.contains(popup->get_id())) {
        return;
    }

    this->children_stack.add(popup->get_id());
    popup->set_visibility(ekg::visibility::LOW_PRIORITY);

    int32_t id = this->get_component_index(popup->get_tag());

    if (id == -1) {
        this->add({popup->tag});
        id = this->get_component_index(popup->get_tag());
    }

    if (id != -1) {
        ekgutil::component &component = this->component_list.at(id);
        component.data = popup->id;
    }
}

int32_t ekg_popup::get_component_index(const std::string &text) {
    for (int32_t i = 0; i < this->component_list.size(); i++) {
        if (this->component_list.at(i).text == text) {
            return i;
        }
    }

    return -1;
}

void ekg_popup::contains(bool &flag, uint32_t id) {
    if (flag) {
        return;
    }

    ekg_element* element;
    ekg_popup* popup;

    for (uint32_t &ids : this->children_stack.ids) {
        if (!the_ekg_core->find_element(element, ids)) {
            continue;
        }

        if (ids == id) {
            flag = true;
            break;
        }

        popup = (ekg_popup*) element;
        popup->contains(flag, id);

        if (flag) {
            break;
        }
    }
}
