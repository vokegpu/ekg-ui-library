#pragma once
#ifndef EKG_UI_ABSTRACT_WIDGET_H
#define EKG_UI_ABSTRACT_WIDGET_H

#include "ekg/util/geometry.hpp"
#include "ekg/ui/abstract/ui_abstract.hpp"
#include <SDL2/SDL.h>

namespace ekg::ui {
    class abstract_widget {
    public:
        abstract_widget();
        ~abstract_widget();

        ekg::ui::abstract* data {};
        ekg::flag flag {};
        ekg::rect offset {};
        ekg::rect rect {};
        ekg::rect parent {};
        ekg::rect extra {};

        virtual void destroy();
        virtual void on_reload();
        virtual void on_pre_event(SDL_Event &sdl_event);
        virtual void on_event(SDL_Event &sdl_event);
        virtual void on_post_event(SDL_Event &sdl_event);
        virtual void on_update();
        virtual void on_draw_refresh();
    };
}

#endif