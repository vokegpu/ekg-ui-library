#include "ekg/util/util.hpp"
#include "ekg/cpu/cpu_info.hpp"
#include <SDL2/SDL.h>

float ekg::dt {};
char* const ekg::empty {""};

void ekg::log(const std::string &log_message) {
    const std::string full_log_message = "[ekg] " + log_message;

    switch (ekg::os) {
        case ekg::platform::os_android: {
            SDL_Log("%s", full_log_message.c_str());
            break;
        }

        default: {
            std::cout << full_log_message.c_str() << '\n';
            break;
        }
    }
}

bool ekg::util::rect_collide_rect(const ekg::rect &rect_a, const ekg::rect &rect_b) {
    return rect_a.x < rect_b.x + rect_b.w && rect_a.x + rect_a.w > rect_b.x &&
           rect_a.y < rect_b.y + rect_b.h && rect_a.x + rect_a.w > rect_b.y;
}

bool ekg::util::reach(ekg::timing &timing, uint64_t ms) {
    timing.ticks_going_on = SDL_GetTicks64();
    timing.current_ticks = timing.ticks_going_on - timing.elapsed_ticks;

    return timing.current_ticks >= ms;
}

bool ekg::util::reset(ekg::timing &timing) {
    timing.elapsed_ticks = timing.current_ticks;
    timing.current_ticks = SDL_GetTicks64();

    return true;
}
