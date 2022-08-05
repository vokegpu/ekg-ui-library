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

ekg_button::ekg_button() {
    this->set_text_dock(ekg::dock::CENTER);
    this->type = ekg::ui::BUTTON;
}

ekg_button::~ekg_button() {

}

void ekg_button::on_killed() {
    ekg_element::on_killed();
}

void ekg_button::on_sync() {
    ekg_element::on_sync();

    this->min_text_width = ekgfont::get_text_width(this->text) + (ekg::text_dock_offset * 2.0f);
    this->min_text_height = ekgfont::get_text_height(this->text);

    float scaled_min_text_width = this->min_text_width / 4.0f;

    this->rect.w = this->rect.w < scaled_min_text_width ? scaled_min_text_width : this->rect.w;
    this->rect.h = this->rect.h < this->min_text_height ? this->min_text_height : this->rect.h;

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
        this->text_offset_x = this->rect.w - this->min_text_width - (ekg::text_dock_offset);
    }

    if (bottom) {
        this->text_offset_y = this->rect.h - this->min_text_height - (this->min_text_height / 4.0f);
    }
}

void ekg_button::on_pre_event_update(SDL_Event &sdl_event) {
    ekg_element::on_pre_event_update(sdl_event);

    float mx = 0;
    float my = 0;

    if (ekgapi::motion(sdl_event, mx, my)) {
        ekgapi::set_direct(this->flag.over, this->is_hovering(mx, my));
    }
}

void ekg_button::on_event(SDL_Event &sdl_event) {
    ekg_element::on_event(sdl_event);

    float mx = 0;
    float my = 0;

    if (ekgapi::motion(sdl_event, mx, my)) {
        ekgapi::set(this->flag.highlight, this->flag.over);
    } else if (ekgapi::input_down_left(sdl_event, mx, my)) {
        ekgapi::set(this->flag.focused, this->flag.over);
        ekgapi::set_direct(this->flag.activy, false);
    } else if (ekgapi::input_up_left(sdl_event, mx, my)) {
        this->set_pressed(this->flag.over && this->flag.focused);
        ekgapi::set(this->flag.focused, false);
    }
}

void ekg_button::on_post_event_update(SDL_Event &sdl_event) {
    ekg_element::on_post_event_update(sdl_event);

    float mx = 0;
    float my = 0;

    if (ekgapi::motion(sdl_event, mx, my)) {
        ekgapi::set_direct(this->flag.over, false);
    }
}

void ekg_button::on_draw_refresh() {
    ekg_element::on_draw_refresh();
    ekggpu::rectangle(this->rect, ekg::theme().button_background);

    if (this->flag.highlight) {
        ekggpu::rectangle(this->rect, ekg::theme().button_highlight);
    }

    if (this->flag.focused) {
        ekggpu::rectangle(this->rect, ekg::theme().button_activy);
    }

    ekgfont::render(this->text, this->rect.x + this->text_offset_x, this->rect.y + this->text_offset_y, ekg::theme().string_enabled_color);
}

void ekg_button::set_text(const std::string &string) {
    if (this->text != string) {
        this->text = string;
        this->on_sync();
    }
}

std::string ekg_button::get_text() {
    return this->text;
}

void ekg_button::set_size(float width, float height) {
    ekg_element::set_size(width, height);

    if (this->rect.w != width || this->rect.h != height) {
        this->rect.w = width;
        this->rect.h = height;

        this->on_sync();
    }
}

void ekg_button::set_pos(float x, float y) {
    ekg_element::set_pos(x, y);
}

void ekg_button::set_width(float width) {
    this->set_size(width, this->rect.h);
}

void ekg_button::set_height(float height) {
    this->set_size(this->rect.x, height);
}

float ekg_button::get_min_text_width() {
    return this->min_text_height;
}

float ekg_button::get_min_text_height() {
    return this->min_text_height;
}

void ekg_button::set_text_dock(uint16_t flags) {
    if (this->enum_flags_text_dock != flags) {
        this->enum_flags_text_dock = flags;
        this->on_sync();
    }
}

uint16_t ekg_button::get_text_dock() {
    return this->enum_flags_text_dock;
}

void ekg_button::set_pressed(bool state) {
    if (this->flag.activy != state) {
        ekgapi::set_direct(this->flag.activy, state);

        if (state) {
            ekgapi::callback_button(this->id, this->text);
        }
    }
}

bool ekg_button::is_pressed() {
    bool flag = this->flag.activy;
    ekgapi::set_direct(this->flag.activy, false);
    return flag;
}
