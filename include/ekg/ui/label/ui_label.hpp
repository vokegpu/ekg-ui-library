#ifndef EKG_UI_LABEL_H
#define EKG_UI_LABEL_H

#include "ekg/ui/abstract/ui_abstract.hpp"

namespace ekg::ui {
    class label : public ekg::ui::abstract {
    protected:
        std::string text {};
        uint16_t dock_text {};
        int32_t scaled_height {};
    public:
        void set_dock(uint16_t);
        int32_t get_scaled_height();

        void set_width(float width);
        float get_width();

        void set_height(int32_t scaled_factor_height);
        float get_height();

        void set_text(std::string_view);
        std::string_view get_text();

        void set_text(uint16_t);
        uint16_t get_text_dock();
    };
}

#endif