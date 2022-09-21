#pragma once
#ifndef EKG_CPU_INPUT_H
#define EKG_CPU_INPUT_H

#include "ekg/util/util.hpp"
#include <string>
#include <map>
#include <SDL2/SDL.h>

namespace ekg {
    extern ekg::vec4 interact;
    extern ekg::timing interact_cooldown;

    enum class input {
        detect, left, middle, right
    };

    enum class cooldown {
        left, middle, right = 500
    };

    namespace cpu {
        void process_input(SDL_Event &sdl_event);

        bool motion(SDL_Event &sdl_event);
        bool up(SDL_Event &sdl_event, ekg::input input = ekg::input::detect);
        bool down(SDL_Event &sdl_event, ekg::input input = ekg::input::detect);
    }
};

#endif