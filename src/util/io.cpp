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
 * @VokeGpu 2023 all rights reserved.
 */

#include "ekg/ekg.hpp"
#include <fstream>

std::string ekg::log::cache {};

bool ekg::bitwise::contains(uint16_t target, uint16_t flags) {
    return target & flags;
}

uint16_t &ekg::bitwise::remove(uint16_t &target, uint16_t flags) {
    target &= ~(flags);
    return target;
}

uint16_t &ekg::bitwise::add(uint16_t &target, uint16_t flags) {
    target |= flags;
    return target;
}

bool ekg::reach(ekg::timing &timing, uint64_t ms) {
    timing.ticks_going_on = SDL_GetTicks64();
    timing.current_ticks = timing.ticks_going_on - timing.elapsed_ticks;
    return timing.current_ticks >= ms;
}

bool ekg::reset(ekg::timing &timing) {
    timing.elapsed_ticks = timing.ticks_going_on;
    return true;
}

bool ekg::file_to_string(std::string &file_content, std::string_view path) {
    std::ifstream ifs {path.data()};
    if (ifs.is_open()) {
        std::string line_content {};
        while (std::getline(ifs, line_content)) {
            file_content += line_content;
            file_content += '\n';
        }

        ifs.close();
        return false;
    } else {
        ekg::log() << "Failed to read file '" << path << "\'!";
    }

    return true;
}

bool ekg::input::action(std::string_view key_input) {
    return ekg::core->service_input.pressed(key_input);
}

void ekg::input::bind(std::string_view key_input, std::string_view value_input) {
    return ekg::core->service_input.bind(key_input, value_input);
}

void ekg::input::fire(std::string_view key_input) {
    ekg::core->service_input.fire(key_input);
}

bool ekg::input::motion() {
    return ekg::core->service_input.has_motion;
}

bool ekg::input::released() {
    return ekg::core->service_input.was_released;
}

bool ekg::input::pressed() {
    return ekg::core->service_input.was_pressed;
}

bool ekg::input::wheel() {
    return ekg::core->service_input.was_wheel;
}

bool ekg::input::receive(std::string_view tag) {
    return ekg::core->service_input.receive(tag);
}

bool ekg::input::typed() {
    return ekg::core->service_input.was_typed;
}

ekg::vec4 &ekg::input::interact() {
    return ekg::core->service_input.interact;
}