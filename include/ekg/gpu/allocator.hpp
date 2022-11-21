/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, free to use in anything (utility, hacking, cheat, game, software).
 * 5- Malware, rat and others virus. We do not care.
 * 6- Do not modify this license under any instance.
 *
 * @VokeGpu 2022 all rights reserved.
 */

#ifndef EKG_GPU_ALLOCATOR_H
#define EKG_GPU_ALLOCATOR_H

#include "impl.hpp"
#include <array>
#include <map>

namespace ekg::gpu {
    class allocator {
    protected:
        std::vector<ekg::gpu::data> data_list {};
        std::vector<ekg::gpu::animation*> animation_update_list {};
        std::map<int32_t, ekg::gpu::scissor> scissor_map {};

        std::map<uint32_t, std::vector<ekg::gpu::animation>> animation_map {};
        std::vector<ekg::gpu::animation> *active_animation {nullptr};
        std::map<uint32_t, bool> persistent_animation_ids_map {};
        std::vector<uint32_t> persistent_animation_ids {};

        std::vector<GLfloat> cached_vertices {};
        std::vector<GLfloat> cached_uvs {};
        std::vector<GLuint> cached_textures {};

        uint32_t data_instance_index {}, previous_data_list_size {};
        int32_t animation_index {}, simple_shape_index {-1}, previous_factor {};

        GLint begin_stride_count {};
        GLint end_stride_count {};
        GLint uniform_active_texture_slot {}, uniform_active_texture {}, uniform_enable_scissor {},
              uniform_color {}, uniform_rect {}, uniform_depth {}, uniform_line_thickness {}, uniform_scissor {};

        int32_t scissor_instance_id {-1}, animation_instance_id {};
        GLfloat current_color_pass[4] {};

        GLuint vbo_vertices {}, vbo_uvs {}, vbo_array {};
        GLfloat depth_testing_preset {};

        bool factor_changed {}, simple_shape {};
        static float viewport[4];
        bool check_simple_shape();
    public:
        static gpu::program program;
        static float orthographicm4[16];

        void init();
        void quit();

        ekg::gpu::data &bind_current_data();
        void clear_current_data();

        ekg::gpu::data* get_data_by_id(int32_t id);
        uint32_t get_current_data_id();
        
        ekg::gpu::scissor *get_scissor_by_id(int32_t id);
        uint32_t get_instance_scissor_id();

        void bind_texture(GLuint&);
        void vertex2f(float, float);
        void coord2f(float, float);

        void on_update();
        void invoke();
        void dispatch();
        void revoke();
        void draw();

        /*
         * This method calculate the difference betwen previous pos with new.
         * Reduce tons of iterations for update scissored UI elements.
         */
        void sync_scissor_pos(int32_t, int32_t);
        void bind_scissor(int32_t);
        void bind_off_scissor();

        void bind_animation(int32_t);
        void bind_off_animation();
    };
}

#endif