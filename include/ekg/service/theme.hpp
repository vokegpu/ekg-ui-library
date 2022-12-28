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
        /* Start of frame scheme colors. */
        ekg::vec4 frame_background {},
                        frame_border {},
                        frame_outline {};
        int32_t frame_activy_offset {};
        /* End of frame scheme colors. */

        /* Start of button scheme colors. */
        ekg::vec4 button_background {},
                        button_string {},
                        button_outline {},
                        button_activy {},
                        button_highlight {};
        /* End of button scheme colors. */

        /* Start of checkbox scheme colors. */
        ekg::vec4 checkbox_background {},
                        checkbox_string {},
                        checkbox_outline {},
                        checkbox_activy {},
                        checkbox_highlight {};
        /* End of checkbox theme. */

        /* Start of slider scheme colors. */
        ekg::vec4 slider_string {},
                        slider_background {},
                        slider_outline {},
                        slider_activy {},
                        slider_activy_bar {},
                        slider_highlight {};
        int32_t slider_bar_thicnkess {}, slider_target_thickness {};
        /* End of slider scheme colors.*/

        /* Start of label scheme colors. */
        ekg::vec4 label_string {};
        /* End of label scheme colors. */

        float min_widget_size {5};

        void init();
        void quit();

        void refresh_theme_list();
        void load_theme(const std::string &theme);
        void gen_default_theme();
    };
}

#endif