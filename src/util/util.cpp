#include "ekg/util/util.hpp"
#include "ekg/cpu/cpu_info.hpp"
#include <SDL2/SDL.h>
#include <fstream>

float ekg::dt {};
char* const ekg::empty {};

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

bool ekg::rect_collide_rect(const ekg::rect &rect_a, const ekg::rect &rect_b) {
    return rect_a.x < rect_b.x + rect_b.w && rect_a.x + rect_a.w > rect_b.x &&
           rect_a.y < rect_b.y + rect_b.h && rect_a.x + rect_a.w > rect_b.y;
}

bool ekg::reach(ekg::timing &timing, uint64_t ms) {
    timing.ticks_going_on = SDL_GetTicks64();
    timing.current_ticks = timing.ticks_going_on - timing.elapsed_ticks;

    return timing.current_ticks >= ms;
}

void ekg::reset(ekg::timing &timing) {
    timing.elapsed_ticks = timing.current_ticks;
    timing.current_ticks = SDL_GetTicks64();
}

bool ekg::file_to_string(std::string &string_builder, const std::string &path) {
    std::ifstream ifs {path};

    if (ifs.is_open()) {
        std::string string_buffer {};

        while (getline(ifs, string_buffer)) {
            string_builder += "\n" + string_buffer;
        }

        ifs.close();
        return true;
    }

    return false;
}
