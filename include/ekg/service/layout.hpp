/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, free to use in anything (utility, hacking, cheat, game, software).
 * 5- Malware, rat and others virus. We do not care.
 * 6- Do not modify this license under any instance.
 *
 * @VokeGpu 2022 all rights reserved.
 */

#ifndef EKG_LAYOUT_H
#define EKG_LAYOUT_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include "ekg/layout/grid.hpp"
#include <map>

namespace ekg::service {
    class layout {
    protected:
        float min_offset {}, min_height {}, scaled_width_divided {}, scaled_height_divided {};
        int32_t min_factor_height {};
        uint16_t enum_docks_flag {}, layout_mask_flags {};

        std::map<uint32_t, ekg::layout::grid> grid_map {};
        std::vector<ekg::rect*> rect_list {};

        ekg::rect curr_top {}, curr_bottom {}, layout_mask {};
        ekg::layout::flag prev_flag_top {}, prev_flag_bottom {}, curr_flag {}, curr_grid {};
    public:
        void init();
        void quit();

        void add(ekg::rect*);
        void process_layout_mask(const ekg::vec2&, uint16_t);
        ekg::rect &get_layout_mask();

        void process_scaled(ekg::ui::abstract_widget* widget_parent);
        void process(ekg::ui::abstract_widget* widget);
    };
}

#endif