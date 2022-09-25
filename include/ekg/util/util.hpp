#pragma once
#ifndef EKG_UTIL_H
#define EKG_UTIL_H

#include <iostream>

namespace ekg {
    extern float dt;
    extern char* const empty;

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

    struct timing {
        uint64_t elapsed_ticks {};
        uint64_t current_ticks {};
        uint64_t ticks_going_on {};
    };

    void log(const std::string &log_message);
    bool rect_collide_rect(const ekg::rect &rect_a, const ekg::rect &rect_b);
    bool reach(ekg::timing &timing, uint64_t ms);
    void reset(ekg::timing &timing);
    bool file_to_string(std::string &string_builder, const std::string &path);
}

#endif