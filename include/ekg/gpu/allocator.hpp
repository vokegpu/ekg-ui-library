/*
 * MIT License
 * 
 * Copyright (c) 2022-2023 Rina Wilk / vokegpu@gmail.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef EKG_GPU_ALLOCATOR_H
#define EKG_GPU_ALLOCATOR_H

#include <array>
#include <vector>

#include "ekg/gpu/data.hpp"
#include "ekg/gpu/gl.hpp"
#include "ekg/util/geometry.hpp"

namespace ekg::gpu {
    class allocator {
    protected:
        static float viewport[4];
    public:
        static ekg::gpu::program program;
        static float mat4x4orthographic[16];
        static bool is_out_of_scissor;
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
    };
}

#endif