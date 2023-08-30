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

#ifndef EKG_UI_SLIDER_H
#define EKG_UI_SLIDER_H

#include "ekg/ui/abstract/ui_abstract.hpp"

namespace ekg::ui {
    class slider : public ekg::ui::abstract {
protected:
        float value {};
        float minimum_value {};
        float maximum_value {};

        int32_t scaled_size {};
        int32_t float_precision {};
        uint16_t bar_flags {};
        uint16_t text_flags {};

        ekg::font font_size {};
        ekg::axis bar_axis {};
        bool dragging {};
    public:
        void set_dragging(bool state);
        bool is_dragging();

        void set_place(uint16_t dock);
        void set_bar_axis(ekg::axis axis);
        ekg::axis get_bar_axis();

        void set_text_align(uint16_t dock);
        uint16_t get_text_align();

        void set_bar_align(uint16_t dock);
        uint16_t get_bar_align();

        void set_font_size(ekg::font font);
        ekg::font get_font_size();

        void set_width(float w);
        float get_width();

        void set_scaled_height(int32_t h);
        int32_t get_scaled_height();
        float get_height();

        void set_value(float val);
        float get_value();

        void set_value_min(float min);
        float get_value_min();

        void set_value_max(float max);
        float get_value_max();

        ekg::ui::slider *set_precision(int32_t precision);
        int32_t get_precision();
    };
}

#endif