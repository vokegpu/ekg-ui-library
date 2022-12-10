#ifndef EKG_UI_POPUP_H
#define EKG_UI_POPUP_H

#include "ekg/ui/abstract/ui_abstract.hpp"

namespace ekg::ui {
	class popup : public ekg::ui::abstract {
	protected:
		std::vector<ekg::component> component_list {};
		uint16_t text_flags {};
	public:
		void append(const std::vector<std::string>&);
		void append(const ekg::component&);
		void remove(std::string_view);
		void linked(std::string_view, ekg::popup*);

		void set_place(uint16_t);
		void set_text_align(uint16_t);
		uint16_t get_text_align();
	};
}

#endif