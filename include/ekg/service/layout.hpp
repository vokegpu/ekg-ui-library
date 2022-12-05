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
#include <map>

namespace ekg::service {
    class layout {
    protected:
        float min_offset {}, min_height {}, scaled_width_divided {}, scaled_height_divided {}, respective_mask_all {}, respective_mask_center {};
        int32_t min_factor_height {};
        uint16_t enum_docks_flag {}, layout_mask_flags {};
        ekg::axis dock_axis_mask {};
        ekg::vec3 offset_mask {};

        std::map<uint32_t, ekg::grid> grid_map {};
        std::vector<ekg::dockrect> dockrect_list {};

        ekg::rect curr_top {}, curr_bottom {}, layout_mask {};
        ekg::grid curr_grid {};
        ekg::docking prev_docking_top {}, curr_docking {}, prev_docking_bottom {};
    public:
        void init();
        void quit();

        void set_preset_mask(const ekg::vec3&, ekg::axis, float = 0.0f);
        void insert_into_mask(const ekg::dockrect&);
        void process_layout_mask();
        ekg::rect &get_layout_mask();
        float get_respective_mask_size();
        float get_respective_mask_center();

        void process_scaled(ekg::ui::abstract_widget* widget_parent);
        void process(ekg::ui::abstract_widget* widget);
    };
}

#endif