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
 * @VokeGpu 2023 all rights reserved.
 */

#ifndef EKG_LAYOUT_H
#define EKG_LAYOUT_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include <map>

namespace ekg::service {
    class layout {
    protected:
        float min_offset {};
        float min_fill_width {};
        float min_height {};
        float scaled_width_divided {};
        float scaled_height_divided {};
        float respective_mask_all {};
        float respective_mask_center {}; 
        float extent_data[4] {};

        int32_t min_factor_height {};
        uint16_t enum_docks_flag {};
        uint16_t layout_mask_flags {};

        ekg::axis dock_axis_mask {};
        ekg::vec3 offset_mask {};
        std::vector<ekg::dockrect> dockrect_list {};
        ekg::rect layout_mask {};

        ekg::vec2 viewport_scale {0.0f, 0.0f};
        float scale_factor {};
    public:
        void init();
        void quit();

        void set_preset_mask(const ekg::vec3&, ekg::axis, float = 0.0f);
        void insert_into_mask(const ekg::dockrect&);
        void process_layout_mask();
        ekg::rect &get_layout_mask();

        float get_min_offset();
        float get_respective_mask_size();
        void update_scale_factor();
        float get_scale_factor();

        float get_dimensional_extent(ekg::ui::abstract_widget *widget, uint16_t flag_ok, uint16_t flag_stop, int64_t &begin_and_count, ekg::axis axis);
        void process_scaled(ekg::ui::abstract_widget* widget_parent);
        void process(ekg::ui::abstract_widget *pwidget);
    };
}

#endif