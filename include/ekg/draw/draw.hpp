#pragma once
#ifndef EKG_DRAW_H
#define EKG_DRAW_H

#include "ekg/util/geometry.hpp"

namespace ekg::draw {
    void rect(const ekg::rect &rect, const ekg::vec4 &color, int32_t line_thickness = 0);
    void rect(int32_t x, int32_t y, int32_t w, int32_t h, const ekg::vec4 &color, int32_t line_thickness = 0);
    void scissor(int32_t x, int32_t y, int32_t w, int32_t h);
    void pop_scissor();
}

#endif