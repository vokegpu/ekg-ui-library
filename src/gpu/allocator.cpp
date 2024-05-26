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

#include "ekg/gpu/allocator.hpp"
#include "ekg/ekg.hpp"

bool ekg::gpu::allocator::is_out_of_scissor {};
float ekg::gpu::allocator::concave {-2.0f};
uint64_t ekg::gpu::allocator::current_rendering_data_count {};

void ekg::gpu::allocator::invoke() {
  this->data_instance_index = 0;
  this->begin_stride_count = 0;
  this->end_stride_count = 0;
  this->simple_shape_index = 0;
  this->cached_geometry_index = 0;

  /**
   * inserting a simple triangle mesh,
   * is necessary to make work the simple-shape
   * rendering.
   **/
  this->push_back_geometry(0.0f, 0.0f, 0.0f, 0.0f);
  this->push_back_geometry(0.0f, 1.0f, 0.0f, 1.0f);
  this->push_back_geometry(1.0f, 0.0f, 1.0f, 0.0f);
  this->push_back_geometry(1.0f, 1.0f, 1.0f, 1.0f);

  this->clear_current_data();
  this->data_list.at(this->data_instance_index).begin_stride = this->end_stride_count;
  this->begin_stride_count += this->end_stride_count;
  this->end_stride_count = 0;
}

void ekg::gpu::allocator::bind_texture(ekg::gpu::sampler_t *p_sampler) {
  if (p_sampler == nullptr) {
    return;
  }

  ekg::gpu::data_t &data {this->data_list.at(this->data_instance_index)};
  data.sampler_index = ekg::core->p_gpu_api->bind_sampler(p_sampler);
}

void ekg::gpu::allocator::dispatch() {
  ekg::gpu::data_t &data {this->data_list.at(this->data_instance_index)};

  /**
   * Scissor must be synchned externally to update the scissor context  
   **/

  data.buffer_content[8] = this->scissor_instance.x;
  data.buffer_content[9] = this->scissor_instance.y;
  data.buffer_content[10] = this->scissor_instance.w;
  data.buffer_content[11] = this->scissor_instance.h;

  /**
   * the point of re-using a simple shape stride makes performance a little better,
   * due the index rendering, with only one triangle for rectangles.
   **/

  this->simple_shape = (
    static_cast<int32_t>(data.buffer_content[2]) != static_cast<int32_t>(ekg::gpu::allocator::concave) &&
    static_cast<int32_t>(data.buffer_content[3]) != static_cast<int32_t>(ekg::gpu::allocator::concave)
  );

  if (this->simple_shape) {
    data.begin_stride = this->simple_shape_index;
    data.end_stride = 4; // simple shape contains 4 vertices.
    this->end_stride_count = 0;
  } else {
    data.begin_stride = this->begin_stride_count;
    data.end_stride = this->end_stride_count;
  }

  /* flag re alloc buffers if factor changed */

  if (!this->factor_changed) {
    this->factor_changed = (
      this->previous_factor != data.factor
    );
  }

  this->begin_stride_count += this->end_stride_count;
  this->end_stride_count = 0;

  this->data_instance_index++;
  this->clear_current_data();
}

void ekg::gpu::allocator::revoke() {
  uint64_t cached_geometry_resources_size {this->cached_geometry_index};
  bool should_re_alloc_buffers {this->previous_cached_geometry_resources_size != cached_geometry_resources_size};

  if (this->data_instance_index < this->data_list.size()) {
    this->data_list.erase(this->data_list.begin() + this->data_instance_index + 1, this->data_list.end());
  }

  if (should_re_alloc_buffers || this->factor_changed) {
    ekg::core->p_gpu_api->re_alloc_geometry_resources(
      this->cached_geometry_resources.data(),
      this->cached_geometry_resources.size()
    );
  }

  this->factor_changed = false;

  if (this->cached_geometry_resources.size() < this->previous_cached_geometry_resources_size) {
    this->cached_geometry_resources.erase(
      this->cached_geometry_resources.begin() + cached_geometry_resources_size + 1,
      this->cached_geometry_resources.end()
    );
  }

  this->previous_cached_geometry_resources_size = cached_geometry_resources_size;
  ekg::gpu::allocator::current_rendering_data_count = this->data_list.size();
}

void ekg::gpu::allocator::on_update() {
}

void ekg::gpu::allocator::draw() {
  ekg::core->p_gpu_api->draw(
    this->data_list.data(),
    this->data_list.size()
  );
}

void ekg::gpu::allocator::init() {
  ekg::log() << "Initializing GPU allocator";
}

void ekg::gpu::allocator::clear_current_data() {
  if (this->data_instance_index >= this->data_list.size()) {
    this->data_list.emplace_back();
  }

  /* allocator handle automatically the size of data */
  ekg::gpu::data_t &data {this->data_list.at(this->data_instance_index)};
  data.line_thickness = 0;
  data.sampler_index = -1;

  this->previous_factor = data.factor;
}

ekg::gpu::data_t &ekg::gpu::allocator::bind_current_data() {
  return this->data_list.at(this->data_instance_index);
}

uint32_t ekg::gpu::allocator::get_current_data_id() {
  return this->data_instance_index;
}

ekg::gpu::data_t *ekg::gpu::allocator::get_data_by_id(int32_t id) {
  if (id < 0 || id > this->data_instance_index) {
    return nullptr;
  }

  return &this->data_list[id];
}

void ekg::gpu::allocator::quit() {
  ekg::log() << "Quitting GPU allocator";
}

void ekg::gpu::allocator::sync_scissor(
  ekg::rect &scissor,
  ekg::rect &rect_child,
  ekg::rect *p_parent_scissor
) {
  scissor.x = rect_child.x;
  scissor.y = rect_child.y;
  scissor.w = rect_child.w;
  scissor.h = rect_child.h;

  ekg::gpu::allocator::is_out_of_scissor = false;

  if (p_parent_scissor) {
    if (scissor.x < p_parent_scissor->x) {
      scissor.w -= p_parent_scissor->x - scissor.x;
      scissor.x = p_parent_scissor->x;
    }

    if (scissor.y < p_parent_scissor->y) {
      scissor.h -= p_parent_scissor->y - scissor.y;
      scissor.y = p_parent_scissor->y;
    }

    if (scissor.x + scissor.w > p_parent_scissor->x + p_parent_scissor->w) {
      scissor.w -= (scissor.x + scissor.w) - (p_parent_scissor->x + p_parent_scissor->w);
    }

    if (scissor.y + scissor.h > p_parent_scissor->y + p_parent_scissor->h) {
      scissor.h -= (scissor.y + scissor.h) - (p_parent_scissor->y + p_parent_scissor->h);
    }

    ekg::gpu::allocator::is_out_of_scissor = (
      !(scissor.x             < p_parent_scissor->x + p_parent_scissor->w &&
        scissor.x + scissor.w > p_parent_scissor->x                       &&
        scissor.y             < p_parent_scissor->y + p_parent_scissor->h &&
        scissor.y + scissor.h > p_parent_scissor->y)
    );
  }
  
  /**
   * It is much better to waste memory than CPU runtime processing.
   **/
  this->scissor_instance.x = scissor.x;
  this->scissor_instance.y = scissor.y;
  this->scissor_instance.w = scissor.w;
  this->scissor_instance.h = scissor.h;
}

void ekg::gpu::allocator::push_back_geometry(
  float x,
  float y,
  float u,
  float v
) {
  this->end_stride_count++;

  if (this->cached_geometry_index >=
      this->cached_geometry_resources.size()) {

    this->cached_geometry_index += 4;

    this->cached_geometry_resources.emplace_back(x);
    this->cached_geometry_resources.emplace_back(y);

    this->cached_geometry_resources.emplace_back(u);
    this->cached_geometry_resources.emplace_back(v);

    return;
  }

  this->cached_geometry_resources[this->cached_geometry_index++] = x;
  this->cached_geometry_resources[this->cached_geometry_index++] = y;

  this->cached_geometry_resources[this->cached_geometry_index++] = u;
  this->cached_geometry_resources[this->cached_geometry_index++] = v;
}