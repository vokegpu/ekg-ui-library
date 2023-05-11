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
#ifndef EKG_UI_TEXTBOX_WIDGET_H
#define EKG_UI_TEXTBOX_WIDGET_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include <unordered_map>

namespace ekg::ui {
    class textbox_widget : public ekg::ui::abstract_widget {
    public:
        enum action {addtext, erasetext, breakline};
    public:
        std::vector<std::string> text_chunk_list {};
        std::string widget_side_text {};
        ekg::rect rect_text {};

        int64_t visible_chunk[2] {};
        int64_t cursor[6] {};

        float scroll[6] {};
        float text_offset {};
        float text_height {};

        bool redraw_cursor {};
    public:
        void check_cursor_text_bounding();
        std::string &get_cursor_emplace_text();
        void process_text(std::string_view text, ekg::ui::textbox_widget::action action, int64_t direction);
        void move_cursor(int64_t, int64_t, bool = false);
        void unset_focus();

        void on_destroy() override;
        void on_reload() override;
        void on_pre_event(SDL_Event &sdl_event) override;
        void on_event(SDL_Event &sdl_event) override;
        void on_post_event(SDL_Event &sdl_event) override;
        void on_update() override;
        void on_draw_refresh() override;
    };
}

#endif