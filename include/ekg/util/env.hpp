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

#ifndef EKG_UTIL_ENV_H
#define EKG_UTIL_ENV_H

#include <iostream>

namespace ekg {
    enum class env {
        refresh, swap, reload, synclayout, redraw, scissor
    };

    enum class cursor {
        normal, arrow_axis_x, arrowleft, arrowp, arrowdown, arrow
    };

    enum class font {
        small = 0, normal = 1, big = 2
    };

    struct flag {
        bool highlight {};
        bool hovered {};
        bool activy {};
        bool focused {};
        bool state {};
        bool extra_state {};
        bool absolute {};
    };

    struct timing {
        uint64_t elapsed_ticks {};
        uint64_t current_ticks {};
        uint64_t ticks_going_on {};
    };

    float lerp(float, float, float);
    void log(const std::string&);
    bool reach(ekg::timing&, uint64_t);
    bool reset(ekg::timing&);

    bool file_to_string(std::string &string_builder, const std::string &path);
    bool set(bool &var_mutable, bool predicate);

    std::string &set(std::string &var_mutable, const std::string &predicate);
    std::string string_float_precision(float, int32_t);

    namespace bitwise {
        bool contains(uint16_t target, uint16_t flag);
        uint16_t &add(uint16_t &target, uint16_t flags);
        uint16_t &remove(uint16_t &target, uint16_t flags);
    }

    namespace input {
        bool pressed(std::string_view);
        void bind(std::string_view, std::string_view);
        bool motion();
        bool released();
        bool pressed();
        bool wheel();
    }
};

#endif