#ifndef EKG_UI_FRAME_H
#define EKG_UI_FRAME_H

#include "ekg/ui/abstract/ui_abstract.hpp"
#include "ekg/util/geometry.hpp"
#include <vector>

namespace ekg::ui {
    class frame : public ekg::ui::abstract {
    protected:
        uint16_t dock_resize {};
        uint16_t dock_drag {};
        uint16_t dock_embed {};

        ekg::rect rect_initial {};
        std::string tag {};
        ekg::vec2 scale_factor {};
    public:
        void set_scale_factor(float, float);
        ekg::vec2 get_scale_factor();

        void set_drag(uint16_t);
        uint16_t get_drag_dock();

        void set_resize(uint16_t);
        uint16_t get_resize_dock();

        void set_pos_initial(float, float);
        ekg::vec2 get_pos_initial();

        void set_size_initial(float, float);
        ekg::vec2 get_size_initial();

        void set_initial_width(float);
        float get_initial_width();

        void set_initial_height(float);
        float get_initial_height();

        void set_embed(uint16_t);
        uint16_t get_embed_dock();

        void set_size(float, float);
        ekg::vec2 get_size();

        void set_pos(float, float);
        ekg::vec2 get_pos();

        void set_width(float);
        float get_width();

        void set_height(float);
        float get_height();
    };
}

#endif