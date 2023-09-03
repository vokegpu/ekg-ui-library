/*
* MIT License
* 
* Copyright (c) 2022-2023 Rina Wilk / vokegpu@gmail.com
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#ifndef EKG_UI_FRAME_WIDGET_H
#define EKG_UI_FRAME_WIDGET_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include "ekg/ui/scroll/ui_scroll_embedded_widget.hpp"

namespace ekg::ui {
	class frame_widget : public ekg::ui::abstract_widget {
	public:
        uint16_t target_dock_drag {};
        uint16_t target_dock_resize {};

        ekg::ui::scroll_embedded_widget *p_scroll_embedded {};
        ekg::docker docker_activy_drag {};
        ekg::docker docker_activy_resize {};
        ekg::rect rect_delta {};

        float ui_theme_activy_offset {};
    public:
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