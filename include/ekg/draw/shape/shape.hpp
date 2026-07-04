/**
 * MIT License
 * 
 * Copyright (c) 2022-2025 Rina Wilk / vokegpu@gmail.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef EKG_DRAW_SHAPE_HPP
#define EKG_DRAW_SHAPE_HPP

#include "ekg/math/geometry.hpp"
#include "ekg/gpu/sampler.hpp"

namespace ekg::draw {
  enum mode : int8_t {
    fill = 0,
    outline = 1
  };

  void rect(
    const ekg::rect_t<float> &rect,
    const ekg::rgba_t<uint8_t> &color,
    ekg::pixel_thickness_t line_thickness,
    ekg::at_t &sampler_at
  );

  void rect(
    float x, float y, float w, float h,
    const ekg::rgba_t<uint8_t> &color,
    ekg::pixel_thickness_t line_thickness,
    ekg::at_t &sampler_at
  );

  void scissor(
    const ekg::rect_t<float> &rect
  );

  void scissor(
    float x, float y, float w, float h
  );
}

#endif
