#ifndef EKG_UI_BUTTON_H
#define EKG_UI_BUTTON_H

#include "ekg/ui/abstract/ui_abstract.hpp"
#include "ekg/cpu/event.hpp"
#include "ekg/util/env.hpp"

namespace ekg::ui {
	class button : public ekg::ui::abstract {
	protected:
		bool value {};
        uint16_t dock_text {};
        int32_t scaled_height {};

        std::string text {};
        ekg::cpu::event* callback {nullptr};
        ekg:: font font_size {};
    public:
        void set_font_size(ekg::font font);
        ekg::font get_font_size();
        void set_dock(uint16_t);

        void set_width(float);
        float get_width();

        void set_scaled_height(int32_t);
        int32_t get_scaled_height();
        float get_height();

        void set_callback(ekg::cpu::event*);
        ekg::cpu::event* get_callback();

        void set_text(std::string_view);
        std::string_view get_text();

		void set_value(bool);
		bool get_value();

        void set_text(uint16_t);
        uint16_t get_text_dock();
	};
}

#endif