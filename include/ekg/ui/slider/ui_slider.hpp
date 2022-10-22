#ifndef EKG_UI_SLIDER_H
#define EKG_UI_SLIDER_H

#include "ekg/ui/abstract/ui_abstract.hpp"

namespace ekg::ui {
	class slider : public ekg::ui::abstract {
	protected:
        float value {};
        float minimum_value {};
        float maximum_value {};

        ekg::dock dock_bar {};
        ekg::font font_size {};
        int32_t scaled_height {};
        bool ieee754 {};
	public:
        void set_dock(uint16_t);
        void set_bar(uint16_t);
        uint16_t get_bar()

        void set_font_size(ekg::font font);
        ekg::font get_font_size();

        void set_width(float);
        float get_width();

        void set_scaled_height(int32_t);
        int32_t get_scaled_height();
        float get_height();

        void set_value(int32_t);
        void set_value(float);
        float get_value();

        void set_value_min(int32_t);
        void set_value_min(float);
        float get_value_min();

        void set_value_max(int32_t);
        void set_value_max(float);
        float get_value_max();
	};
}

#endif