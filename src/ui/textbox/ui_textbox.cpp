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
#include "ekg/util/gui.hpp"

void ekg::ui::textbox::unsafe_set_text(std::string &ref_text) {
    this->text = ref_text;
}

ekg::ui::textbox *ekg::ui::textbox::set_tab_size(uint8_t size) {
    this->tab_size = size < 1 ? 1 : size;
    return  this;
}

uint8_t ekg::ui::textbox::get_tab_size() {
    return this->tab_size;
}

ekg::ui::textbox *ekg::ui::textbox::set_enabled(bool state) {
    this->enabled = state;
    return  this;
}

bool ekg::ui::textbox::is_enabled() {
    return this->enabled;
}

ekg::ui::textbox *ekg::ui::textbox::set_place(uint16_t flags) {
    if (this->dock_flags != flags) {
        this->dock_flags = flags;
        ekg::synclayout(this->parent_id);
    }

    return  this;
}

ekg::ui::textbox *ekg::ui::textbox::set_text(std::string_view string) {
    if (this->text != string) {
        this->text = string;
        ekg::reload(this->id);
    }

    return  this;
}

std::string ekg::ui::textbox::get_text() {
    return this->text;
}

ekg::ui::textbox *ekg::ui::textbox::set_width(float width) {
    if (this->sync_ui.w != width) {
        this->sync_ui.w = width;

        ekg::bitwise::add(this->sync_flags, (uint16_t) ekg::uisync::dimension);
        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }

    return  this;
}

float ekg::ui::textbox::get_width() {
    return this->rect_widget.w;
}

ekg::ui::textbox *ekg::ui::textbox::set_scaled_height(int32_t scaled_factor_height) {
    if (this->scaled_height != scaled_factor_height) {
        this->scaled_height = scaled_factor_height;

        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }

    return  this;
}

float ekg::ui::textbox::get_height() {
    return this->rect_widget.h;
}

int32_t ekg::ui::textbox::get_scaled_height() {
    return this->scaled_height;
}

ekg::ui::textbox *ekg::ui::textbox::set_font_size(ekg::font font) {
    if (this->font_size != font) {
        this->font_size = font;

        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }

    return  this;
}

ekg::font ekg::ui::textbox::get_font_size() {
    return this->font_size;
}

ekg::ui::textbox *ekg::ui::textbox::set_max_lines(uint64_t lines) {
    if (this->max_lines != lines) {
        this->max_lines = lines;
        ekg::reload(this->id);
    }

    return  this;
}

uint64_t ekg::ui::textbox::get_max_lines() {
    return this->max_lines;
}

ekg::ui::textbox *ekg::ui::textbox::set_max_chars_per_line(uint64_t chars_per_line) {
    if (this->max_chars_per_line != chars_per_line) {
        this->max_chars_per_line = chars_per_line;
        ekg::reload(this->id);
    }

    return  this;
}

uint64_t ekg::ui::textbox::get_max_chars_per_line() {
    return this->max_chars_per_line;
}