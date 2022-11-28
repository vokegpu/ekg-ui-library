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

#ifndef EKG_UTIL_GEOMETRY_H
#define EKG_UTIL_GEOMETRY_H

#include <iostream>

namespace ekg {
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

    struct vec2 {
        float x {};
        float y {};

        ekg::vec2 operator / (float);
    };

    struct vec3 {
        float x {};
        float y {};
        float z {};
    };

    struct vec4 {
        float x {};
        float y {};
        float z {};
        float w {};

        vec4() = default;

        vec4(float, float, float, float);
        vec4(float, float, const ekg::vec2&);
        vec4(const ekg::vec2&, float, float);
        vec4(const ekg::vec2&, const ekg::vec2&);
        vec4(const ekg::vec4&, float);
    };

    struct rect {
        float x {};
        float y {};
        float w {};
        float h {};

        rect() = default;

        rect(float, float, float, float);
        rect(float, float, const ekg::vec2&);
        rect(const ekg::vec2&, float, float);
        rect(const ekg::vec2&, const ekg::vec2&);

        bool operator ==(const ekg::rect&) const;
        bool operator !=(const ekg::rect&) const;

        bool operator ==(const ekg::vec4&) const;
        bool operator !=(const ekg::vec4&) const;

        ekg::rect operator - (const ekg::rect&) const;
        ekg::rect operator + (const ekg::rect&) const;
    };

    struct docker {
        ekg::rect left {}, right {}, top {}, bottom {}, center {}, rect {};
    };

    float clamp(float, float, float);
    void orthographic2d(float* matrix, float left, float right, float bottom, float top);
    bool rect_collide_rect(const ekg::rect &rect_a, const ekg::rect &rect_b);
    bool rect_collide_vec(const ekg::rect &rect, const ekg::vec4 &vec);

    void set_rect_clamped(ekg::rect &rect, float min_size);
    void set_dock_scaled(const ekg::rect &rect, const ekg::vec2 &offset, ekg::docker &docker);
    int32_t find_collide_dock(ekg::docker &docker, uint16_t flags, const ekg::vec4 &vec);
    float find_min_offset(float text_width, float offset);

    float min(float a, float b);
    float max(float a, float b);
    ekg::vec4 &interact();
}

#endif