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
 * @VokeGpu 2022 all rights reserved.
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

        ekg::dock dock_bar {};
        ekg::font font_size {};
        int32_t scaled_height {};
        int32_t scaled_size {};
        bool ieee754 {};
    public:
        void set_dock(uint16_t);
        void set_bar(ekg::dock);
        ekg::dock get_bar_dock();

        void set_font_size(ekg::font font);
        ekg::font get_font_size();

        void set_width(float);
        float get_width();

        void set_scaled_height(int32_t);
        int32_t get_scaled_height();
        float get_height();

        void set_value(int32_t);
        void set_value(float);
        float get_value();

        void set_value_min(int32_t);
        void set_value_min(float);
        float get_value_min();

        void set_value_max(int32_t);
        void set_value_max(float);
        float get_value_max();

        bool is_float_precision();
    };
}

#endif