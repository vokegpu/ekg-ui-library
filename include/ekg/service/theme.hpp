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
    public:
        /* Start of frame color scheme. */
        ekg::vec4 frame_background {},
                  frame_border {},
                  frame_outline {};
        int32_t frame_activy_offset {};
        /* End of frame color scheme. */

        /* Start of button color scheme. */
        ekg::vec4 button_background {},
                  button_string {},
                  button_outline {},
                  button_activy {},
                  button_highlight {};
        /* End of button color scheme. */

        /* Start of checkbox color scheme. */
        ekg::vec4 checkbox_background {},
                  checkbox_string {},
                  checkbox_outline {},
                  checkbox_activy {},
                  checkbox_highlight {};
        /* End of checkbox color scheme. */

        /* Start of slider color scheme. */
        ekg::vec4 slider_string {},
                  slider_background {},
                  slider_outline {},
                  slider_activy {},
                  slider_activy_bar {},
                  slider_highlight {};
        int32_t slider_bar_thickness {}, slider_target_thickness {};
        /* End of slider color scheme.*/

        /* Start of label color scheme. */
        ekg::vec4 label_string {};
        /* End of label color scheme. */

        /* Start of popup color scheme. */
        ekg::vec4 popup_string {},
                  popup_background {},
                  popup_outline {},
                  popup_highlight {};
        int64_t popup_drop_animation_delay {};
        /* End of popup color scheme. */

        float min_widget_size {5};

        void init();
        void quit();

        void refresh_theme_list();
        void load_theme(const std::string &theme);
        void gen_default_theme();
    };
}

#endif