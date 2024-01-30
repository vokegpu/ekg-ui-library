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
#include "ekg/gpu/gl.hpp"
#include "ekg/os/ekg_opengl.hpp"

ekg::gpu::program ekg::gpu::allocator::program {};
float             ekg::gpu::allocator::mat4x4orthographic[16] {};
float             ekg::gpu::allocator::viewport[4] {};
bool              ekg::gpu::allocator::is_out_of_scissor {};

void ekg::gpu::allocator::invoke() {
  /*
   * Invocation segment reset the CPU-batching counters.
   */
  this->data_instance_index = 0;
  this->begin_stride_count = 0;
  this->end_stride_count = 0;
  this->simple_shape_index = 0;
  this->cached_geometry_index = 0;

  /*
   * The first 4 vertices are used by the simple shapes,
   * soon it is necessary to add not even one simple shape indices
   * but everything.
   */
  this->push_back_geometry(0.0f, 0.0f, 0.0f, 0.0f);
  this->push_back_geometry(0.0f, 1.0f, 0.0f, 1.0f);
  this->push_back_geometry(1.0f, 0.0f, 1.0f, 0.0f);
  this->push_back_geometry(1.0f, 1.0f, 1.0f, 1.0f);

  /* unique shape data will break if not clear the first index. */

  this->clear_current_data();
  this->loaded_data_list.at(this->data_instance_index).begin_stride = this->end_stride_count;
  this->begin_stride_count += this->end_stride_count;
  this->end_stride_count = 0;
}

void ekg::gpu::allocator::bind_texture(ekg::sampler_t *p_sampler) {
  ekg::gpu::data_t &data {this->loaded_data_list.at(this->data_instance_index)};
  data.sampler_index = ekg::core->p_gpu_api->bind_sampler(p_sampler);
}

void ekg::gpu::allocator::dispatch() {
  ekg::gpu::data_t &data {this->loaded_data_list.at(this->data_instance_index)};

  /* if this data contains a simple rect shape scheme, save this index and reuse later */

  this->simple_shape = (
    static_cast<int32_t>(data.buffer_content[2]) != ekg::concave &&
    static_cast<int32_t>(data.buffer_content[3]) != ekg::concave
  );

  if (this->simple_shape) {
    data.begin_stride = this->simple_shape_index;
    data.end_stride = 4; // simple shape contains 4 vertices.
    this->end_stride_count = 0;
  } else {
    data.begin_stride = this->begin_stride_count;
    data.end_stride = this->end_stride_count;
  }

  data.scissor_id = this->scissor_instance_id;

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

  if (this->data_instance_index < this->loaded_data_list.size()) {
    this->loaded_data_list.erase(this->loaded_data_list.begin() + this->data_instance_index + 1, this->loaded_data_list.end());
  }

  if (should_re_alloc_buffers || this->factor_changed) {
    ekg::core->p_gpu_api->re_alloc_rendering_geometry(
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
}

void ekg::gpu::allocator::on_update() {
}

void ekg::gpu::allocator::draw() {
  ekg::core->p_gpu_api->draw(
    this->loaded_data_list.data(),
    this->loaded_data_list.size()
    this->cached_texture_list.data(),
    this->cached_texture_list.size()
  );

  ekg::gpu::invoke(ekg::gpu::allocator::program);

  glBindVertexArray(this->vbo_array);
  glBindTexture(GL_TEXTURE_2D, 0);

  uint8_t prev_texture_bound {};
  bool texture_enabled {};

  /*
   * Before each rendering section, the allocator iterate alls textures and bind it on global context.
   */
  for (uint32_t it {}; it < this->cached_texture_list.size(); it++) {
    glActiveTexture(GL_TEXTURE0 + static_cast<int32_t>(it));
    glBindTexture(GL_TEXTURE_2D, this->cached_texture_list.at(it));
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /*
   * The allocator provides a high-performance dynamic mapped-shapes batching,
   * each simple rect (not a complex shape) is not batched, but complex shapes
   * like textured shapes - font rendering - is batched.
   */

  for (uint64_t it {}; it < this->data_instance_index; it++) {
    ekg::gpu::data_t &data {this->loaded_data_list.at(it)};
    texture_enabled = data.active_tex_slot > 0;

    if (texture_enabled && prev_texture_bound != data.active_tex_slot) {
      glUniform1i(this->uniform_active_tex_slot, data.active_tex_slot - 1);
      glUniform1i(this->uniform_active_texture, true);
      prev_texture_bound = data.active_tex_slot;
    } else if (!texture_enabled && prev_texture_bound > 0) {
      glUniform1i(this->uniform_active_texture, false);
      prev_texture_bound = 0;
    }

    glUniform4fv(this->uniform_color, GL_TRUE, data.material_color);
    glUniform4fv(this->uniform_rect, GL_TRUE, data.shape_rect);
    glUniform1i(this->uniform_line_thickness, data.line_thickness);
    glUniform4fv(this->uniform_scissor, GL_TRUE, this->scissor_map[data.scissor_id].rect);

    switch (data.begin_stride) {
      case 0: {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
        break;
      }

      default: {
        glDrawArrays(GL_TRIANGLES, data.begin_stride, data.end_stride);
        break;
      }
    }
  }

  glUniform1i(this->uniform_active_texture, false);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_BLEND);

  glBindVertexArray(0);
  glUseProgram(0);
}

void ekg::gpu::allocator::init() {
  ekg::core->p_gpu_api->init();
}

void ekg::gpu::allocator::clear_current_data() {
  /* allocator handle automatically the size of data */

  if (this->data_instance_index >= this->loaded_data_list.size()) {
    this->loaded_data_list.emplace_back();
  }

  ekg::gpu::data_t &data {this->loaded_data_list.at(this->data_instance_index)};
  data.line_thickness = 0;
  data.active_tex_slot = 0;
  data.scissor_id = -1;

  this->previous_factor = data.factor;
}

ekg::gpu::data_t &ekg::gpu::allocator::bind_current_data() {
  return this->loaded_data_list.at(this->data_instance_index);
}

uint32_t ekg::gpu::allocator::get_current_data_id() {
  return this->data_instance_index;
}

ekg::gpu::data_t *ekg::gpu::allocator::get_data_by_id(int32_t id) {
  if (id < 0 || id > this->data_instance_index) {
    return nullptr;
  }

  return &this->loaded_data_list[id];
}

void ekg::gpu::allocator::quit() {
  glDeleteTextures((int32_t) this->cached_texture_list.size(), &this->cached_texture_list[0]);
  glDeleteBuffers(1, &this->vbo_array);
  glDeleteVertexArrays(1, &this->vbo_array);

  // @ GPU API 3
  ekg::core->p_gpu_api->quit();
}

ekg::gpu::scissor *ekg::gpu::allocator::get_scissor_by_id(int32_t id) {
  return &this->scissor_map[id];
}

void ekg::gpu::allocator::erase_scissor_by_id(int32_t id) {
  if (this->scissor_map.count(id)) {
    this->scissor_map.erase(id);
  }
}

uint32_t ekg::gpu::allocator::get_instance_scissor_id() {
  return this->scissor_instance_id;
}

void ekg::gpu::allocator::sync_scissor(ekg::rect &rect_child, int32_t mother_parent_id) {
  auto &scissor {this->scissor_map[this->scissor_instance_id]};

  scissor.rect.x = rect_child.x;
  scissor.rect.y = rect_child.y;
  scissor.rect.w = rect_child.w;
  scissor.rect.h = rect_child.h;

  ekg::gpu::allocator::is_out_of_scissor = false;

  /**
   * The EKG rendering clipping/scissor part solution is actually very simple,
   * each draws section contains an ID, which map for a scissor data,
   * when batching is going on, the scissor is automatically fixed together.
   **/
  if (mother_parent_id) {
    auto &mother_rect {this->scissor_map[mother_parent_id]};

    if (scissor.rect.x < mother_rect.rect.x) {
      scissor.rect.w -= mother_rect.rect.x - scissor.rect.x;
      scissor.rect.x = mother_rect.rect.x;
    }

    if (scissor.rect.y < mother_rect.rect.y) {
      scissor.rect.h -= mother_rect.rect.y - scissor.rect.y;
      scissor.rect.y = mother_rect.rect.y;
    }

    if (scissor.rect.x + scissor.rect.w > mother_rect.rect.x + mother_rect.rect.w) {
      scissor.rect.w -= (scissor.rect.x + scissor.rect.w) - (mother_rect.rect.x + mother_rect.rect.w);
    }

    if (scissor.rect.y + scissor.rect.h > mother_rect.rect.y + mother_rect.rect.h) {
      scissor.rect.h -= (scissor.rect.y + scissor.rect.h) - (mother_rect.rect.y + mother_rect.rect.h);
    }

    ekg::gpu::allocator::is_out_of_scissor = (
      !(scissor.rect.x                  < mother_rect.rect.x + mother_rect.rect.w &&
        scissor.rect.x + scissor.rect.w > mother_rect.rect.x &&
        scissor.rect.y                  < mother_rect.rect.y + mother_rect.rect.h &&
        scissor.rect.y + scissor.rect.h > mother_rect.rect.y)
    );
  }


  ekg::gpu::data_t &data {this->loaded_data_list.at(this->data_instance_index)};
  
  /**
   * It is much better to waste memory than CPU runtime processing.
   **/
  data.buffer_content[8] = scissor.rect.x;
  data.buffer_content[9] = scissor.rect.y;
  data.buffer_content[10] = scissor.rect.w;
  data.buffer_content[11] = scissor.rect.h;
}

void ekg::gpu::allocator::bind_scissor(int32_t scissor_id) {
  this->scissor_instance_id = scissor_id;
}

void ekg::gpu::allocator::bind_off_scissor() {
  this->scissor_instance_id = -1;
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