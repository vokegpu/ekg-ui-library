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
    ekg::core->service_handler.generate() = *event;
}

void ekg::dispatch(const ekg::env &env) {
    switch (env) {
    case ekg::env::redraw:
        ekg::core->redraw_gui();
        break;
    default:
        ekg::core->service_handler.dispatch_pre_allocated_task((uint64_t) env);
        break;
    }
}

bool ekg::listen(ekg::cpu::uievent &ekg_event, SDL_Event &sdl_event) {
    if (sdl_event.type == ekg::listener) {
        ekg::cpu::uievent *p_ekg_event {static_cast<ekg::cpu::uievent*>(sdl_event.user.data1)};
        ekg_event = *p_ekg_event;
        delete p_ekg_event;
        return true;
    }

    return false;
}