#include "ekg/cpu/cpu_input.hpp"

ekg::vec4 ekg::interact {};
ekg::timing ekg::interact_cooldown {};

void ekg::cpu::process_input(SDL_Event &sdl_event) {
    if (ekg::cpu::down(sdl_event, ekg::input::right) || ekg::cpu::motion(sdl_event)) {
        ekg::reset(ekg::interact_cooldown);
    }
}

bool ekg::cpu::motion(SDL_Event &sdl_event) {
    switch (sdl_event.type) {
        case SDL_MOUSEMOTION: {
            ekg::interact.x = static_cast<float>(sdl_event.motion.x);
            ekg::interact.y = static_cast<float>(sdl_event.motion.y);
            return true;
        }

        case SDL_FINGERMOTION: {
            ekg::interact.x = static_cast<float>(sdl_event.motion.x);
            ekg::interact.y = static_cast<float>(sdl_event.motion.y);

            return true;
        }
    }

    return false;
}

bool ekg::cpu::up(SDL_Event &sdl_event, ekg::input input) {
    bool flag {};

    switch (sdl_event.type) {
        case SDL_MOUSEBUTTONUP: {
            switch (sdl_event.button.button) {
                case SDL_BUTTON_LEFT: {
                    flag = input == ekg::input::left;
                    break;
                }

                case SDL_BUTTON_MIDDLE: {
                    flag = input == ekg::input::middle;
                    break;
                }

                case SDL_BUTTON_RIGHT: {
                    flag = input == ekg::input::right;
                    break;
                }
            }

            break;
        }
        
        case SDL_FINGERUP: {
            uint64_t cooldown_time {};

            switch (input) {
                case ekg::input::left: {
                    cooldown_time = (uint64_t) ekg::cooldown::left;
                    break;
                }

                case ekg::input::middle: {
                    cooldown_time = (uint64_t) ekg::cooldown::middle;
                    break;
                }

                default: {
                    cooldown_time = (uint64_t) ekg::cooldown::right;
                    break;
                }
            }
            
            flag = ekg::reach(ekg::interact_cooldown, cooldown_time);
        }
    }

    return flag;
}

bool ekg::cpu::down(SDL_Event &sdl_event, ekg::input input) {
    bool flag {};

    switch (sdl_event.type) {
        case SDL_MOUSEBUTTONUP: {
            switch (sdl_event.button.button) {
                case SDL_BUTTON_LEFT: {
                    flag = input == ekg::input::left;
                    break;
                }

                case SDL_BUTTON_MIDDLE: {
                    flag = input == ekg::input::middle;
                    break;
                }

                case SDL_BUTTON_RIGHT: {
                    flag = input == ekg::input::right;
                    break;
                }
            }

            break;
        }

        case SDL_FINGERUP: {
            flag = true;
            break;
        }
    }

    return flag;
}