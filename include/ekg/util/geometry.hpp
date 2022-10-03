#pragma once
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

    enum class dock {
        free, top, bottom, right, left, center, full, none
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
    };

    struct docker {
        ekg::rect left {};
        ekg::rect right {};
        ekg::rect top {};
        ekg::rect bottom {};
        ekg::rect area {};

        uint16_t dock {};
    };

    void log(const std::string &log_message);
    void orthographic2d(float* matrix, float left, float right, float bottom, float top);

    bool file_to_string(std::string &string_builder, const std::string &path);
    bool rect_collide_rect(const ekg::rect &rect_a, const ekg::rect &rect_b);
    bool rect_collide_vec(const ekg::rect &rect, const ekg::vec4 &vec);

    uint16_t docker_collide_vec(const ekg::docker &docker, const ekg::vec4 &vec);
    uint16_t docker_collide_rect(const ekg::docker &docker, const ekg::rect &rect);
}

#endif