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
 * @VokeGpu 2023 all rights reserved.
 */

#include "ekg/ui/checkbox/ui_checkbox.hpp"
#include "ekg/util/util_ui.hpp"

void ekg::ui::checkbox::set_callback(ekg::cpu::event *event) {
    event->flags = ekg::bitwise::add(event->flags, ekg::event::shared);

    if (this->callback != event && this->callback != nullptr) {
        delete this->callback;
        this->callback = nullptr;
    }

    this->callback = event;
}

ekg::cpu::event *ekg::ui::checkbox::get_callback() {
    return this->callback;
}

void ekg::ui::checkbox::set_font_size(ekg::font font) {
    if (this->font_size != font) {
        this->font_size = font;

        ekg::synclayout(this->parent_id);
        ekg::reload(this->id);
    }
}

ekg::font ekg::ui::checkbox::get_font_size() {
    return this->font_size;
}

void ekg::ui::checkbox::set_place(uint16_t docks) {
    if (this->dock_flags != docks) {
        this->dock_flags = docks;

        ekg::synclayout(this->id);
    }
}

void ekg::ui::checkbox::set_width(float width) {
    if (this->sync_ui.w != width) {
        this->sync_ui.w = width;

        ekg::bitwise::add(this->sync_flags, (uint16_t) ekg::uisync::dimension);
        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }
}

float ekg::ui::checkbox::get_width() {
    return this->rect_widget.w;
}

void ekg::ui::checkbox::set_scaled_height(int32_t scaled_height_factor) {
    if (this->scaled_height != scaled_height_factor) {
        this->scaled_height = scaled_height_factor;

        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }
}

int32_t ekg::ui::checkbox::get_scaled_height() {
    return this->scaled_height;
}

float ekg::ui::checkbox::get_height() {
    return this->rect_widget.h;
}

void ekg::ui::checkbox::set_text(std::string_view string) {
    if (this->text != string) {
        this->text = string;

        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }
}

std::string_view ekg::ui::checkbox::get_text() {
    return this->text;
}

void ekg::ui::checkbox::set_value(bool val) {
    if (this->value != val) {
        this->value = val;
        ekg::reload(this->id);
    }
}

bool ekg::ui::checkbox::get_value() {
    return this->value;
}

void ekg::ui::checkbox::set_text_align(uint16_t docks) {
    if (this->dock_text != docks) {
        this->dock_text = docks;
        ekg::reload(this->id);
    }
}

uint16_t ekg::ui::checkbox::get_text_align() {
    return this->dock_text;
}

void ekg::ui::checkbox::set_box_align(uint16_t docks) {
    if (this->dock_box != docks) {
        this->dock_box = docks;
        ekg::reload(this->id);
    }
}

uint16_t ekg::ui::checkbox::get_box_align() {
    return this->dock_box;    
}
