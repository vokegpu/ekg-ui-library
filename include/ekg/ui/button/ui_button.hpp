#pragma once
#ifndef EKG_UI_BUTTON_H
#define EKG_UI_BUTTON_H

#include "ekg/ui/abstract/ui_abstract.hpp"

namespace ekg::ui {
	class button : public ekg::ui::abstract {
	protected:
		bool value {};
	public:
		void set_value(bool value);
		bool get_value();
	};
}

#endif