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

#ifndef EKG_UI_SCROLL_EMBEDDED_WIDGET_H
#define EKG_UI_SCROLL_EMBEDDED_WIDGET_H

#include "ekg/os/ekg_sdl.hpp"
#include "ekg/util/geometry.hpp"
#include "ekg/util/env.hpp"

namespace ekg::ui {
    class scroll_embedded_widget {
    public:
        std::vector<int32_t> child_id_list {};
        ekg::rect *rect_mother {};
        ekg::rect rect_child {};

        ekg::vec4 scroll {};
        ekg::rect rect_vertical_scroll_bar {};
        ekg::rect rect_horizontal_scroll_bar {};

        ekg::flag flag {};
        int32_t mother_id {};
        int32_t widget_id {};

        bool is_vertical_enabled {};
        bool is_horizontal_enabled {};

        ekg::vec2 acceleration {};
        ekg::vec2 bar_drag {};
    public:
        explicit scroll_embedded_widget();

        void calculate_rect_bar_sizes();
        void clamp_scroll();
        bool is_dragging_bar();
        bool check_activy_state(bool);
        void reset_scroll();
        void check_axis_states();

        void on_destroy();
        void on_reload();
        void on_pre_event(SDL_Event &sdl_event);
        void on_event(SDL_Event &sdl_event);
        void on_post_event(SDL_Event &sdl_event);
        void on_update();
        void on_draw_refresh();
    };
}

#endif