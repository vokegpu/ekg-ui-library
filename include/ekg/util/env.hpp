#pragma once
#ifndef EKG_UTIL_ENV_H
#define EKG_UTIL_ENV_H

#include <iostream>

namespace ekg {
    struct timing {
        uint64_t elapsed_ticks {};
        uint64_t current_ticks {};
        uint64_t ticks_going_on {};
    };

    void log(const std::string &log_message);
    bool reach(ekg::timing &timing, uint64_t ms);
    void reset(ekg::timing &timing);

    bool file_to_string(std::string &string_builder, const std::string &path);
    bool set(bool &var_mutable, bool predicate);
    std::string &set(std::string &var_mutable, const std::string &predicate);
};

#endif