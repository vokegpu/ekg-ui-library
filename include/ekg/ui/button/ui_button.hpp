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

#ifndef EKG_UI_BUTTON_H
#define EKG_UI_BUTTON_H

#include "ekg/ui/abstract/ui_abstract.hpp"
#include "ekg/cpu/event.hpp"

namespace ekg::ui {
	class button : public ekg::ui::abstract {
	protected:
		bool value {};
        uint16_t dock_text {};

        std::string text {};
        ekg::cpu::event* callback {nullptr};
        ekg::font font_size {};
    public:
        void set_font_size(ekg::font font);
        ekg::font get_font_size();
        void set_place(uint16_t dock);

        void set_width(float w);
        float get_width();

        void set_scaled_height(int32_t h);
        int32_t get_scaled_height();
        float get_height();

        void set_callback(ekg::cpu::event *p_callback);
        ekg::cpu::event *get_callback();

        void set_text(std::string_view text);
        std::string_view get_text();

		void set_value(bool state);
		bool get_value();

        void set_text_align(uint16_t dock);
        uint16_t get_text_align();
	};
}

#endif