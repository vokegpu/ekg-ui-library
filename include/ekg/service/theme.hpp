#ifndef EKG_SERVICE_THEME_H
#define EKG_SERVICE_THEME_H

#include "ekg/util/geometry.hpp"
#include <vector>

namespace ekg::service {
    class theme {
    protected:
        std::vector<std::string> loaded_theme_list {};
    public:
        /* Start of frame theme. */
        ekg::vec4 frame_background {};
        ekg::vec4 frame_border {};
        ekg::vec4 frame_outline {};

        int32_t frame_resize_offset {};
        int32_t frame_drag_offset {};
        /* End of frame theme. */

        /* Start of button theme. */
        ekg::vec4 button_background {};
        ekg::vec4 button_string {};
        ekg::vec4 button_outline {};
        ekg::vec4 button_activy {};
        /* End of button theme. */

        void init();
        void quit();

        void refresh_theme_list();
        void load_theme(const std::string &theme);
        void gen_default_theme();
    };
}

#endif