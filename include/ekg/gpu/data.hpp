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

#ifndef EKG_GPU_DATA_H
#define EKG_GPU_DATA_H

#include <iostream>
#include <cstdint>

namespace ekg {
    extern int32_t concave;

    namespace gpu {
        struct data {
            float shape_rect[4] {};
            float material_color[4] {};

            uint8_t active_tex_slot {};
            int32_t begin_stride {};
            int32_t end_stride {};
            int32_t line_thickness {};

            int32_t factor {};
            int32_t scissor_id {-1};
            int32_t id {};
        };

        struct scissor {
            float rect[4] {};
        };
    }
}

#endif