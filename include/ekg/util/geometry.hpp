#ifndef EKG_UTIL_GEOMETRY_H
#define EKG_UTIL_GEOMETRY_H

#include <iostream>

namespace ekg {
    extern char* const empty;

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
        full   = 64
    };

    struct flag {
        bool highlight {};
        bool hovered {};
        bool activy {};
        bool focused {};
        bool state {};
        bool extra_state {};
    };

    struct vec2 {
        float x {};
        float y {};
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
    };

    struct rect {
        float x {};
        float y {};
        float w {};
        float h {};

        bool operator ==(const ekg::rect &rect) const;
        bool operator !=(const ekg::rect &rect) const;
    };

    struct docker {
        ekg::rect left {};
        ekg::rect right {};
        ekg::rect top {};
        ekg::rect bottom {};
        ekg::rect rect {};
    };

    void orthographic2d(float* matrix, float left, float right, float bottom, float top);
    bool rect_collide_rect(const ekg::rect &rect_a, const ekg::rect &rect_b);
    bool rect_collide_vec(const ekg::rect &rect, const ekg::vec4 &vec);

    void set_rect_clamped(ekg::rect &rect, float min_size);
    void set_dock_scaled(const ekg::rect &rect, float offset, ekg::docker &docker);
    int32_t find_collide_dock(ekg::docker &docker, uint16_t flags, const ekg::vec4 &vec);

    ekg::vec4 &interact();
}

#endif