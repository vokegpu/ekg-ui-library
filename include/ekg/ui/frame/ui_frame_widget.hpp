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

#ifndef EKG_UI_FRAME_WIDGET_H
#define EKG_UI_FRAME_WIDGET_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"

namespace ekg::ui {
	class frame_widget : public ekg::ui::abstract_widget {
	public:
        uint16_t target_dock_drag {};
        uint16_t target_dock_resize {};
        ekg::docker docker_activy_drag {};
        ekg::docker docker_activy_resize {};
        ekg::rect rect_delta {};

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