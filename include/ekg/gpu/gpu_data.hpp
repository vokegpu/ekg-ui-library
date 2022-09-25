#pragma once
#ifndef EKG_GPU_DATA_H
#define EKG_GPU_DATA_H

#include "gpu_gl.hpp"
#include <vector>

namespace ekg::gpu {
    struct data {
        float rect_area[4] {};
        float scissored_area[4] {};
        float colored_area[4] {};

        GLuint texture {};
        GLuint texture_slot {};
        int32_t factor {};
    };
}

#endif