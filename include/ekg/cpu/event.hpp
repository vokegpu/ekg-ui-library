#ifndef EKG_CPU_EVENT_H
#define EKG_CPU_EVENT_H

#include <iostream>
#include <functional>

namespace ekg {
    enum event {
        unique = 0,
        alloc = 2,
        allocated = 4,
        shared = 8
    };

    namespace cpu {
        struct event {
            const char *uuid {};
            void* callback {};
            std::function<void(void*)> fun {};
            uint16_t flags {};
        };
    }
}

#endif