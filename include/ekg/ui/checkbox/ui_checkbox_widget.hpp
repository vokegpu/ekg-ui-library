#ifndef EKG_UI_CHECKBOX_WIDGET_H
#define EKG_UI_CHECKBOX_WIDGET_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"

namespace ekg::ui {
    class checkbox_widget : public ekg::ui::abstract_widget {
    public:
        ekg::docker docker_text {};

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