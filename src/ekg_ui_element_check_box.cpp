#include <ekg/impl/ekg_ui_element_check_box.hpp>
#include <ekg/ekg.hpp>

ekg_check_box::ekg_check_box() {
    this->set_text_dock(ekg::dock::LEFT);
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

void ekg_check_box::set_checked(float checked) {
    ekgapi::set(this->flag.old_activy, this->flag.activy, checked);
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
    ekg::core::instance.dispatch_todo_event(ekgutil::action::REFRESH);

    float text_width = ekgfont::get_text_width(this->text);

    this->min_text_width = text_width;
    this->min_text_height = ekgfont::get_text_height(this->text);

    this->square_size = this->min_text_height;
    this->min_text_width += this->square_size + (this->square_size / 3.5f);

    this->rect.w = this->rect.w < this->min_text_width ? this->min_text_width : this->rect.w;
    this->rect.h = this->rect.h < this->min_text_height ? this->min_text_height : this->rect.h;

    bool center = ekgutil::contains(this->enum_flags_text_dock, ekg::dock::CENTER);
    bool top = ekgutil::contains(this->enum_flags_text_dock, ekg::dock::TOP);
    bool bottom = ekgutil::contains(this->enum_flags_text_dock, ekg::dock::BOTTOM);
    bool left = ekgutil::contains(this->enum_flags_text_dock, ekg::dock::LEFT);
    bool right = ekgutil::contains(this->enum_flags_text_dock, ekg::dock::RIGHT);

    this->box_offset = this->square_size / 10.0f;

    if (center) {
        this->text_offset_x = (this->square_size / 2.0f) + (this->rect.w / 2.0f) - (text_width / 2);
        this->text_offset_y = (this->rect.h / 2.0f) - (this->min_text_height / 2);
    }

    if (top) {
        this->text_offset_y = this->min_text_height / 2.0f;
    }

    if (left) {
        this->box_offset = this->square_size / 10.0f;
        this->text_offset_x = this->square_size + this->box_offset;
    }

    if (right) {
        this->box_offset = this->square_size + (this->square_size / 10.0f);
        this->text_offset_x = this->rect.w - text_width - this->box_offset;
        this->box_offset = this->rect.w - this->box_offset;
    }

    if (bottom) {
        this->text_offset_y = this->rect.h - this->min_text_height - (this->min_text_height / 4.0f);
    }
}

void ekg_check_box::on_pre_event_update(SDL_Event &sdl_event) {
    ekg_element::on_pre_event_update(sdl_event);

    float mx = 0;
    float my = 0;

    if (ekgapi::motion(sdl_event, mx, my)) {
        ekgapi::set_direct(this->flag.old_over, this->flag.over, this->rect.collide_aabb_with_point(mx, my));
    }
}

void ekg_check_box::on_event(SDL_Event &sdl_event) {
    ekg_element::on_event(sdl_event);

    float mx = 0;
    float my = 0;

    if (ekgapi::motion(sdl_event, mx, my)) {
        ekgapi::set(this->flag.old_highlight, this->flag.highlight, this->flag.over);
    } else if (ekgapi::input_down_left(sdl_event, mx, my)) {
        ekgapi::set(this->flag.old_focused, this->flag.focused, this->flag.over);
    } else if (ekgapi::input_up_left(sdl_event, mx, my)) {
        if (this->flag.old_focused && this->flag.over) {
            this->toggle();
        }

        ekgapi::set(this->flag.old_focused, this->flag.focused, false);
    }
}

void ekg_check_box::on_post_event_update(SDL_Event &sdl_event) {
    ekg_element::on_post_event_update(sdl_event);

    float mx = 0;
    float my = 0;

    if (ekgapi::motion(sdl_event, mx, my)) {
        ekgapi::set_direct(this->flag.old_over, this->flag.over, false);
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

    ekggpu::rectangle(this->rect.x + this->box_offset + (this->square_size / 10.0f), this->rect.y + this->text_offset_y + (this->square_size / 10.0f), this->square_size - ((this->square_size / 10.0f) * 2.0f), this->square_size - ((this->square_size / 10.0f) * 2.0f), ekg::theme().check_box_highlight);

    if (this->flag.activy) {
        ekggpu::rectangle(this->rect.x + this->box_offset + (this->square_size / 6.0f), this->rect.y + this->text_offset_y + (this->square_size / 6.0f), this->square_size - ((this->square_size / 6.0f) * 2.0f), this->square_size - ((this->square_size / 6.0f) * 2.0f), ekg::theme().check_box_activy);
    }

    ekgfont::render(this->text, this->rect.x + this->text_offset_x, this->rect.y + this->text_offset_y, ekg::theme().string_color);
}