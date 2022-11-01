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
    void rect(const ekg::rect &rect, const ekg::vec4 &color, int32_t line_thickness = 0);
    void rect(float x, float y, float w, float h, const ekg::vec4 &color, int32_t line_thickness = 0);

    void scissor(int32_t x, int32_t y, int32_t w, int32_t h);
    void pop_scissor();
}

#endif