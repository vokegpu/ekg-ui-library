/**
 * MIT License
 * 
 * Copyright (c) 2022-2024 Rina Wilk / vokegpu@gmail.com
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

#include "ekg/gpu/api.hpp"
#include "ekg/util/geometry.hpp"

namespace ekg::gpu {
  class allocator {
  public:
    static bool is_out_of_scissor;
    static float concave;
    static uint64_t current_rendering_data_count;
  protected:
    std::vector<ekg::gpu::data_t> data_list {};
    std::vector<float> cached_geometry_resources {};

    uint64_t data_instance_index {};
    uint64_t previous_cached_geometry_resources_size {};
    uint64_t cached_geometry_index {};

    int32_t simple_shape_index {-1};
    int32_t previous_factor {};

    int32_t begin_stride_count {};
    int32_t end_stride_count {};

    bool factor_changed {};
    bool simple_shape {};
    bool out_of_scissor_rect {};
    ekg::rect scissor_instance {};
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
    ekg::gpu::data_t &bind_current_data();

    /*
     * Clear current gpu data active.
     */
    void clear_current_data();

    /*
     * Find registered gpu data in allocator's batch.
     */
    ekg::gpu::data_t *get_data_by_id(int32_t id);

    /*
     * Get current gpu data.
     */
    uint32_t get_current_data_id();

    /*
     * Bind texture for send in data.
     */
    void bind_texture(ekg::gpu::sampler_t *p_sampler);

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
    void sync_scissor(ekg::rect &scissor, ekg::rect &rect_child, ekg::rect *p_parent_scissor);
  };
}

#endif