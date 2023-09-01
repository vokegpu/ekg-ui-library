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

#ifndef EKG_UI_CHECKBOX_H
#define EKG_UI_CHECKBOX_H

#include "ekg/ui/abstract/ui_abstract.hpp"
#include "ekg/cpu/event.hpp"

namespace ekg::ui {
    class checkbox : public ekg::ui::abstract {
    protected:
        bool value {};
        uint16_t dock_text {}, dock_box {};
        std::string text {};
        ekg::font font_size {};
        ekg::cpu::event *callback {};
    public:
        ekg::ui::checkbox *set_callback(ekg::cpu::event *p_callback);
        ekg::cpu::event *get_callback();

        ekg::ui::checkbox *set_font_size(ekg::font font);
        ekg::font get_font_size();
        ekg::ui::checkbox *set_place(uint16_t dock);

        ekg::ui::checkbox *set_width(float w);
        float get_width();

        ekg::ui::checkbox *set_scaled_height(int32_t h);
        int32_t get_scaled_height();
        float get_height();

        ekg::ui::checkbox *set_text(std::string_view text);
        std::string_view get_text();

        ekg::ui::checkbox *set_value(bool state);
        bool get_value();

        ekg::ui::checkbox *set_text_align(uint16_t dock);
        uint16_t get_text_align();

        ekg::ui::checkbox *set_box_align(uint16_t dock);
        uint16_t get_box_align();
    };
}

#endif