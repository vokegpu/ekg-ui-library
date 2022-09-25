#pragma once
#ifndef EKG_GPU_DATA_H
#define EKG_GPU_DATA_H

#include "gpu_gl.hpp"
#include <vector>

namespace ekg::gpu {
    struct data {
        GLfloat rect_area[4] {};
        GLfloat colored_area[4] {};
        GLint scissored_area[4] {-1, -1, -1, -1};

        GLuint texture {};
        GLuint texture_slot {};

        GLint begin_stride {};
        GLint end_stride {};
        GLint outline[2] {0, 1};

        int32_t factor {};
    };
}

#endif