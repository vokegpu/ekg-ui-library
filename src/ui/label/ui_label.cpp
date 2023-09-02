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

#include "ekg/ui/label/ui_label.hpp"
#include "ekg/util/gui.hpp"

ekg::ui::label *ekg::ui::label::set_place(uint16_t flags) {
    if (this->dock_flags != flags) {
        this->dock_flags = flags;
        ekg::synclayout(this->parent_id);
    }

    return this;
}

ekg::ui::label *ekg::ui::label::set_text(std::string_view string) {
    if (this->text != string) {
        this->text = string;
        ekg::reload(this->id);
    }

    return this;
}

std::string ekg::ui::label::get_text() {
    return this->text;
}

ekg::ui::label *ekg::ui::label::set_text_align(uint16_t flags) {
    if (this->dock_text != flags) {
        this->dock_text = flags;
        ekg::reload(this->id);
    }

    return this;
}

uint16_t ekg::ui::label::get_text_align() {
    return this->dock_text;
}

ekg::ui::label *ekg::ui::label::set_width(float width) {
    if (this->sync_ui.w != width) {
        this->sync_ui.w = width;

        ekg::bitwise::add(this->sync_flags, (uint16_t) ekg::uisync::dimension);
        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }

    return this;
}

float ekg::ui::label::get_width() {
    return this->rect_widget.w;
}

ekg::ui::label *ekg::ui::label::set_scaled_height(int32_t scaled_factor_height) {
    if (this->scaled_height != scaled_factor_height) {
        this->scaled_height = scaled_factor_height;

        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }

    return  this;
}

float ekg::ui::label::get_height() {
    return this->rect_widget.h;
}

int32_t ekg::ui::label::get_scaled_height() {
    return this->scaled_height;
}

ekg::ui::label *ekg::ui::label::set_font_size(ekg::font font) {
    if (this->font_size != font) {
        this->font_size = font;

        ekg::reload(this->id);
        ekg::synclayout(this->parent_id);
    }

    return this;
}

ekg::font ekg::ui::label::get_font_size() {
    return this->font_size;
}