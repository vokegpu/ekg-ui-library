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

#include "ekg/ui/button/ui_button.hpp"
#include "ekg/util/gui.hpp"

ekg::ui::button *ekg::ui::button::set_value(bool state) {
    if (this->value != state) {
        this->value = state;
        ekg::reload(this->id);
    }

    return this;
}

bool ekg::ui::button::get_value() {
    return this->value;
}

ekg::ui::button *ekg::ui::button::set_text(std::string_view new_text) {
    if (this->text != new_text) {
        this->text = new_text;

        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }

    return this;
}

std::string_view ekg::ui::button::get_text() {
    return this->text;
}

ekg::ui::button *ekg::ui::button::set_text_align(uint16_t enum_docks) {
    if (this->dock_text != enum_docks) {
        this->dock_text = enum_docks;

        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }

    return this;
}

uint16_t ekg::ui::button::get_text_align() {
    return this->dock_text;
}

ekg::ui::button *ekg::ui::button::set_callback(ekg::cpu::event *event) {
    if (this->callback != event && this->callback != nullptr) {
        delete this->callback;
        this->callback = nullptr;
    }

    this->callback = event;
    return this;
}

ekg::cpu::event *ekg::ui::button::get_callback() {
    return this->callback;
}

ekg::ui::button *ekg::ui::button::set_width(float width) {
    if (this->sync_ui.w != width) {
        this->sync_ui.w = width;

        ekg::bitwise::add(this->sync_flags, (uint16_t) ekg::uisync::dimension);
        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }

    return this;
}

float ekg::ui::button::get_width() {
    return this->rect_widget.w;
}

ekg::ui::button *ekg::ui::button::set_scaled_height(int32_t scaled_height_factor) {
    if (this->scaled_height != scaled_height_factor) {
        this->scaled_height = scaled_height_factor;

        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }

    return this;
}

float ekg::ui::button::get_height() {
    return this->rect_widget.h;
}

int32_t ekg::ui::button::get_scaled_height() {
    return this->scaled_height;
}

ekg::ui::button *ekg::ui::button::set_place(uint16_t flags) {
    if (this->dock_flags != flags) {
        this->dock_flags = flags;

        ekg::reload(this->id);
        ekg::synclayout(this->id);
    }

    return this;
}

ekg::ui::button *ekg::ui::button::set_font_size(ekg::font font) {
    if (this->font_size != font) {
        this->font_size = font;

        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }

    return this;
}

ekg::font ekg::ui::button::get_font_size() {
    return this->font_size;
}
