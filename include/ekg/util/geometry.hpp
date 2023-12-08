/*
* MIT License
* 
* Copyright (c) 2022-2023 Rina Wilk / vokegpu@gmail.com
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

#ifndef EKG_UTIL_GEOMETRY_H
#define EKG_UTIL_GEOMETRY_H

#include <iostream>
#include <cstdint>
#include <cfloat>
#include <cmath>

#define ekg_equals_float(x, y) ((fabsf((x) - (y))) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))))
#define ekg_pixel_div_2        (0.500000f)

namespace ekg {
  extern double pi;

  struct display {
  public:
    static float dt;
    static int32_t width;
    static int32_t height;
  };

  enum dock {
    none = 0,
    free = 2 << 2,
    top = 2 << 3,
    bottom = 2 << 4,
    right = 2 << 5,
    left = 2 << 6,
    center = 2 << 7,
    full = 2 << 8,
    next = 2 << 9,
    fill = 2 << 10,
    resize = 2 << 11
  };

  /*
   * Enum linked to dock.
   */
  enum axis {
    vertical = 12 << 2,
    horizontal = 13 << 2
  };

  typedef struct vec2 {
  public:
    union {
      struct {
        float x {};
        float y {};
      };
    };
  public:
    inline vec2() {};

    inline vec2(float _x, float _y) {
      this->x = _x;
      this->y = _y;
    };
  } vec2;

  ekg::vec2 operator/(const ekg::vec2 &, float);

  typedef struct vec3 {
  public:
    union {
      struct {
        float x {};
        float y {};
        float z {};
      };
    };
  public:
    inline vec3() {};

    inline vec3(float _x, float _y, float _z) {
      this->x = _x;
      this->y = _y;
      this->z = _z;
    }
  } vec3;

  typedef struct vec4 {
  public:
    union {
      struct {
        float x {};
        float y {};
        float z {};
        float w {};
      };
    };
  public:
    inline vec4() {};

    inline vec4(float _x, float _y, float _z, float _w) {
      this->x = _x;
      this->y = _y;
      this->z = _z;
      this->w = _w;
    }

    inline vec4(float _x, float _y, const ekg::vec2 &_v) {
      this->x = _x;
      this->y = _y;
      this->z = _v.x;
      this->w = _v.y;
    }

    inline vec4(const ekg::vec2 &_v, float _z, float _w) {
      this->x = _v.x;
      this->y = _v.y;
      this->z = _z;
      this->w = _w;
    }

    inline vec4(const ekg::vec2 &l, const ekg::vec2 &r) {
      this->x = l.x;
      this->y = l.y;
      this->z = r.x;
      this->w = r.y;
    }

    inline vec4(const ekg::vec4 &_v, float _z) {
      this->x = _v.x;
      this->y = _v.y;
      this->z = _v.z;
      this->w = _z;
    }
  } vec4;

  typedef struct rect {
  public:
    union {
      struct {
        float x {};
        float y {};
        float w {};
        float h {};
      };
    };
  public:
    inline rect() {};

    inline rect(float _x, float _y, float _w, float _h) {
      this->x = _x;
      this->y = _y;
      this->w = _w;
      this->h = _h;
    }

    inline rect(float _x, float _y, const ekg::vec2 &_v) {
      this->x = _x;
      this->y = _y;
      this->w = _v.x;
      this->h = _v.y;
    }

    inline rect(const ekg::vec2 &_v, float _w, float _h) {
      this->x = _v.x;
      this->y = _v.y;
      this->w = _w;
      this->h = _h;
    }

    inline rect(const ekg::vec2 &_l, const ekg::vec2 &_r) {
      this->x = _l.x;
      this->y = _l.y;
      this->w = _r.x;
      this->h = _r.y;
    }
  } rect;

  bool operator==(const ekg::rect &, const ekg::rect &);

  bool operator!=(const ekg::rect &, const ekg::rect &);

  bool operator==(const ekg::rect &l, const ekg::vec4 &r);

  bool operator!=(const ekg::rect &l, const ekg::vec4 &r);

  ekg::rect operator-(const ekg::rect &l, const ekg::rect &r);

  ekg::rect operator+(const ekg::rect &l, const ekg::rect &r);

  ekg::rect operator-(const ekg::rect &l, const ekg::vec4 &r);

  ekg::rect operator+(const ekg::rect &l, const ekg::vec4 &r);

  ekg::vec4 operator-(const ekg::vec4 &l, const ekg::vec4 &r);

  ekg::vec4 operator+(const ekg::vec4 &l, const ekg::vec4 &r);

  ekg::vec4 operator-(const ekg::vec4 &l, const ekg::rect &r);

  ekg::vec4 operator+(const ekg::vec4 &l, const ekg::rect &r);

  ekg::vec4 operator/(const ekg::vec4 &l, float r);

  struct docker {
  public:
    ekg::rect left {};
    ekg::rect right {};
    ekg::rect top {};
    ekg::rect bottom {};
    ekg::rect center {};
    ekg::rect rect {};
  };

  struct dock_rect {
  public:
    ekg::rect *p_rect {};
    uint16_t dock {};
  };

  struct component {
  public:
    ekg::rect rect_dimension {};
    ekg::rect rect_box {};
    ekg::rect rect_text {};
    ekg::rect rect_dimension_closed {};
    ekg::rect rect_dimension_opened {};

    bool is_open {};
    bool is_hovering {};
  };

  float clamp(float, float, float);

  void orthographic2d(float *, float, float, float, float);

  bool rect_collide_rect(const ekg::rect &, const ekg::rect &);

  bool rect_collide_vec(const ekg::rect &, const ekg::vec4 &);

  bool rect_collide_vec_precisely(const ekg::rect &, const ekg::vec4 &);

  void set_rect_clamped(ekg::rect &, float);

  void set_dock_scaled(const ekg::rect &, const ekg::vec2 &, ekg::docker &);

  int32_t find_collide_dock(ekg::docker &, uint16_t, const ekg::vec4 &);

  float find_min_offset(float, float);

  int32_t min(int32_t, int32_t);

  int32_t max(int32_t, int32_t);

  uint64_t min(uint64_t, uint64_t);

  uint64_t max(uint64_t, uint64_t);

  int64_t min(int64_t, int64_t);

  int64_t max(int64_t, int64_t);

  ekg::vec4 color(int32_t, int32_t, int32_t, int32_t);

  float min(float, float);

  float max(float, float);

  float smooth(float, uint64_t);

  float lerp(float, float, float);
}

#endif