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

#ifndef EKG_UI_POPUP_H
#define EKG_UI_POPUP_H

#include "ekg/ui/abstract/ui_abstract.hpp"
#include "ekg/util/io.hpp"

namespace ekg::ui {
	class popup : public ekg::ui::abstract {
    public:
        struct component {
        public:
            std::string name {};
            ekg::flag flag {};
            ekg::rect rect {};

            bool boolean {};
            void *data1 {};

            int32_t id {};
            int32_t linked_id {};
        };
	protected:
		std::vector<ekg::ui::popup::component> component_list {};
		uint16_t text_flags {};
        int32_t scaled_height {}, token_id {};
        ekg::font font_size;
	public:
		void append(const std::vector<std::string>&);
		void append(std::string_view);
		void append_linked(std::string_view, ekg::ui::popup*);
		void remove(std::string_view);

        int64_t contains(std::string_view);
        std::vector<ekg::ui::popup::component> &get_component_list();

        void set_pos(float, float);
        ekg::vec2 get_pos();

		void set_text_align(uint16_t);
		uint16_t get_text_align();

        void set_width(float);
        float get_width();

        void set_scaled_height(int32_t);
        int32_t get_scaled_height();
        float get_height();

        void set_font_size(ekg::font);
        ekg::font get_font_size();
	};
}

#endif