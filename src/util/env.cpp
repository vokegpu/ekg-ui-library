/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, free to use in anything (utility, hacking, cheat, game, software).
 * 5- Malware, rat and others virus. We do not care.
 * 6- Do not modify this license under any instance.
 *
 * @VokeGpu 2022 all rights reserved.
 */

#include "ekg/util/util_event.hpp"
#include "ekg/cpu/info.hpp"
#include "ekg/ekg.hpp"
#include "ekg/util/env.hpp"

#include <SDL2/SDL.h>
#include <fstream>

bool ekg::bitwise::contains(uint16_t target, uint16_t flags) {
    return target & (flags);
}

uint16_t &ekg::bitwise::remove(uint16_t &target, uint16_t flags) {
    target &= ~(flags);
    return target;
}

uint16_t &ekg::bitwise::add(uint16_t &target, uint16_t flags) {
    target |= flags;
    return target;
}

void ekg::log(const std::string &log_message) {
    const std::string &full_log_message = ("[ekg] " + log_message);

    switch (ekg::os) {
        case ekg::platform::os_android: {
            SDL_Log("%s", full_log_message.data());
            break;
        }

        default: {
            std::cout << full_log_message.data() << '\n';
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
    return timing.current_ticks > ms;
}

bool ekg::reset(ekg::timing &timing) {
    timing.elapsed_ticks = timing.ticks_going_on;
    return true;
}

float ekg::lerp(float a, float b, float dt) {
    if (dt < 0 || dt >= 1) {
        return b;
    }

    return a + (b - a) * dt;
}

bool ekg::set(bool &var_mutable, bool predicate) {
    if (var_mutable != predicate) {
        ekg::dispatch(ekg::env::redraw);
    }

    return (var_mutable = predicate);
}

std::string &ekg::set(std::string &var_mutable, const std::string &predicate) {
    if (var_mutable != predicate) {
        ekg::dispatch(ekg::env::redraw);
    }

    return (var_mutable = predicate);
}

std::string ekg::parse_float_precision(float n, int32_t precision) {
    std::string parsed {std::to_string(n)};
    return parsed.substr(0, ekg::max(parsed.find('.') + precision + (1 * precision), parsed.size()));
}

bool ekg::input::pressed(std::string_view key_input) {
    return ekg::core->get_service_input().pressed(key_input);
}

void ekg::input::bind(std::string_view key_input, std::string_view value_input) {
    return ekg::core->get_service_input().bind(key_input, value_input);
}

bool ekg::input::motion() {
    return ekg::core->get_service_input().was_motion();
}

bool ekg::input::released() {
    return ekg::core->get_service_input().was_released();
}

bool ekg::input::pressed() {
    return ekg::core->get_service_input().was_pressed();
}

bool ekg::input::wheel() {
    return ekg::core->get_service_input().was_wheel();
}