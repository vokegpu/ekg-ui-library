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
#include <ekg/ekg.hpp>
#include "ekg/impl/ekg_ui_element_slider.hpp"

ekg_slider::ekg_slider() {
    this->set_bar_axis(ekg::dock::LEFT);
    this->type = ekg::ui::SLIDER;
}

ekg_slider::~ekg_slider() {

}

float ekg_slider::get_min_text_width() {
    return this->min_text_width;
}

float ekg_slider::get_min_text_height() {
    return this->min_text_height;
}

void ekg_slider::set_width(float width) {
    this->set_size(width, this->rect.h);
}

void ekg_slider::set_height(float height) {
    this->set_size(this->rect.w, height);
}

void ekg_slider::set_value(float val) {
    if (this->value != val) {
        this->value = val;
        this->on_sync();
    }
}

float ekg_slider::get_value() {
    return this->value;
}

void ekg_slider::set_min(float val_min) {
    if (this->min != val_min) {
        this->min = val_min;
        this->on_sync();
    }
}

float ekg_slider::get_min() {
    return this->min;
}

void ekg_slider::set_max(float val_max) {
    if (this->max != val_max) {
        this->max = val_max;
        this->on_sync();
    }
}

float ekg_slider::get_max() {
    return this->max;
}

void ekg_slider::set_size(float width, float height) {
    ekg_element::set_size(width, height);

    if (this->rect.w != width || this->rect.h != height) {
        this->rect.w = width;
        this->rect.h = height;

        this->on_sync();
    }
}

void ekg_slider::set_pos(float x, float y) {
    ekg_element::set_pos(x, y);
}

void ekg_slider::on_killed() {
    ekg_element::on_killed();
}

void ekg_slider::on_sync() {
    ekg_element::on_sync();
    the_ekg_core->dispatch_todo_event(ekgutil::action::REFRESH);

    this->text = std::to_string(this->value);
    this->text = this->text.substr(0, this->text.find('.') + 3);

    this->min_text_width = ekgfont::get_text_width(this->text);
    this->min_text_height = ekgfont::get_text_height(this->text);

    this->rect.h = this->rect.h < this->min_text_height ? this->min_text_height : this->rect.h;

    float factor = (this->min_text_height / 4.0f);
    float factor_height = (this->min_text_height / 8.0f);

    this->bar[0] = this->rect.x + factor;
    this->bar[1] = this->rect.y + (this->rect.h / 2.0f) - (factor_height / 2.0f);
    this->bar[2] = this->rect.w - (factor * 2);
    this->bar[3] = factor_height;

    if (this->enum_flag_bar_axis_dock == ekg::dock::RIGHT) {
        this->bar_width = this->bar[2] * (this->value - this->min) / (this->max - this->min);
        this->bar_height = this->bar[3];
    } else if (this->enum_flag_bar_axis_dock == ekg::dock::LEFT) {
        this->bar_width = this->bar[2] * (this->value - this->min) / (this->max - this->min);
        this->bar_height = this->bar[3];
    } else if (this->enum_flag_bar_axis_dock == ekg::dock::TOP) {
        this->bar_width = this->bar[2];
        this->bar_height = this->bar[3] * (this->value - this->min) / (this->max - this->min);
    } else if (this->enum_flag_bar_axis_dock == ekg::dock::BOTTOM) {
        this->bar_width = this->bar[2];
        this->bar_height = this->bar[3] * (this->value - this->min) / (this->max - this->min);
    }
}

void ekg_slider::on_pre_event_update(SDL_Event &sdl_event) {
    ekg_element::on_pre_event_update(sdl_event);

    float mx = 0;
    float my = 0;

    if (ekgapi::motion(sdl_event, mx, my)) {
        ekgapi::set_direct(flag.over, this->rect.collide_aabb_with_point(mx, my));
    }
}

void ekg_slider::on_event(SDL_Event &sdl_event) {
    ekg_element::on_event(sdl_event);

    float mx = 0;
    float my = 0;

    if (ekgapi::motion(sdl_event, mx, my)) {
        ekgapi::set(this->flag.highlight, this->flag.over);
    } else if (ekgapi::input_down_left(sdl_event, mx, my)) {
        ekgapi::set(this->flag.focused, this->flag.over);
    } else if (ekgapi::any_input_up(sdl_event, mx, my)) {
        ekgapi::set(this->flag.focused, false);
    }

    if (this->flag.focused) {
        bool flag = (this->enum_flag_bar_axis_dock == ekg::dock::LEFT || this->enum_flag_bar_axis_dock == ekg::dock::RIGHT);

        float axis_mouse = flag ? mx : my;
        float axis_size = flag ? this->bar[2] : this->bar[3];
        float axis_pos = flag ? this->bar[0] : this->bar[1];

        float factor = std::min(axis_size, std::max(0.0f, axis_mouse - axis_pos));

        if (factor == 0) {
            this->set_value(this->get_min());
        } else {
            this->set_value((factor / axis_size) * (this->max - this->min) + this->min);
        }
    }
}

void ekg_slider::on_post_event_update(SDL_Event &sdl_event) {
    ekg_element::on_post_event_update(sdl_event);

    float mx = 0;
    float my = 0;

    if (ekgapi::motion(sdl_event, mx, my)) {
        ekgapi::set_direct(flag.over, false);
    }
}

void ekg_slider::on_draw_refresh() {
    ekg_element::on_draw_refresh();

    ekggpu::rectangle(this->rect, ekg::theme().slider_background);
    ekggpu::rectangle(this->bar[0], this->bar[1], this->bar[2], this->bar[3], ekg::theme().slider_highlight);

    bool flag = this->flag.focused;

    if (this->flag.highlight) {
        ekggpu::rectangle(this->rect, ekg::theme().slider_highlight);
        flag = true;
    }

    if (flag) {
        flag = (this->enum_flag_bar_axis_dock == ekg::dock::LEFT || this->enum_flag_bar_axis_dock == ekg::dock::RIGHT);

        // TODO height bar test.
        float width = this->bar_width;
        float height = this->bar_height;

        if (flag) {
            height = 0;
        } else {
            width = 0;
        }

        // Draw an immediate popup at pos based on bar width.
        the_ekg_core->immediate_popup(this->rect.x + width, this->rect.y + height, this->text);
    }

    ekggpu::rectangle(this->bar[0], this->bar[1], this->bar_width, this->bar_height, ekg::theme().slider_activy);
}

void ekg_slider::set_bar_axis(uint16_t dock) {
    this->enum_flag_bar_axis_dock = dock;

    if (this->enum_flag_bar_axis_dock != dock) {
        this->enum_flag_bar_axis_dock = dock;
        this->on_sync();
    }
}

float ekg_slider::get_bar_axis() {
    return this->enum_flag_bar_axis_dock;
}

bool ekg_slider::is_dragging() {
    return this->flag.focused;
}
