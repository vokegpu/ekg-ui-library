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
#ifndef EKG_UI_TEXTBOX_H
#define EKG_UI_TEXTBOX_H

#include "ekg/ui/abstract/ui_abstract.hpp"

namespace ekg::ui {
    class textbox : public ekg::ui::abstract {
    protected:
        std::string text {};
        ekg::font font_size {};
        bool enabled {true};
        uint8_t tab_size {4};

        uint64_t max_lines {UINT32_MAX};
        uint64_t max_chars_per_line {UINT32_MAX};
    public:
        /*
          * As the name says, unsafe set text is not a method for any purpose.
          */
        void unsafe_set_text(std::string &ref_text);
    public:
        ekg::ui::textbox *set_enabled(bool enabled);
        bool is_enabled();

        ekg::ui::textbox *set_font_size(ekg::font font);
        ekg::font get_font_size();

        ekg::ui::textbox *set_place(uint16_t dock);
        int32_t get_scaled_height();

        ekg::ui::textbox *set_width(float w);
        float get_width();

        ekg::ui::textbox *set_scaled_height(int32_t h);
        float get_height();

        ekg::ui::textbox *set_text(std::string_view text);
        std::string get_text();

        ekg::ui::textbox *set_tab_size(uint8_t size);
        uint8_t get_tab_size();

        ekg::ui::textbox *set_max_chars_per_line(uint64_t chars_per_line);
        uint64_t get_max_chars_per_line();

        ekg::ui::textbox *set_max_lines(uint64_t lines);
        uint64_t get_max_lines();
    };
}

#endif