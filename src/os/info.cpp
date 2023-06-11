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

#include "ekg/os/info.hpp"
#include "ekg/os/ekg_sdl.hpp"

ekg::platform ekg::os {};

void ekg::os_get_monitor_resolution(float &w, float &h) {
    SDL_DisplayMode sdl_display_mode {};
    SDL_GetDisplayMode(0, 0, &sdl_display_mode);
    w = sdl_display_mode.w;
    h = sdl_display_mode.h;
}