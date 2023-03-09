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

#ifndef EKG_UTIL_GEOMETRY_H
#define EKG_UTIL_GEOMETRY_H

#include <iostream>
#include <cfloat>
#include <cmath>

#define EQUALS_FLOAT(x, y) (\
(fabsf((x) - (y))) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))))

namespace ekg {
    extern double pi;

    struct display {
        static float dt;
        static int32_t width;
        static int32_t height;
    };

    enum dock {
        none   = 0,
        free   = 1,
        top    = 2,
        bottom = 4,
        right  = 8,
        left   = 16,
        center = 32,
        full   = 64,
        next   = 128
    };

    /*
     * Enum linked to dock.
     */
    enum axis {
        vertical   = 256,
        horizontal = 512
    };

    typedef struct vec2 {
        union {
            struct {
                float x;
                float y;
            };
        };

        inline vec2() = default;
    } vec2;

    ekg::vec2 operator/(const ekg::vec2&, float);

    typedef struct vec3 {
        union {
            struct {
                float x;
                float y;
                float z;
            };
        };
    } vec3;

    typedef struct vec4 {
        union {
            struct {
                float x;
                float y;
                float z;
                float w;
            };

            float data[4];
        };

        inline vec4() = default;
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
        union {
            struct {
                float x;
                float y;
                float w;
                float h;
            };

            float data[4] {};
        };

        inline rect() = default;
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

    bool operator ==(const ekg::rect&, const ekg::rect&);
    bool operator !=(const ekg::rect&, const ekg::rect&);

    bool operator ==(const ekg::rect &l, const ekg::vec4 &r);
    bool operator !=(const ekg::rect &l, const ekg::vec4 &r);

    ekg::rect operator-(const ekg::rect &l, const ekg::rect &r);
    ekg::rect operator+(const ekg::rect &l, const ekg::rect &r);

    struct docker {
        ekg::rect left {}, right {}, top {}, bottom {}, center {}, rect {};
    };

    struct grid {
        ekg::rect top {},
                  top_left {},
                  top_center {},
                  top_right {};
        ekg::rect bottom {},
                  bottom_center {},
                  bottom_left {},
                  bottom_right {};
    };

    struct docking {
        bool top {},
             bottom {},
             left {},
             right {},
             center {},
             full {},
             free {},
             none {},
             next {};
    };

    struct dockrect {
        ekg::rect *rect {nullptr};
        uint16_t dock {};        
    };

    float clamp(float, float, float);
    void orthographic2d(float*, float, float, float, float);
    bool rect_collide_rect(const ekg::rect&, const ekg::rect&);
    bool rect_collide_vec(const ekg::rect&, const ekg::vec4&);

    void set_rect_clamped(ekg::rect&, float);
    void set_dock_scaled(const ekg::rect&, const ekg::vec2&, ekg::docker&);
    
    int32_t find_collide_dock(ekg::docker&, uint16_t, const ekg::vec4&);
    float find_min_offset(float, float);

    int32_t min(int32_t, int32_t);
    int32_t max(int32_t, int32_t);

    float min(float, float);
    float max(float, float);
    float smooth(float, uint64_t);

    ekg::vec4 &interact();
}

#endif