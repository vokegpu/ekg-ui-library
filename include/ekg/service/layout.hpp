#ifndef EKG_LAYOUT_H
#define EKG_LAYOUT_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include "ekg/layout/grid.hpp"
#include <map>

namespace ekg::service {
    class layout {
    protected:
        float min_offset {};
        float min_height {};

        float scaled_width_divided {};
        float scaled_height_divided {};

        int32_t min_factor_height {};
        uint16_t enum_docks_flag {};

        std::map<uint32_t, ekg::layout::grid> grid_map {};
        ekg::rect curr_top {};
        ekg::rect curr_bottom {};

        ekg::layout::flag prev_flag_top {};
        ekg::layout::flag prev_flag_bottom {};
        ekg::layout::flag curr_flag {};
        ekg::layout::grid curr_grid {};
    public:
        void init();
        void quit();

        void process_scaled(ekg::ui::abstract_widget* widget);
        void process(ekg::ui::abstract_widget* widget);
    };
}

#endif