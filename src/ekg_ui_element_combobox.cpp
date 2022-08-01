#include "ekg/ekg.hpp"

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

    this->min_text_width = ekgfont::get_text_width(this->text);
    this->min_text_height = ekgfont::get_text_height(this->text);

    this->rect.w = this->rect.w < this->min_text_width ? this->min_text_width : this->rect.w;
    this->rect.h = this->rect.h < this->min_text_height ? this->min_text_height : this->rect.h;

    the_ekg_core->dispatch_todo_event(ekgutil::action::REFRESH);

    bool center = ekgutil::contains(this->enum_flags_text_dock, ekg::dock::CENTER);
    bool top = ekgutil::contains(this->enum_flags_text_dock, ekg::dock::TOP);
    bool bottom = ekgutil::contains(this->enum_flags_text_dock, ekg::dock::BOTTOM);
    bool left = ekgutil::contains(this->enum_flags_text_dock, ekg::dock::LEFT);
    bool right = ekgutil::contains(this->enum_flags_text_dock, ekg::dock::RIGHT);

    if (center) {
        this->text_offset_x = (this->rect.w / 2.0f) - (this->min_text_width / 2);
        this->text_offset_y = (this->rect.h / 2.0f) - (this->min_text_height / 2);
    }

    if (top) {
        this->text_offset_y = this->min_text_height / 2;
    }

    if (left) {
        this->text_offset_x = this->min_text_width / 4.0f;
    }

    if (right) {
        this->text_offset_x = this->rect.w - this->min_text_width - (this->min_text_width / 4.0f);
    }

    if (bottom) {
        this->text_offset_y = this->rect.h - this->min_text_height - (this->min_text_height / 4.0f);
    }
}

void ekg_combobox::on_pre_event_update(SDL_Event &sdl_event) {
    ekg_element::on_pre_event_update(sdl_event);
}

void ekg_combobox::on_event(SDL_Event &sdl_event) {
    ekg_element::on_event(sdl_event);
}

void ekg_combobox::on_post_event_update(SDL_Event &sdl_event) {
    ekg_element::on_post_event_update(sdl_event);
}

void ekg_combobox::on_update() {
    ekg_element::on_update();
}

void ekg_combobox::on_draw_refresh() {
    ekg_element::on_draw_refresh();

    ekggpu::rectangle(this->rect, ekg::theme().combobox_background);
    ekgfont::render(this->text, this->rect.x + this->text_offset_x, this->rect.y + this->text_offset_y, ekg::theme().string_enabled_color);


}
