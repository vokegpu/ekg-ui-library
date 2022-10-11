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
	public:
        void set_callback(ekg::cpu::event* callback);
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