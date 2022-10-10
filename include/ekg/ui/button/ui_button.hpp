#ifndef EKG_UI_BUTTON_H
#define EKG_UI_BUTTON_H

#include "ekg/ui/abstract/ui_abstract.hpp"

namespace ekg::ui {
	class button : public ekg::ui::abstract {
	protected:
		bool value {};
        uint16_t dock_text {};
        std::string text {};
	public:
        void set_text(std::string_view);
        std::string_view get_text();

		void set_value(bool);
		bool get_value();

        void set_text(uint16_t);
        uint16_t get_text_dock();
	};
}

#endif