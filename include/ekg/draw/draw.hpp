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

#ifndef EKG_DRAW_H
#define EKG_DRAW_H

#include "ekg/util/geometry.hpp"

namespace ekg::draw {
    void rect(const ekg::rect&, const ekg::vec4&, int32_t = 0);
    void rect(float, float, float, float, const ekg::vec4&, int32_t = 0);

    void sync_scissor_pos(int32_t, int32_t);
    void bind_scissor(int32_t);
    void bind_off_scissor();

    void bind_animation(int32_t);
    void bind_off_animation();
}

#endif