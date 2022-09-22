#pragma once
#ifndef EKG_UI_ELEMENT_BUTTON_H
#define EKG_UI_ELEMENT_BUTTON_H

#include "ui_element.hpp"

namespace ekg {
	class boolean : ekg::element {
	protected:
		bool value {};
	public:
		void set_value(bool value);
		bool get_value();
	};
}

#endif