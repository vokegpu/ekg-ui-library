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

#include "ekg/gpu/allocator.hpp"
#include "ekg/ekg.hpp"
#include "ekg/gpu/gl.hpp"

ekg::gpu::program ekg::gpu::allocator::program {};
float ekg::gpu::allocator::orthographicm4[16] {};
float ekg::gpu::allocator::viewport[4] {};

void ekg::gpu::allocator::invoke() {
    /* reset all "flags", everything is used tick by tick to compare factories */

    this->data_instance_index = 0;
    this->begin_stride_count = 0;
    this->end_stride_count = 0;
    this->simple_shape_index = -1;
    this->persistent_animation_ids_map.clear();

    /* unique shape data will break if not clear the first index. */

    this->clear_current_data();
    this->bind_current_data().begin_stride = this->begin_stride_count; // reset to 0
}

void ekg::gpu::allocator::bind_texture(uint32_t &texture) {
    bool should_alloc_new_texture {};
    uint8_t texture_slot {};

    /* repeating textures increase the active textures, for this reason allocator prevent "dupes" */

    for (std::size_t it = 0; it < this->cached_textures.size(); it++) {
        auto &textures = this->cached_textures.at(it);
        should_alloc_new_texture = textures != texture;

        if (!should_alloc_new_texture) {
            texture_slot = (uint8_t) it;
            break;
        }
    }

    if (should_alloc_new_texture) {
        this->cached_textures.push_back(texture);
        texture_slot = (uint8_t) this->cached_textures.size() - 1;
    }

    auto &data {this->bind_current_data()};
    data.material_texture = texture;
    data.active_tex_slot = texture_slot;
}

void ekg::gpu::allocator::dispatch() {
    auto &data {this->bind_current_data()};

    /* if this data contains a simple rect shape scheme, save this index and reuse later */

    this->simple_shape = static_cast<int32_t>(data.shape_rect[2]) != ekg::concave && static_cast<int32_t>(data.shape_rect[3]) != ekg::concave;
    if (this->simple_shape_index == -1 && this->simple_shape) {
        this->simple_shape_index = this->begin_stride_count;
        this->begin_stride_count += this->end_stride_count;
    }

    if (this->simple_shape) {
        data.begin_stride = this->simple_shape_index;
        data.end_stride = 6; // simple shape contains 6 vertices.
        this->end_stride_count = 0;
    } else {
        data.begin_stride = this->begin_stride_count;
        data.end_stride = this->end_stride_count;
    }

    data.scissor_id = this->scissor_instance_id;
    data.id = static_cast<int32_t>(this->data_instance_index);

    /* animate this data adding the reference into loaded widget list */
    // todo: animation
    if (this->active_animation != nullptr && false) {
        if (this->animation_index >= this->active_animation->size()) this->active_animation->emplace_back();
        auto &animation {this->active_animation->at(this->animation_index)};
        animation.data = &data;

        /* if this animation still exists, independent of states, we keep not remove from list */

        if (this->animation_index == 0) {
            this->persistent_animation_ids_map[this->animation_instance_id] = true;
            this->persistent_animation_ids.push_back(this->animation_instance_id);
        }

        if (animation.finished) {
            data.material_color[4] = data.material_color[3];
        } else if (animation.initial) {
            this->animation_update_list.push_back(&animation);
            data.material_color[4] = 0;
            animation.initial = false;
        }

        this->animation_index++;
    } else {
        /* else we do not need to process animation update in this data */
        data.material_color[4] = data.material_color[3];
    }

    /* flag re alloc buffers if factor changed */

    if (!this->factor_changed) {
        this->factor_changed = this->previous_factor != data.factor;
    }

    this->begin_stride_count += this->end_stride_count;
    this->end_stride_count = 0;

    this->data_instance_index++;
    this->clear_current_data();
}

void ekg::gpu::allocator::revoke() {
    bool should_re_alloc_buffers {this->previous_data_list_size != this->data_instance_index};

    if (should_re_alloc_buffers) {
        this->data_list.resize(this->data_instance_index);
    }

    should_re_alloc_buffers = should_re_alloc_buffers || this->factor_changed;

    this->previous_data_list_size = this->data_instance_index;
    this->factor_changed = false;

    if (should_re_alloc_buffers) {
        /* bind the vertex array object (list of vbos) */

        glBindVertexArray(this->vbo_array);

        /* set shader binding location 0 and dispatch mesh of vertices collected by allocator */

        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
        glEnableVertexAttribArray(0);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->cached_vertices.size(), &this->cached_vertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

        /* set shader binding location 1 and dispatch mesh of texture coordinates collected by allocator */

        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_uvs);
        glEnableVertexAttribArray(1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->cached_uvs.size(), &this->cached_uvs[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
        glBindVertexArray(0);
    }

    if (!this->persistent_animation_ids.empty()) {
        const std::vector<uint32_t> cached_ids {this->persistent_animation_ids};
        this->persistent_animation_ids.clear();

        for (const uint32_t &ids : cached_ids) {
            if (this->persistent_animation_ids_map[ids]) {
                this->persistent_animation_ids.push_back(ids);
                continue;
            }

            this->persistent_animation_ids_map.erase(ids);
            this->animation_map.erase(ids);
        }
    }

    this->cached_textures.clear();
    this->cached_vertices.clear();
    this->cached_uvs.clear();
}

void ekg::gpu::allocator::on_update() {
    if (!this->animation_update_list.empty()) {
        int32_t animation_progress_count {};
        ekg::log(std::to_string(this->animation_update_list.size()));

        /* interpolate the alpha using interpolation linear (lerp) */

        for (ekg::gpu::animation* &animation : this->animation_update_list) {
                if (animation == NULL || animation->data == NULL) {
                animation_progress_count++;
                continue;
            }

            if ((animation->finished = animation->data->material_color[4] == animation->data->material_color[3])) {
                animation_progress_count++;
                continue;
            }

            animation->data->material_color[4] = static_cast<uint8_t>(ekg::lerp(static_cast<float>(animation->data->material_color[4]), static_cast<float>(animation->data->material_color[3]), ekg::display::dt));
        }

        /* no problem with segment fault, because this is a reference pointer */

        if (animation_progress_count == this->animation_update_list.size()) {
            this->animation_update_list.clear();
        }
    }
}

void ekg::gpu::allocator::draw() {
    ekg::gpu::invoke(ekg::gpu::allocator::program);
    glBindVertexArray(this->vbo_array);

    bool active_texture {}, texture_enabled {};
    float depth_level {this->depth_testing_preset};
    ekg::gpu::scissor *scissor {};
    auto &shading_program_id {ekg::gpu::allocator::program.id};

    /*
      The batching system of gpu allocator use instanced rendering concept, if there is some simple shape rect
      (gpu data rect that contains x, y, w & h rectangle), allocator reuse this to every draw call.
      For text rendering, allocator do draw calls per text, or be, rendering a long text only use one draw call!

      Why there is glUniforms direct calls? I think it can reduce some wrappers runtime calls, but only here.
      What is the depth level? That is the layer level of current gpu data rendered, processing layer depth testing.

      VokeGpu coded powerfully gpu allocator for drawing UIs, the batching system have animations, scissor & others
      important draw features for UI context.
     */

    for (ekg::gpu::data &data : this->data_list) {
        active_texture = data.material_texture != 0;

        if (active_texture) {
            glActiveTexture(GL_TEXTURE0 + static_cast<int32_t>(data.active_tex_slot));
            glBindTexture(GL_TEXTURE_2D, data.material_texture);

            glUniform1i(this->uniform_active_texture_slot, static_cast<int32_t>(data.active_tex_slot));
            glUniform1i(this->uniform_active_texture, true);
            texture_enabled = true;
        }

        if (texture_enabled && !active_texture) {
            glUniform1i(this->uniform_active_texture, false);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        this->current_color_pass[0] = static_cast<float>(data.material_color[0]) / 255;
        this->current_color_pass[1] = static_cast<float>(data.material_color[1]) / 255;
        this->current_color_pass[2] = static_cast<float>(data.material_color[2]) / 255;
        this->current_color_pass[3] = static_cast<float>(data.material_color[4]) / 255;

        glUniform4f(this->uniform_color, this->current_color_pass[0], this->current_color_pass[1], this->current_color_pass[2], this->current_color_pass[3]);
        glUniform4f(this->uniform_rect, data.shape_rect[0], data.shape_rect[1], data.shape_rect[2], data.shape_rect[3]);
        glUniform1i(this->uniform_line_thickness, data.line_thickness);
        glUniform1f(this->uniform_depth, depth_level);

        /* allocator use 6 vertices to draw, no need for element buffer object */

        switch (data.scissor_id) {
            case -1: {
                glUniform1i(this->uniform_enable_scissor, false);
                break;
            }

            default: {
                scissor = this->get_scissor_by_id(data.scissor_id);
                if (scissor == nullptr) {
                    break;
                }

                glUniform1i(this->uniform_enable_scissor, true);
                glUniform4f(this->uniform_scissor, scissor->rect[0], scissor->rect[1], scissor->rect[2], scissor->rect[3]);
                break;
            }
        }

        glDrawArrays(GL_TRIANGLES, data.begin_stride, data.end_stride);

        /* plus: depth testing is needed for layout level */

        depth_level += 0.000001f;
    }

    if (texture_enabled) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void ekg::gpu::allocator::init() {
    glGenVertexArrays(1, &this->vbo_array);
    glGenBuffers(1, &this->vbo_vertices);
    glGenBuffers(1, &this->vbo_uvs);

    /* reduce glGetLocation calls when rendering the batch */
    auto &shading_program_id {ekg::gpu::allocator::program.id};
    this->uniform_active_texture = glGetUniformLocation(shading_program_id, "ActiveTexture");
    this->uniform_active_texture_slot = glGetUniformLocation(shading_program_id, "ActiveTextureSlot");
    this->uniform_color = glGetUniformLocation(shading_program_id, "Color");
    this->uniform_rect = glGetUniformLocation(shading_program_id, "Rect");
    this->uniform_line_thickness = glGetUniformLocation(shading_program_id, "LineThickness");
    this->uniform_depth = glGetUniformLocation(shading_program_id, "Depth");
    this->uniform_scissor = glGetUniformLocation(shading_program_id, "Scissor");
    this->uniform_enable_scissor = glGetUniformLocation(shading_program_id, "EnableScissor");

    ekg::log("gpu allocator shading program loaded uniforms successfully!");
}

void ekg::gpu::allocator::clear_current_data() {
    /* allocator handle automatically the size of data */

    if (this->data_instance_index >= this->data_list.size()) {
        this->data_list.emplace_back();
    }

    ekg::gpu::data &data {this->bind_current_data()};

    data.line_thickness = 0;
    data.material_texture = 0;
    data.active_tex_slot = 0;
    data.scissor_id = -1;

    this->previous_factor = data.factor;
}

ekg::gpu::data &ekg::gpu::allocator::bind_current_data() {
    return this->data_list[this->data_instance_index];
}

uint32_t ekg::gpu::allocator::get_current_data_id() {
    return this->data_instance_index;
}

ekg::gpu::data* ekg::gpu::allocator::get_data_by_id(int32_t id) {
    if (id < 0 || id > this->data_instance_index) {
        return nullptr;
    }

    return &this->data_list[id];
}

void ekg::gpu::allocator::quit() {
    glDeleteTextures((int32_t) this->cached_textures.size(), &this->cached_textures[0]);
    glDeleteBuffers(1, &this->vbo_array);
    glDeleteBuffers(1, &this->vbo_uvs);
    glDeleteVertexArrays(1, &this->vbo_array);
}

ekg::gpu::scissor *ekg::gpu::allocator::get_scissor_by_id(int32_t id) {
    return &this->scissor_map[id];
}

uint32_t ekg::gpu::allocator::get_instance_scissor_id() {
    return this->scissor_instance_id;
}

void ekg::gpu::allocator::sync_scissor_pos(float x, float y) {
    auto &scissor {this->scissor_map[this->scissor_instance_id]};
    scissor.rect[0] = x;
    scissor.rect[1] = y;
}

void ekg::gpu::allocator::bind_scissor(int32_t scissor_id) {
    this->scissor_instance_id = scissor_id;
}

void ekg::gpu::allocator::bind_off_scissor() {
    this->scissor_instance_id = -1;
}

void ekg::gpu::allocator::vertex2f(float x, float y) {
    if (this->check_convex_shape()) {
        return;
    }

    this->cached_vertices.push_back(x);
    this->cached_vertices.push_back(y);
    this->end_stride_count++;
}

void ekg::gpu::allocator::coord2f(float x, float y) {
    if (this->check_convex_shape()) {
        return;
    }

    this->cached_uvs.push_back(x);
    this->cached_uvs.push_back(y);
}

void ekg::gpu::allocator::bind_animation(int32_t animation_id) {
    if (animation_id == 0) {
        this->active_animation = nullptr;
    } else {
        this->active_animation = &this->animation_map[animation_id];
        this->animation_instance_id = static_cast<int32_t>(animation_id);
    }
}

void ekg::gpu::allocator::bind_off_animation() {
    this->active_animation = nullptr;
    this->animation_index = 0;
}

bool ekg::gpu::allocator::check_convex_shape() {
    auto &data {this->bind_current_data()};
    this->simple_shape = static_cast<int32_t>(data.shape_rect[2]) != ekg::concave && static_cast<int32_t>(data.shape_rect[3]) != ekg::concave;
    return this->simple_shape_index != -1 && this->simple_shape;
}
