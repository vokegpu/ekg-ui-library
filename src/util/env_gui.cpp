#include "ekg/util/env.hpp"
#include "ekg/util/thread.hpp"

#include <SDL2/SDL.h>

bool ekg::reach(ekg::timing &timing, uint64_t ms) {
    timing.ticks_going_on = SDL_GetTicks64();
    timing.current_ticks = timing.ticks_going_on - timing.elapsed_ticks;

    return timing.current_ticks >= ms;
}

void ekg::reset(ekg::timing &timing) {
    timing.elapsed_ticks = timing.current_ticks;
    timing.current_ticks = SDL_GetTicks64();
}

bool ekg::set(bool &var_mutable, bool predicate) {
    if (var_mutable != predicate) {
        ekg::process(ekg::env::redraw, ekg::thread::start);
    }

    return (var_mutable = predicate);
}

std::string &ekg::set(std::string &var_mutable, const std::string &predicate) {
    if (var_mutable != predicate) {
        ekg::process(ekg::env::redraw, ekg::thread::start);
    }

    return (var_mutable = predicate);
}
