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

#ifndef EKG_OS_SYSTEM_CURSOR_H
#define EKG_OS_SYSTEM_CURSOR_H

#include "ekg/os/ekg_sdl.hpp"

namespace ekg {
    enum class systemcursor {
        arrow, ibeam, wait, crosshair, waitarrow, sizenwse, sizenesw, sizewe, sizens, sizeall, no, hand
    };

    extern ekg::systemcursor cursor;
    static SDL_Cursor *loadedsystemcursorlist[12];

    void init_cursor();
    void set_cursor(ekg::systemcursor);
};

#endif