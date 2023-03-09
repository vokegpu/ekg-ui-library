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

void ekg::dispatch(ekg::cpu::event* event) {
    ekg::core->get_service_handler().dispatch(event);
}

void ekg::dispatch(const ekg::env &env) {
    ekg::core->get_service_handler().task((uint32_t) env);
}

bool ekg::listen(ekg::cpu::uievent &ekg_event, SDL_Event &sdl_event) {
    switch (sdl_event.type) {
        case SDL_USEREVENT: {
            if (sdl_event.user.type == ekg::listener) {
                ekg::log() << "hi event from SDL";

                return true;
            }

            break;
        }
    }

    return false;
}