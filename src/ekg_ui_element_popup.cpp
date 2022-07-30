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
    this->type = ekg::ui::POPUP;
}

ekg_popup::~ekg_popup() {

}

void ekg_popup::set_opened(bool opened) {
    uint16_t flag = opened ? ekg::visibility::VISIBLE : ekg::visibility::LOW_PRIORITY;

    if (this->visibility != flag) {
        this->animation_elapsed_ticks = SDL_GetTicks();
        this->set_visibility(flag);
    }

    ekgapi::set(this->flag.focused, opened);
    this->set_should_update(true);
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

    this->on_sync();
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

    this->on_sync();
}

void ekg_popup::state(const std::string &pattern, bool enabled) {
    for (ekgutil::component &component : this->component_list) {
        if (ekgutil::contains(pattern, component.text)) {
            component.enabled = enabled;
        }
    }

    this->on_sync();
}

void ekg_popup::set_size(float width, float height) {
    ekg_element::set_size(width, height);

    if (this->rect.w != width || this->component_height != height) {
        this->rect.w = width;
        this->component_height = height;
        this->on_sync();
    }
}

void ekg_popup::destroy() {
    ekg_popup* popup;

    if (this->has_mother() && the_ekg_core->find_element((ekg_element*&) popup, this->mother_id) && !popup->access_flag().dead) {
        popup->destroy();
    }

    the_ekg_core->kill_element(this);

    for (uint32_t &ids : this->children_stack.ids) {
        if (the_ekg_core->find_element((ekg_element*&) popup, ids)) {
            popup->destroy();
        }
    }
}

void ekg_popup::set_pos(float x, float y) {
    if (!this->has_mother()) {
        ekgmath::clamp_aabb_with_screen_size(x, y, this->rect.w, this->full_height);
    } else {
        x += this->scaled.x;
        y += this->scaled.y;

        float width = the_ekg_core->get_screen_width();
        float height = the_ekg_core->get_screen_height();

        if (x + this->rect.w > width) {
            x = this->scaled.x - (this->rect.w - (this->component_height / 10));
        }

        if (y + this->full_height > height) {
            y -= this->full_height - this->component_height;
        }

        x -= this->scaled.x;
        y -= this->scaled.y;
    }

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

    auto offset_dimension = static_cast<float>(static_cast<int32_t>(this->component_height / 9));
    this->offset_separator = offset_dimension;
    this->full_height = this->offset_separator;

    // First iteration for get real the most large string width.
    for (ekgutil::component &component : this->component_list) {
        max_width = ekgfont::get_text_width(component.text) + offset_dimension * 4;
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
    this->rect.h = this->rect.h < this->component_text_min_height ? this->component_text_min_height: this->rect.h;

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

        component.x = this->rect.x + offset_dimension + this->component_text_offset_x;
        component.y = this->rect.y + this->full_height + this->component_text_offset_y;
        component.w = this->rect.w - (offset_dimension * 3);
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
        highlight = this->flag.over || !this->flag.focused || (this->has_mother() && this->mother_id == the_ekg_core->get_popup_top_level());
    } else if (ekgapi::any_input_down(sdl_event, mx, my) && this->flag.focused) {
        bool flag = the_ekg_core->get_hovered_element_type() != ekg::ui::POPUP;

        if (flag) {
            ekgapi::set(this->flag.focused, false);
            this->set_visibility(ekg::visibility::NONE);
            this->set_should_update(true);
        }

        if (this->flag.highlight && this->flag.over && !this->activy_component.empty()) {
            ekgapi::set_direct(this->flag.activy, this->flag.highlight);
        }

        highlight = this->flag.over;
    } else if (ekgapi::any_input_up(sdl_event, mx, my) && this->flag.focused) {
        if (this->flag.activy && this->flag.over) {
            ekgapi::set(this->flag.focused, false);
            
            this->set_visibility(ekg::visibility::NONE);
            this->set_should_update(true);
        }
    }

    if (highlight) {
        std::string previous_focused_element = this->focused_component;

        if (this->flag.focused) {
            this->full_height = this->offset_separator;

            for (ekgutil::component &component : this->component_list) {
                if (ekgmath::collide_aabb_with_point(this->rect.x, this->rect.y + this->full_height, component.w, component.h) && component.enabled) {
                    ekgapi::set(this->flag.highlight, true);
                    ekgapi::set(this->focused_component, component.text);

                    the_ekg_core->get_popup_top_level() = component.data != 0 ? this->id : this->mother_id;
                }

                this->full_height += component.h + this->offset_separator;
            }
        }

        if (this->has_mother() && ((!this->flag.focused) || (the_ekg_core->get_popup_top_level() == this->mother_id && !this->flag.over))) {
            ekgapi::set(this->flag.highlight, false);
            ekgapi::set(this->focused_component, "");
        }

        if (this->focused_component == previous_focused_element) {
            return;
        }

        bool flag = false;
        ekg_popup* popup;
        this->activy_component = this->focused_component;

        for (uint32_t &ids : this->children_stack.ids) {
            if (!the_ekg_core->find_element((ekg_element*&) popup, ids)) {
                continue;
            }

            flag = popup->get_tag() == this->focused_component;

            if (flag) {
                this->activy_component = "";
                this->get_component_pos(popup->get_tag(), mx, my);

                popup->access_scaled_rect().copy(this->rect);
                popup->set_pos(mx + this->rect.w - (this->component_height / 10), my);
            }

            popup->set_opened(flag);
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

    switch (this->visibility) {
        case ekg::visibility::NONE: {
            if (this->flag.focused) {
                this->set_should_update(false);
                break;
            }

            this->destroy();
            this->set_should_update(false);
            the_ekg_core->dispatch_todo_event(ekgutil::action::REFRESH);
            break;
        }

        case ekg::visibility::VISIBLE: {
            this->cache.x = this->rect.x;
            this->cache.y = this->rect.y;
            this->cache.w = this->rect.w;
            this->rect.h = this->full_height;

            float val = 0.0f;
            ekgmath::smoothf(val, 100, SDL_GetTicks() - this->animation_elapsed_ticks);

            this->cache.h = val * this->full_height;

            // Set scissor size and pos.
            ekggpu::bind_scissor(this->scissor_id);
            ekggpu::scissor(this->cache);

            if (this->cache.h >= this->full_height) {
                this->cache.h = this->full_height;
                this->set_should_update(false);
            }

            break;
        }

        case ekg::visibility::LOW_PRIORITY: {
            this->cache.h = 0;
            this->set_should_update(false);
            the_ekg_core->dispatch_todo_event(ekgutil::action::REFRESH);
            break;
        }
    }
}

void ekg_popup::on_draw_refresh() {
    ekg_element::on_draw_refresh();

    float height_scaled = this->offset_separator;
    auto offset_dimension = static_cast<float>(static_cast<int32_t>(this->component_height / 8));

    // We want to enable scissor in this element.
    // So here, we get the current scissor id from gpu handler.
    this->scissor_id = ekggpu::start_scissor();

    // Enable scissor and set the scissor area.
    ekggpu::bind_scissor(this->scissor_id);
    ekggpu::scissor(this->cache);

    // Background.
    ekggpu::rectangle(this->rect, ekg::theme().popup_background);

    for (ekgutil::component &component : this->component_list) {
        if (component.enabled && this->flag.highlight && component.text == this->focused_component) {
            ekggpu::rectangle(this->rect.x + (offset_dimension), this->rect.y + height_scaled, component.w, component.h, ekg::theme().popup_highlight);
        }

        // Render text.
        ekgfont::render(component.text, component.x, component.y, component.enabled ? ekg::theme().string_enabled_color : ekg::theme().string_disabled_color);

        // Add height by iteration.
        height_scaled += component.h + this->offset_separator;
    }

    ekggpu::rectangle(this->rect, ekg::theme().popup_border, 1);
    ekggpu::end_scissor();
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

void ekg_popup::place(ekg_popup *popup) {
    if (popup == nullptr || popup->get_id() == this->id || this->children_stack.contains(popup->get_id())) {
        return;
    }

    this->children_stack.add(popup->get_id());
    popup->set_opened(false);

    int32_t id = this->get_component_index(popup->get_tag());

    if (id == -1) {
        this->add({popup->tag});
        id = this->get_component_index(popup->get_tag());
    }

    if (id != -1) {
        ekgutil::component &component = this->component_list.at(id);
        component.data = popup->id;
    }

    popup->set_mother_id(this->id);
}

int32_t ekg_popup::get_component_index(const std::string &text) {
    for (int32_t i = 0; i < this->component_list.size(); i++) {
        if (this->component_list.at(i).text == text) {
            return i;
        }
    }

    return -1;
}

bool ekg_popup::get_component_pos(const std::string &text, float &x, float &y) {
    x = 0;
    y = 0;

    for (ekgutil::component &components : this->component_list) {
        if (components.text == text) {
            return true;
        }

        y += this->component_height + this->offset_separator;
    }

    return false;
}

void ekg_popup::children_popup_hovered_flag(bool &flag, uint32_t id) {
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
        popup->children_popup_hovered_flag(flag, id);

        if (flag) {
            break;
        }
    }
}
