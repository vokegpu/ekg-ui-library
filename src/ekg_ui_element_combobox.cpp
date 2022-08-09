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
#include "ekg/ekg.hpp"

ekg_combobox::ekg_combobox() {
    this->type = ekg::ui::COMBOBOX;
}

ekg_combobox::~ekg_combobox() {
}

float ekg_combobox::get_min_text_width() {
    return this->min_text_width;
}

float ekg_combobox::get_min_text_height() {
    return this->min_text_height;
}

void ekg_combobox::set_text_dock(uint16_t flags) {
    if (this->enum_flags_text_dock != flags) {
        this->enum_flags_text_dock = flags;
        this->on_sync();
    }
}

uint16_t ekg_combobox::get_text_dock() {
    return this->enum_flags_text_dock;
}

void ekg_combobox::set_width(float width) {
    this->set_size(width, this->rect.h);
}

void ekg_combobox::set_height(float height) {
    this->set_size(this->rect.w, height);
}

void ekg_combobox::add(const std::vector<std::string> &vec) {
    this->value_list.insert(this->value_list.end(), vec.begin(), vec.end());
}

void ekg_combobox::remove(const std::string &pattern) {
    std::vector<std::string> vec;

    for (std::string &string : this->value_list) {
        if (!ekgutil::contains(pattern, string)) {
            vec.push_back(string);
        }
    }

    if (vec.size() != this->value_list.size()) {
        this->value_list = vec;
        this->on_sync();
    }
}

void ekg_combobox::set_size(float width, float height) {
    ekg_element::set_size(width, height);

    width = width < this->min_text_width ? this->min_text_width : width;
    height = height < this->min_text_height ? this->min_text_height : height;

    if (this->rect.w != width || this->rect.h != height) {
        this->rect.w = width;
        this->rect.h = height;
        this->on_sync();
    }
}

void ekg_combobox::set_pos(float x, float y) {
    ekg_element::set_pos(x, y);
}

void ekg_combobox::on_sync() {
    ekg_element::on_sync();

    if (!this->contains(this->value)) {
        this->value = this->value_list.empty() ? "" : this->value_list[0];
    }

    float width = 0.0f;
    float longest_width = 0.0f;
    float val_width = 0.0;

    for (std::string &values : this->value_list) {
        width = ekgfont::get_text_width(values);

        if (width > longest_width) {
            longest_width = width;
        }

        if (values == this->value) {
            val_width = width;
        }
    }

    this->min_text_width = val_width;
    this->min_text_height = ekgfont::get_text_height(this->value);

    longest_width /= 4;

    this->rect.w = this->rect.w < longest_width ? this->min_text_width : this->rect.w;
    this->rect.h = this->rect.h < this->min_text_height ? this->min_text_height : this->rect.h;

    this->cache.w = this->rect.h;
    this->cache.h = this->rect.h;
    this->cache.x = this->rect.x + this->rect.w - this->cache.w;
    this->cache.y = this->rect.y;

    ekg::the_ekg_core->dispatch_todo_event(ekgutil::action::REFRESH);

    bool center = ekgutil::contains(this->enum_flags_text_dock, ekg::dock::CENTER);
    bool top = ekgutil::contains(this->enum_flags_text_dock, ekg::dock::TOP);
    bool bottom = ekgutil::contains(this->enum_flags_text_dock, ekg::dock::BOTTOM);
    bool left = ekgutil::contains(this->enum_flags_text_dock, ekg::dock::LEFT);
    bool right = ekgutil::contains(this->enum_flags_text_dock, ekg::dock::RIGHT);

    if (center) {
        this->text_offset_x = (this->rect.w / 2.0f) - (this->min_text_width / 2);
    }
    
    this->text_offset_y = (this->rect.h / 2.0f) - (this->min_text_height / 2);

    if (top) {
        this->text_offset_y = this->min_text_height / 2;
    }

    if (left) {
        this->text_offset_x = ekg::text_dock_offset;
    }

    if (right) {
        this->text_offset_x = this->rect.w - this->min_text_width - ekg::text_dock_offset - this->cache.w;
    }

    if (bottom) {
        this->text_offset_y = this->rect.h - this->min_text_height - (this->min_text_height / 2.0f);
    }
}

void ekg_combobox::on_pre_event_update(SDL_Event &sdl_event) {
    ekg_element::on_pre_event_update(sdl_event);

    float mx = 0;
    float my = 0;

    if (ekgapi::motion(sdl_event, mx, my)) {
        ekgapi::set_direct(this->flag.over, this->is_hovering(mx, my));
    }
}

void ekg_combobox::on_event(SDL_Event &sdl_event) {
    ekg_element::on_event(sdl_event);

    if (sdl_event.type == SDL_USEREVENT && this->flag.focused && !this->children_stack.ids.empty() && ekg::event()->type == ekg::ui::POPUP && ekg::event()->id == this->children_stack.ids[0]) {
        for (std::string &values : this->value_list) {
            if (ekgutil::contains(ekg::event()->text, values) && this->value != values) {
                this->value = values;
                this->on_sync();
                ekgapi::callback_combobox(this->id, this->value);
                break;
            }
        }

        ekgapi::set(this->flag.focused, false);
        return;
    }

    float mx = 0;
    float my = 0;

    if (ekgapi::motion(sdl_event, mx, my)) {
        ekgapi::set(this->flag.highlight, this->flag.over);
    } else if (ekgapi::any_input_down(sdl_event, mx, my)) {
        ekgapi::set(this->flag.activy, this->flag.over);
    } else if (ekgapi::any_input_up(sdl_event, mx, my)) {
        ekg_element* instance;

        if (this->flag.focused && (this->children_stack.ids.empty() || !ekg::the_ekg_core->find_element(instance, this->children_stack.ids.at(0)))) {
            ekgapi::set(this->flag.focused, false);
            this->children_stack.ids.clear();
        }

        if (this->flag.over && this->flag.activy && !this->flag.focused) {
            ekgapi::set(this->flag.focused, true);

            auto popup = ekg::popup(this->tag, this->value_list);
            popup->set_pos(this->rect.x, this->rect.y + this->rect.h);
            popup->set_width(this->rect.w);

            this->children_stack.ids.clear();
            this->children_stack.add(popup->get_id());
        }

        ekgapi::set(this->flag.activy, false);
    }
}

void ekg_combobox::on_post_event_update(SDL_Event &sdl_event) {
    ekg_element::on_post_event_update(sdl_event);

    float mx = 0;
    float my = 0;

    if (ekgapi::motion(sdl_event, mx, my)) {
        ekgapi::set_direct(this->flag.over, false);
    }
}

void ekg_combobox::on_update() {
    ekg_element::on_update();
}

void ekg_combobox::on_draw_refresh() {
    ekg_element::on_draw_refresh();

    ekggpu::rectangle(this->rect, ekg::theme().combobox_background);

    if (this->flag.highlight) {
        ekggpu::rectangle(this->rect, ekg::theme().combobox_highlight);
    }

    if (!this->flag.focused && !this->flag.activy) {
        ekggpu::rectangle(this->rect, ekg::theme().combobox_border, 1);
        ekggpu::rectangle(this->cache, ekg::theme().combobox_border);
    }

    if (this->flag.activy || this->flag.focused) {
        ekggpu::rectangle(this->cache, ekg::theme().combobox_activy, 1);
        ekggpu::rectangle(this->rect, ekg::theme().combobox_activy);
    }

    ekgfont::render(this->value, this->rect.x + this->text_offset_x, this->rect.y + this->text_offset_y, ekg::theme().string_enabled_color);
}

void ekg_combobox::set_value(const std::string &val) {
    if (this->value != val) {
        this->value = val;
        this->on_sync();
    }
}

std::string ekg_combobox::get_value() {
    return this->value;
}

bool ekg_combobox::contains(const std::string &str) {
    for (std::string &components : this->value_list) {
        if (components == str) {
            return true;
        }
    }

    return false;
}
