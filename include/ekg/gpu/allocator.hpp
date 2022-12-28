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
 * @VokeGpu 2023 all rights reserved.
 */

#ifndef EKG_GPU_ALLOCATOR_H
#define EKG_GPU_ALLOCATOR_H

#include "ekg/gpu/data.hpp"
#include "ekg/gpu/gl.hpp"
#include <array>
#include <map>
#include <vector>

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

        std::vector<float> cached_vertices {};
        std::vector<float> cached_uvs {};
        std::vector<uint32_t> cached_textures {};

        uint32_t data_instance_index {}, previous_data_list_size {};
        int32_t animation_index {}, simple_shape_index {-1}, previous_factor {};

        int32_t begin_stride_count {},
                end_stride_count {};
        int32_t uniform_active_texture_slot {}, uniform_active_texture {}, uniform_enable_scissor {},
                uniform_color {}, uniform_rect {}, uniform_depth {}, uniform_line_thickness {}, uniform_scissor {};

        int32_t scissor_instance_id {-1}, animation_instance_id {};
        float current_color_pass[4] {};

        uint32_t vbo_vertices {}, vbo_uvs {}, vbo_array {};
        float depth_testing_preset {};

        bool factor_changed {}, simple_shape {};
        static float viewport[4];
        bool check_convex_shape();
    public:
        static ekg::gpu::program program;
        static float orthographicm4[16];

        /*
         * Init gpu allocator.
         */
        void init();

        /*
         * Delete all GPU buffers & GL stuf.
         */
        void quit();

        /*
         * Bind a new gpu data.
         */
        ekg::gpu::data &bind_current_data();

        /*
         * Clear current gpu data active.
         */
        void clear_current_data();

        /*
         * Find registered gpu data in allocator's batch.
         */
        ekg::gpu::data *get_data_by_id(int32_t id);

        /*
         * Get current gpu data.
         */
        uint32_t get_current_data_id();
        
        /*
         * Find registered scissor in allocator's batch.
         */
        ekg::gpu::scissor *get_scissor_by_id(int32_t id);

        /*
         * Get current scissor active.
         */
        uint32_t get_instance_scissor_id();

        /*
         * Bind texture for send in data.
         */
        void bind_texture(uint32_t&);

        /*
         * Alloc vertices positions for gpu data.
         */
        void vertex2f(float, float);

        /*
         * Alloc texture UV coordinates for gpu data.
         */
        void coord2f(float, float);

        /* 
         * Update animations.
         */
        void on_update();

        /* 
         * Invoke allocator data reader.
         */
        void invoke();

        /*
         * Dispatch current data.
         */
        void dispatch();

        /*
         * Revoke data to GPU.
         */
        void revoke();

        /*
         * Draw all data from the gpu allocator's batch.
         */
        void draw();

        /*
         * Sync active scissor position.
         */
        void sync_scissor_pos(float, float);

        /*
         * Bind scissor using one ID for send in batch.
         */
        void bind_scissor(int32_t);

        /*
         * Stop batching rectangle scissor.
         */
        void bind_off_scissor();

        /*
         * Bind animation index for process in CPU time.
         */
        void bind_animation(int32_t);

        /*
         * Stop indexing animation.
         */
        void bind_off_animation();
    };
}

#endif