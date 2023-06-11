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

#ifndef EKG_SERVICE_THEME_H
#define EKG_SERVICE_THEME_H

#include "ekg/util/geometry.hpp"
#include <vector>

namespace ekg::service {
    class theme {
    protected:
        std::vector<std::string> loaded_theme_list {};
        std::string current_theme {};
    public:
        ekg::vec4 frame_background {};
        ekg::vec4 frame_border {};
        ekg::vec4 frame_outline {};
        int32_t frame_activy_offset {};

        ekg::vec4 button_background {};
        ekg::vec4 button_string {};
        ekg::vec4 button_outline {};
        ekg::vec4 button_activy {};
        ekg::vec4 button_highlight {};

        ekg::vec4 checkbox_background {};
        ekg::vec4 checkbox_string {};
        ekg::vec4 checkbox_outline {};
        ekg::vec4 checkbox_activy {};
        ekg::vec4 checkbox_highlight {};

        ekg::vec4 slider_string {};
        ekg::vec4 slider_background {};
        ekg::vec4 slider_outline {};
        ekg::vec4 slider_activy {};
        ekg::vec4 slider_activy_bar {};
        ekg::vec4 slider_highlight {};
        int32_t slider_bar_thickness {};
        int32_t slider_target_thickness {};

        ekg::vec4 label_string {};

        ekg::vec4 popup_string {};
        ekg::vec4 popup_background {};
        ekg::vec4 popup_outline {};
        ekg::vec4 popup_highlight {};
        ekg::vec4 popup_separator {};
        int64_t popup_drop_animation_delay {};

        ekg::vec4 textbox_string {};
        ekg::vec4 textbox_background {};
        ekg::vec4 textbox_outline {};
        ekg::vec4 textbox_cursor {};
        ekg::vec4 textbox_select {};

        ekg::vec4 scrollbar_background {};
        ekg::vec4 scrollbar_highlight {};
        ekg::vec4 scrollbar_activy {};
        ekg::vec4 scrollbar_outline {};
        int32_t scrollbar_pixel_thickness {};

        float min_widget_size {5};
        bool symmetric_layout {};
      public:
        void init();
        void quit();

        std::string get_current_theme_name();

        void refresh_theme_list();
        void load_theme(const std::string &theme);
        void gen_default_light_theme();
        void gen_default_dark_theme();
    };
}

#endif