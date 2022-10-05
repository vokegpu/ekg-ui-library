#ifndef EKG_UI_FRAME_WIDGET_H
#define EKG_UI_FRAME_WIDGET_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"

namespace ekg::ui {
	class frame_widget : public ekg::ui::abstract_widget {
	public:
        uint16_t target_dock_drag {};
        uint16_t target_dock_resize {};

		void destroy() override;
		void on_reload() override;
		void on_pre_event(SDL_Event &sdl_event) override;
		void on_event(SDL_Event &sdl_event) override;
		void on_post_event(SDL_Event &sdl_event) override;
		void on_update() override;
		void on_draw_refresh() override;
	};
}

#endif