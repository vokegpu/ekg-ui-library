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
#include "ekg/ui/textbox/ui_textbox.hpp"
#include "ekg/util/util_ui.hpp"

void ekg::ui::textbox::set_enabled(bool state) {
    this->enabled = state;
}

bool ekg::ui::textbox::is_enabled() {
    return this->enabled;
}

void ekg::ui::textbox::set_place(uint16_t flags) {
    if (this->dock_flags != flags) {
        this->dock_flags = flags;
        ekg::synclayout(this->parent_id);
    }
}

void ekg::ui::textbox::set_text(std::string_view string) {
    if (this->text != string) {
        this->text = string;
        ekg::reload(this->id);
    }
}

std::string ekg::ui::textbox::get_text() {
    return this->text;
}

void ekg::ui::textbox::set_width(float width) {
    if (this->sync_ui.w != width) {
        this->sync_ui.h = width;

        ekg::bitwise::add(this->sync_flags, (uint16_t) ekg::uisync::dimension);
        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }
}

float ekg::ui::textbox::get_width() {
    return this->rect_widget.w;
}

void ekg::ui::textbox::set_scaled_height(int32_t scaled_factor_height) {
    if (this->scaled_height != scaled_factor_height) {
        this->scaled_height = scaled_factor_height;

        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }
}

float ekg::ui::textbox::get_height() {
    return this->rect_widget.h;
}

int32_t ekg::ui::textbox::get_scaled_height() {
    return this->scaled_height;
}

void ekg::ui::textbox::set_font_size(ekg::font font) {
    if (this->font_size != font) {
        this->font_size = font;

        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }
}

ekg::font ekg::ui::textbox::get_font_size() {
    return this->font_size;
}

void ekg::ui::textbox::set_max_chars(uint64_t chars_amount) {
    this->max_chars = chars_amount;
}

uint64_t ekg::ui::textbox::get_max_chars() {
    return this->max_chars;
}