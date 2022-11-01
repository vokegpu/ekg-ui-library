/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, use in any thing (hacking, cheating, games, softwares).
 * 5- All malwares, rat and others virus. We do not care.
 * 6- Do not modify this license under any circunstancie.
 *
 * @VokeGpu 2022 all rights reserved.
 */

#ifndef EKG_GPU_ALLOCATOR_H
#define EKG_GPU_ALLOCATOR_H

#include "data.hpp"
#include "impl.hpp"
#include <array>

namespace ekg::gpu {
    class allocator {
    protected:
        std::vector<ekg::gpu::data> cpu_allocated_data {};
        std::vector<ekg::gpu::scissor> cache_scissor {};

        std::vector<GLfloat> loaded_vertex_list {};
        std::vector<GLfloat> loaded_uv_list {};
        std::vector<GLuint> loaded_texture_list {};

        uint32_t allocated_size {};
        uint32_t previous_allocated_size {};

        bool factor_changed {};
        int32_t previous_factor {};

        GLint begin_stride_count {};
        GLint end_stride_count {};

        GLint current_scissor_bind[4] {};
        int32_t scissor_instance_id {-1};

        GLuint buffer_vertex {};
        GLuint buffer_uv {};
        GLuint buffer_list {};
        GLfloat depth_testing_preset {};

        static float viewport[4];
    public:
        static gpu::program program;
        static float orthographicm4[16];

        void init();
        void quit();

        ekg::gpu::data &bind_current_data();
        void clear_current_data();

        ekg::gpu::data* bind_data(int32_t id);
        int32_t get_current_data_id();
        
        ekg::gpu::scissor* bind_scissor(int32_t id);
        int32_t get_instance_scissor_id();

        void invoke_scissor();
        void scissor(int32_t x, int32_t y, int32_t w, int32_t h);
        void revoke_scissor();

        void bind_texture(GLuint &texture);
        void vertex2f(float x, float y);
        void coord2f(float x, float y);

        void invoke();
        void dispatch();
        void revoke();
        void draw();
    };
}

#endif