/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, use in any thing (hacking, cheating, games, softwares).
 * 5- All malwares, rat and others virus. We do not care.
 * 6- Do not modify this license under any circunstancie.
 *
 * @VokeGpu 2022 all rights reserved.
 */

#ifndef EKG_GPU_SCHEME_H
#define EKG_GPU_SCHEME_H

#include <iostream>

namespace ekg::gpu {
    struct animation {
        int32_t allocated_slot {};
        uint8_t alpha {};
    };

    struct scissor {
        int32_t rect[4] {-1, 0, 0, 0};
    };
}

#endif