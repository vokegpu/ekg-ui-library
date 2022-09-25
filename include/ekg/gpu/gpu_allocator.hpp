#pragma once
#ifndef EKG_GPU_ALLOCATOR_H
#define EKG_GPU_ALLOCATOR_H

#include "gpu_data.hpp"
#include <array>

namespace ekg::gpu {
    class allocator {
    protected:
        std::array<ekg::gpu::data, 1024> cpu_allocated_data {};
        std::vector<GLuint> loaded_texture {};

        uint32_t empty_shape_data {};
        uint32_t iterate_ticked_count {};
        uint32_t previous_allocated_size {};

        GLuint buffer_vertex {};
        GLuint buffer_uv {};
        GLuint buffer_index {};
    public:
        void init();
        void invoke();
        void bind_texture(GLuint &texture);
        void dispatch(ekg::gpu::data &data);
        void revoke();
        void draw();
    };
}

#endif