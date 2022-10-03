#ifndef EKG_UTIL_THREAD_H
#define EKG_UTIL_THREAD_H

#include "ekg/util/env.hpp"
#include <iostream>

namespace ekg {
    enum class thread {
        start, stop
    };

    void process(const std::string &process_tag, const ekg::thread &thread_env);
    void process(const ekg::env &process_env_id, const ekg::thread &thread_env);
}

#endif