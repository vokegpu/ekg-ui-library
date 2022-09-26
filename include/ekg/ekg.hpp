#pragma once
#ifndef EKG_H
#define EKG_H

#include "ekg/core/runtime.hpp"

namespace ekg {
    extern ekg::runtime* core;
    extern std::string gl_version;

    void depth(float depth_preset);
    void demo();
    void init(SDL_Window* root);
    void quit();

    void event(SDL_Event &sdl_event);
    void update();
    void render();
}

#endif