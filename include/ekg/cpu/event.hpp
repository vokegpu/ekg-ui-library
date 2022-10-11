#ifndef EKG_CPU_EVENT_H
#define EKG_CPU_EVENT_H

#include <iostream>
#include <functional>

namespace ekg::cpu {
    struct event {
        const char* uuid {};
        void* callback {};
        std::function<void(void*)> fun {};
        bool should_free_memory {};
    };
};

#endif