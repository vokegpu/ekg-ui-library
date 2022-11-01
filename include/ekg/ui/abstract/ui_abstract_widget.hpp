/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, use in any thing (hacking, cheating, games, softwares).
 * 5- All malwares, rat and others virus. We do not care.
 * 6- Do not modify this license under any circunstancie.
 *
 * @VokeGpu 2022 all rights reserved.
 */

#ifndef EKG_UI_ABSTRACT_WIDGET_H
#define EKG_UI_ABSTRACT_WIDGET_H

#include "ekg/ui/abstract/ui_abstract.hpp"
#include <SDL2/SDL.h>

namespace ekg::ui {
    class abstract_widget : public ekg::feature {
    public:
        abstract_widget();
        ~abstract_widget();

        ekg::ui::abstract* data {};
        ekg::flag flag {};
        ekg::rect offset {};
        ekg::rect* parent {};
        ekg::rect extra {};
        ekg::rect layout {};
        ekg::rect ghost {};

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