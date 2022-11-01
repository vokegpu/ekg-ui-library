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

#ifndef EKG_GPU_DATA_H
#define EKG_GPU_DATA_H

#include "gl.hpp"
#include <vector>

namespace ekg::gpu {
    struct data {
        GLfloat rect_area[4] {};
        GLfloat colored_area[4] {};

        GLuint texture {};
        GLuint texture_slot {};

        GLint begin_stride {};
        GLint end_stride {};
        GLint outline {};

        int32_t factor {};
        int32_t scissor_id {-1};
    };
}

#endif