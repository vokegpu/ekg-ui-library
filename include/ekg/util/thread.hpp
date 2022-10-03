#pragma once
#ifndef EKG_UTIL_THREAD_H
#define EKG_UTIL_THREAD_H

#include <iostream>

namespace ekg {
    enum class thread {
        start, stop
    };

    enum class env {
        swap, refresh, reset, update, redraw
    };

    void process(const std::string &process_tag, const ekg::thread &thread_env);
    void process(const ekg::env &process_env_id, const ekg::thread &thread_env);
}

#endif