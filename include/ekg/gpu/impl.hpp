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

#ifndef EKG_GPU_SCHEME_H
#define EKG_GPU_SCHEME_H

#include <iostream>
#include "data.hpp"

namespace ekg::gpu {
    struct animation {
        bool finished {};
        bool initial {true};
        ekg::gpu::data* data {};
    };

    struct scissor {
        int32_t rect[4] {-1, 0, 0, 0};
    };
}

#endif