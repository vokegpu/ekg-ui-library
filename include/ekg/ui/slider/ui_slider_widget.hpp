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

#ifndef EKG_UI_SLIDER_WIDGET_H
#define EKG_UI_SLIDER_WIDGET_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"

namespace ekg::ui {
	class sliderwidget : public ekg::ui::abstract_widget {
    protected:
        void update_bar(float, float);
        void update_bar_axis(ekg::dock);
	public:
        ekg::rect rect_target {};
        ekg::rect rect_text {};
        ekg::rect rect_bar {};
        ekg::rect rect_bar_value {};
        std::string string_value {};
        ekg::font font_render_size {};

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