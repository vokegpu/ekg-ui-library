#pragma once
#ifndef EKG_H
#define EKG_H

#include "ekg/core/runtime.hpp"

namespace ekg {
    extern ekg::runtime* core;

    void demo();
    void init(SDL_Window* root);
    void quit();

    void event(SDL_Event &sdl_event);
    void update();
    void render();
}

#endif