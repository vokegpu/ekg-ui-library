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
#include "ekg/util/geometry.hpp"
#include <array>
#include <map>
#include <vector>

namespace ekg::gpu {
    class allocator {
    protected:
        static float viewport[4];
    protected:
        std::vector<ekg::gpu::data> data_list {};
        std::unordered_map<int32_t, ekg::gpu::scissor> scissor_map {};

        std::vector<float> cached_geometry_resources {};
        std::vector<uint32_t> cached_textures {};

        uint64_t data_instance_index {};
        uint64_t previous_cached_geometry_resources_size {};

        int32_t simple_shape_index {-1};
        int32_t previous_factor {};
        int32_t scissor_instance_id {-1};

        uint64_t uvs_size {};
        uint64_t vertexes_size {};

        int32_t begin_stride_count {};
        int32_t end_stride_count {};

        int32_t uniform_active_texture {};
        int32_t uniform_active_tex_slot {};
        int32_t uniform_enable_scissor {};
        int32_t uniform_color {};
        int32_t uniform_rect {};
        int32_t uniform_line_thickness {};
        int32_t uniform_scissor {};

        uint32_t geometry_buffer {};
        uint32_t vbo_array {};
        uint32_t ebo_simple_shape {};

        bool factor_changed {};
        bool simple_shape {};
        bool out_of_scissor_rect {};
    public:
        static ekg::gpu::program program;
        static float mat4x4orthographic[16];

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
        ekg::gpu::data *get_data_by_id(int32_t);

        /*
         * Get current gpu data.
         */
        uint32_t get_current_data_id();
        
        /*
         * Find registered scissor in allocator's batch.
         */
        ekg::gpu::scissor *get_scissor_by_id(int32_t);

        /*
         * Remove scissor data from memory.
         */
        void erase_scissor_by_id(int32_t);

        /*
         * Get current scissor active.
         */
        uint32_t get_instance_scissor_id();

        /*
         * Bind texture for send in data.
         */
        void bind_texture(uint32_t);

        /*
         * Insert geometry positions here:
         * vertex x, y and texture coords u, and v.
         */
        void push_back_geometry(float, float, float, float);

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
        void sync_scissor(ekg::rect&, int32_t);

        /*
         * Bind scissor using one ID for send in batch.
         */
        void bind_scissor(int32_t);

        /*
         * Stop batching rectangle scissor.
         */
        void bind_off_scissor();

        /*
         * Check if the current shape rect is out of bound scissor.
         */
        bool is_out_of_scissor_rect();
    };
}

#endif