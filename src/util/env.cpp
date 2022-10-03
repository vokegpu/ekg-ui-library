#include "ekg/util/thread.hpp"
#include "ekg/cpu/info.hpp"
#include <SDL2/SDL.h>
#include <fstream>

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
