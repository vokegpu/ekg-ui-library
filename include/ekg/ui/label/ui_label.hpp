/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, use in any thing (hacking, cheating, games, softwares).
 * 5- All malwares, rat and others virus. We do not care.
 * 6- Do not modify this license under any circunstancie.
 *
 * @VokeGpu 2022 all rights reserved.
 */

#ifndef EKG_UI_LABEL_H
#define EKG_UI_LABEL_H

#include "ekg/ui/abstract/ui_abstract.hpp"
#include "ekg/util/env.hpp"

namespace ekg::ui {
    class label : public ekg::ui::abstract {
    protected:
        std::string text {};
        uint16_t dock_text {};
        int32_t scaled_height {};
        ekg:: font font_size {};
    public:
        void set_font_size(ekg::font font);
        ekg::font get_font_size();

        void set_dock(uint16_t);
        int32_t get_scaled_height();

        void set_width(float width);
        float get_width();

        void set_scaled_height(int32_t scaled_factor_height);
        float get_height();

        void set_text(std::string_view);
        std::string_view get_text();

        void set_text(uint16_t);
        uint16_t get_text_dock();
    };
}

#endif