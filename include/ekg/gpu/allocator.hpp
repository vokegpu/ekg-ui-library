#ifndef EKG_GPU_ALLOCATOR_H
#define EKG_GPU_ALLOCATOR_H

#include "data.hpp"
#include <array>

namespace ekg::gpu {
    class allocator {
    protected:
        std::array<ekg::gpu::data, 1024> cpu_allocated_data {};
        std::vector<GLfloat> loaded_vertex_list {};
        std::vector<GLfloat> loaded_uv_list {};
        std::vector<GLuint> loaded_texture_list {};

        uint32_t iterate_ticked_count {};
        uint32_t previous_allocated_size {};

        GLint begin_stride_count {};
        GLint end_stride_count {};

        GLint current_scissor_bind[4] {};
        GLuint buffer_vertex {};
        GLuint buffer_uv {};
        GLuint buffer_list {};
        GLfloat depth_testing_preset {0.001f};

        static gpu::program program;
        static float orthographicm4[16];
    public:
        void init();
        void quit();

        ekg::gpu::data &bind_current_data();
        void clear_current_data();

        void bind_texture(GLuint &texture);
        void bind_scissor(int32_t x, int32_t y, int32_t w, int32_t h);
        void vertex2f(float x, float y);
        void coord2f(float x, float y);

        void invoke();
        void dispatch();
        void revoke();
        void draw();
    };
}

#endif