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

ekg_check_box::ekg_check_box() {
    this->type = ekg::ui::CHECKBOX;
}

ekg_check_box::~ekg_check_box() {

}

void ekg_check_box::set_text(const std::string &string) {
    if (this->text != string) {
        this->text = string;
        this->on_sync();
    }
}

std::string ekg_check_box::get_text() {
    return this->text;
}

void ekg_check_box::toggle() {
    this->set_checked(!this->is_checked());
}

void ekg_check_box::set_checked(bool checked) {
    if (this->flag.activy != checked) {
        ekgapi::set(this->flag.activy, checked);
        ekgapi::callback_check_box(this->id, this->text, checked);
    }
}

bool ekg_check_box::is_checked() {
    return this->flag.activy;
}

float ekg_check_box::get_min_text_width() {
    return this->min_text_width;
}

float ekg_check_box::get_min_text_height() {
    return this->min_text_height;
}

void ekg_check_box::set_text_dock(uint16_t flags) {
    if (this->enum_flags_text_dock != flags) {
        this->enum_flags_text_dock = flags;
        this->on_sync();
    }
}

uint16_t ekg_check_box::get_text_dock() {
    return this->enum_flags_text_dock;
}

void ekg_check_box::set_width(float width) {
    this->set_size(width, this->rect.h);
}

void ekg_check_box::set_height(float height) {
    this->set_size(this->rect.h, height);
}

void ekg_check_box::set_size(float width, float height) {
    ekg_element::set_size(width, height);

    if (this->rect.w != width || this->rect.h != height) {
        this->rect.w = width;
        this->rect.h = height;

        this->on_sync();
    }
}

void ekg_check_box::set_pos(float x, float y) {
    ekg_element::set_pos(x, y);
}

void ekg_check_box::on_killed() {
    ekg_element::on_killed();
}

void ekg_check_box::on_sync() {
    ekg_element::on_sync();
    the_ekg_core->dispatch_todo_event(ekgutil::action::REFRESH);

    float text_width = ekgfont::get_text_width(this->text);

    this->min_text_width = text_width;
    this->min_text_height = ekgfont::get_text_height(this->text);

    this->cache.w = this->min_text_height;
    this->cache.h = this->min_text_height;
    this->min_text_width += this->cache.w + (this->cache.w / 3.5f);

    this->rect.w = this->rect.w < this->min_text_width ? this->min_text_width : this->rect.w;
    this->rect.h = this->rect.h < this->min_text_height ? this->min_text_height : this->rect.h;

    bool center = ekgutil::contains(this->enum_flags_text_dock, ekg::dock::CENTER);
    bool top = ekgutil::contains(this->enum_flags_text_dock, ekg::dock::TOP);
    bool bottom = ekgutil::contains(this->enum_flags_text_dock, ekg::dock::BOTTOM);
    bool left = ekgutil::contains(this->enum_flags_text_dock, ekg::dock::LEFT);
    bool right = ekgutil::contains(this->enum_flags_text_dock, ekg::dock::RIGHT);

    this->box_offset = ekg::text_dock_offset;

    if (center) {
        this->text_offset_x = (this->cache.w / 2) + (this->rect.w / 2) - (text_width / 2);
    }
    
    this->text_offset_y = (this->rect.h / 2) - (this->min_text_height / 2);

    if (top) {
        this->text_offset_y = this->min_text_height / 2;
    }

    if (left) {
        this->text_offset_x = this->cache.w + this->box_offset;
    }

    if (right) {
        this->text_offset_x = this->rect.w - text_width - this->box_offset - this->cache.w;
        this->box_offset = this->rect.w - this->cache.w - ekg::text_dock_offset;
    }

    if (bottom) {
        this->text_offset_y = this->rect.h - this->min_text_height - (this->min_text_height / 4);
    }

    this->cache.x = this->rect.x + this->box_offset;
    this->cache.y = this->rect.y + this->text_offset_y;
    this->cache.w = (this->cache.w - ((this->cache.w / 10) * 2));
    this->cache.h = this->cache.w;
}

void ekg_check_box::on_pre_event_update(SDL_Event &sdl_event) {
    ekg_element::on_pre_event_update(sdl_event);

    float mx = 0;
    float my = 0;

    if (ekgapi::motion(sdl_event, mx, my)) {
        ekgapi::set_direct(this->flag.over, this->is_hovering(mx, my));
    }
}

void ekg_check_box::on_event(SDL_Event &sdl_event) {
    ekg_element::on_event(sdl_event);

    float mx = 0;
    float my = 0;

    if (ekgapi::motion(sdl_event, mx, my)) {
        ekgapi::set(this->flag.highlight, this->flag.over);
        ekgapi::set(this->flag.extra, this->flag.over && this->cache.collide_aabb_with_point(mx, my));
    } else if (ekgapi::input_down_left(sdl_event, mx, my)) {
        ekgapi::set(this->flag.focused, this->flag.over);
    } else if (ekgapi::input_up_left(sdl_event, mx, my)) {
        if (this->flag.focused && this->flag.over) {
            this->toggle();
        }

        ekgapi::set(this->flag.focused, false);
    }
}

void ekg_check_box::on_post_event_update(SDL_Event &sdl_event) {
    ekg_element::on_post_event_update(sdl_event);

    float mx = 0;
    float my = 0;

    if (ekgapi::motion(sdl_event, mx, my)) {
        ekgapi::set_direct(this->flag.over, false);
    }
}

void ekg_check_box::on_draw_refresh() {
    ekg_element::on_draw_refresh();
    ekggpu::rectangle(this->rect, ekg::theme().check_box_background);

    if (this->flag.highlight) {
        ekggpu::rectangle(this->rect, ekg::theme().check_box_highlight);
    }

    if (this->flag.focused) {
        ekggpu::rectangle(this->rect, ekg::theme().check_box_highlight);
    }

    ekggpu::rectangle(this->cache, ekg::theme().check_box_highlight);

    if (this->flag.activy || this->flag.extra) {
        float offset_pos = 2 * static_cast<float>(this->flag.activy);
        float offset_size = 4 * static_cast<float>(this->flag.activy);

        ekggpu::rectangle(this->cache.x + offset_pos, this->cache.y + offset_pos, this->cache.w - offset_size, this->cache.h - offset_size, ekg::theme().check_box_activy, this->flag.activy ? 0 : 2);

        if (this->flag.extra && this->flag.activy) {
            ekggpu::rectangle(this->cache.x, this->cache.y, this->cache.w, this->cache.h, ekg::theme().check_box_activy, 2);
        }
    }

    ekgfont::render(this->text, this->rect.x + this->text_offset_x, this->rect.y + this->text_offset_y, ekg::theme().string_enabled_color);
}