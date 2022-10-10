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

        ekg::vec2 initial_pos {};
        ekg::vec2 initial_size {};
        ekg::vec2 pos {};
        ekg::vec2 size {};

        std::string tag {};
    public:
        void set_tag(std::string_view);
        std::string_view get_tag();

        void set_drag(uint16_t);
        uint16_t get_drag_dock();

        void set_resize(uint16_t);
        uint16_t get_resize_dock();

        void set_pos_initial(float, float);
        ekg::vec2 get_pos_initial();

        void set_size_initial(float, float);
        ekg::vec2 get_size_initial();

        void set_embed(uint16_t);
        uint16_t get_embed_dock();

        void set_size(float, float);
        ekg::vec2 get_size();

        void set_pos(float, float);
        ekg::vec2 get_pos();
    };
}

#endif