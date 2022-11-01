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

#ifndef EKG_LAYOUT_GRID_H
#define EKG_LAYOUT_GRID_H

#include "ekg/util/geometry.hpp"

namespace ekg::layout {
    struct grid {
        ekg::rect top {},
                  top_left {},
                  top_center {},
                  top_right {};
        ekg::rect bottom {},
                  bottom_center {},
                  bottom_left {},
                  bottom_right {};
    };

    struct flag {
        bool top {},
             bottom {},
             left {},
             right {},
             center {},
             full {},
             free {},
             none {},
             next {};
    };
}

#endif