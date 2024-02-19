/**
 * MIT License
 * 
 * Copyright (c) 2022-2024 Rina Wilk / vokegpu@gmail.com
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

#include "ekg/ekg.hpp"

bool ekg::operator==(const ekg::rect &l, const ekg::rect &r) {
  return ekg_equals_float(l.x, r.x) && ekg_equals_float(l.y, r.y) && ekg_equals_float(l.w, r.w) &&
         ekg_equals_float(l.h, r.h);
}

bool ekg::operator!=(const ekg::rect &l, const ekg::rect &r) {
  return !(l == r);
}

bool ekg::operator==(const ekg::rect &l, const ekg::vec4 &r) {
  return ekg_equals_float(l.x, r.x) && ekg_equals_float(l.y, r.y) && ekg_equals_float(l.w, r.z) &&
         ekg_equals_float(l.h, r.w);
}

bool ekg::operator!=(const ekg::rect &l, const ekg::vec4 &r) {
  return !(l == r);
}

ekg::rect ekg::operator-(const ekg::rect &l, const ekg::rect &r) {
  return {l.x - r.x, l.y - r.y, l.w, l.h};
}

ekg::rect ekg::operator+(const ekg::rect &l, const ekg::rect &r) {
  return {l.x + r.x, l.y + r.y, l.w, l.h};
}

ekg::rect ekg::operator-(const ekg::rect &l, const ekg::vec4 &r) {
  return {l.x - r.x, l.y - r.y, l.w, l.h};
}

ekg::rect ekg::operator+(const ekg::rect &l, const ekg::vec4 &r) {
  return {l.x + r.x, l.y + r.y, l.w, l.h};
}

ekg::vec4 ekg::operator-(const ekg::vec4 &l, const ekg::vec4 &r) {
  return {l.x - r.x, l.y - r.y, l.z, l.w};
}

ekg::vec4 ekg::operator+(const ekg::vec4 &l, const ekg::vec4 &r) {
  return {l.x + r.x, l.y + r.y, l.z, l.w};
}

ekg::vec4 ekg::operator-(const ekg::vec4 &l, const ekg::rect &r) {
  return {l.x - r.x, l.y - r.y, l.z, l.w};
}

ekg::vec4 ekg::operator+(const ekg::vec4 &l, const ekg::rect &r) {
  return {l.x + r.x, l.y + r.y, l.z, l.w};
}

ekg::vec2 ekg::operator/(const ekg::vec2 &l, float r) {
  return {l.x / r, l.y / r};
}

ekg::vec4 ekg::operator/(const ekg::vec4 &l, float r) {
  return {l.x / r, l.y / r, l.z / r, l.w / r};
}

bool ekg::rect_collide_rect(const ekg::rect &a, const ekg::rect &b) {
  return a.x < b.x + b.w && a.x + a.w > b.x &&
         a.y < b.y + b.h && a.x + a.w > b.y;
}

void ekg::ortho(float *p_matrix, float left, float right, float bottom, float top) {
  const float depth_near = -1.0f;
  const float depth_far = 1.0f;
  const float depth_inv = 1.0f / (depth_far - depth_near);
  const float y_inv = 1.0f / (top - bottom);
  const float x_inv = 1.0f / (right - left);

  p_matrix[0] = 2.0f * x_inv;
  p_matrix[1] = 0.0f;
  p_matrix[2] = 0.0f;
  p_matrix[3] = 0.0f;

  p_matrix[4] = 0.0f;
  p_matrix[5] = 2.0f * y_inv;
  p_matrix[6] = 0.0f;
  p_matrix[7] = 0.0f;

  p_matrix[8] = 0.0f;
  p_matrix[9] = 0.0f;
  p_matrix[10] = -2.0f * depth_inv;
  p_matrix[11] = 0.0f;

  p_matrix[12] = -(right + left) * x_inv;
  p_matrix[13] = -(top + bottom) * y_inv;
  p_matrix[14] = -(depth_far + depth_near) * depth_inv;
  p_matrix[15] = 1.0f;
}

bool ekg::rect_collide_vec_precisely(const ekg::rect &rect, const ekg::vec4 &vec) {
  return vec.x >= rect.x && vec.x <= rect.x + rect.w && vec.y >= rect.y && vec.y <= rect.y + rect.h;
}

bool ekg::rect_collide_vec(const ekg::rect &rect, const ekg::vec4 &vec) {
  return vec.x > rect.x && vec.x < rect.x + rect.w && vec.y > rect.y && vec.y < rect.y + rect.h;
}

void ekg::set_dock_scaled(const ekg::rect &rect, const ekg::vec2 &offset, ekg::docker &docker) {
  docker.rect = rect;

  docker.left.x = rect.x;
  docker.left.y = rect.y;
  docker.left.w = offset.x;
  docker.left.h = rect.h;

  docker.right.w = offset.x;
  docker.right.h = rect.h;
  docker.right.x = rect.x + rect.w - offset.x;
  docker.right.y = rect.y;

  docker.top.x = rect.x;
  docker.top.y = rect.y;
  docker.top.w = rect.w;
  docker.top.h = offset.y;

  docker.bottom.w = rect.w;
  docker.bottom.h = offset.y;
  docker.bottom.x = rect.x;
  docker.bottom.y = rect.y + rect.h - offset.y;

  float dx = offset.x / 2;
  float dy = offset.y / 2;

  docker.center.x = rect.x + (rect.w / 2) - dx;
  docker.center.y = rect.y + (rect.h / 2) - dy;
  docker.center.w = dx;
  docker.center.h = dy;
}

int32_t ekg::find_collide_dock(ekg::docker &docker, uint16_t flags, const ekg::vec4 &vec) {
  if (ekg_bitwise_contains(flags, ekg::dock::full) && ekg::rect_collide_vec(docker.rect, vec)) {
    return ekg::dock::full;
  }

  uint16_t collided {ekg::dock::none};

  collided = (
    ekg_bitwise_contains(flags, ekg::dock::left) &&
    ekg::rect_collide_vec(docker.left, vec) ? ekg_bitwise_add(collided, ekg::dock::left) : collided
  );

  collided = (
    ekg_bitwise_contains(flags, ekg::dock::right) &&
    ekg::rect_collide_vec(docker.right, vec) ? ekg_bitwise_add(collided, ekg::dock::right) : collided
  );

  collided = (
    ekg_bitwise_contains(flags, ekg::dock::top) &&
    ekg::rect_collide_vec(docker.top, vec) ? ekg_bitwise_add(collided, ekg::dock::top) : collided
  );

  collided = (
    ekg_bitwise_contains(flags, ekg::dock::bottom) &&
    ekg::rect_collide_vec(docker.bottom, vec) ? ekg_bitwise_add(collided, ekg::dock::bottom) : collided
  );
  
  collided = (
    ekg_bitwise_contains(flags, ekg::dock::center) &&
    ekg::rect_collide_vec(docker.center, vec) ? ekg_bitwise_add(collided, ekg::dock::center) : collided
  );

  return collided;
}

void ekg::set_rect_clamped(ekg::rect &rect, float min_size) {
  rect.x = ekg_min(rect.x, 0.0f);
  rect.y = ekg_min(rect.y, 0.0f);
  rect.w = ekg_min(rect.w, min_size);
  rect.h = ekg_min(rect.h, min_size);
}

float ekg::find_min_offset(float width, float offset) {
  /*
   * Use initial offset to get the min possible offset.
   * Initial offset value sometime is font height divided by 2.
   */
  float full_rect {width + offset};
  return static_cast<float>(static_cast<int32_t>((full_rect * 0.5f) - (width * 0.5f)));
}

float ekg::smooth(float duration, uint64_t ticks) {
  duration = static_cast<float>(ticks) / duration;
  return ekg_clamp(6.0f * powf(duration, 5) - (15 * powf(duration, 4)) + (10 * powf(duration, 3)), 0.0f, 1.0f);
}

float ekg::lerp(float a, float b, float dt) {
  return a + (b - a) * dt;
}

ekg::vec4 ekg::color(int32_t r, int32_t g, int32_t b, int32_t a) {
  return {static_cast<float>(r) / 255, static_cast<float>(g) / 255, static_cast<float>(b) / 255,
          static_cast<float>(a) / 255};
}