#ifndef EKG_UI_SLIDER_H
#define EKG_UI_SLIDER_H

#include "ekg/ui/abstract/ui_abstract.hpp"

namespace ekg::ui {
	class slider : public ekg::ui::abstract {
	protected:
		double min_value {};
		double max_value {};
		double value {};
	public:
	};
}

#endif