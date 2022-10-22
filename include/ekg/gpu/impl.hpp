#ifndef EKG_GPU_SCHEME_H
#define EKG_GPU_SCHEME_H

#include <iostream>

namespace ekg::gpu {
    struct animation {
        int32_t allocated_slot {};
        uint8_t alpha {};
    };

    struct scissor {
        int32_t area[4];
    };
}

#endif