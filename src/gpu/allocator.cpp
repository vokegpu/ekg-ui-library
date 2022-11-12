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

#include "ekg/gpu/allocator.hpp"
#include "ekg/ekg.hpp"

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

void ekg::gpu::allocator::bind_texture(GLuint &texture) {
    bool should_alloc_new_texture {};
    GLuint texture_slot {};

    /* repeating textures increase the active textures, for this reason allocator prevent "dupes" */

    for (std::size_t it = 0; it < this->cached_textures.size(); it++) {
        auto &textures = this->cached_textures.at(it);
        should_alloc_new_texture = textures != texture;

        if (!should_alloc_new_texture) {
            texture_slot = (uint32_t) it;
            break;
        }
    }

    if (should_alloc_new_texture) {
        this->cached_textures.push_back(texture);
        texture_slot = (GLuint) this->cached_textures.size() - 1;
    }

    ekg::gpu::data &data = this->bind_current_data();

    data.texture = texture;
    data.texture_slot = texture_slot;
}

void ekg::gpu::allocator::dispatch() {
    auto &data = this->bind_current_data();

    /* if this data contains a simple rect shape scheme, save this index and reuse later */

    this->simple_shape = static_cast<int32_t>(data.rect_area[2]) != 0 && static_cast<int32_t>(data.rect_area[3]) != 0;
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

    if (this->active_animation != nullptr) {
        if (this->animation_index >= this->active_animation->size()) this->active_animation->emplace_back();
        auto &animation = this->active_animation->at(this->animation_index);
        animation.data = &data;

        /* if this animation still exists, independent of states, we keep not remove from list */

        if (this->animation_index == 0) {
            this->persistent_animation_ids_map[this->animation_instance_id] = true;
            this->persistent_animation_ids.push_back(this->animation_instance_id);
        }

        if (animation.finished) {
            data.colored_area[4] = data.colored_area[3];
        } else if (animation.initial) {
            this->animation_update_list.push_back(&animation);
            data.colored_area[4] = 0;
            animation.initial = false;
        }

        this->animation_index++;
    } else {
        /* else we do not need to process animation update in this data */
        data.colored_area[4] = data.colored_area[3];
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
    bool should_re_alloc_buffers = this->previous_data_list_size != this->data_instance_index;

    if (should_re_alloc_buffers) {
        this->data_list.resize(this->data_instance_index);
        this->scissor_list.resize(this->data_instance_index);
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * this->cached_vertices.size(), &this->cached_vertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

        /* set shader binding location 1 and dispatch mesh of texture coordinates collected by allocator */

        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_uvs);
        glEnableVertexAttribArray(1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * this->cached_uvs.size(), &this->cached_uvs[0], GL_STATIC_DRAW);
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

        /* interpolate the alpha using interpolation linear (lerp) */

        for (ekg::gpu::animation* &animation : this->animation_update_list) {
            if (animation == nullptr || animation->data == nullptr) {
                animation_progress_count++;
                continue;
            }

            if (animation->finished || animation->data->colored_area[4] == animation->data->colored_area[3]) {
                animation_progress_count++;
                animation->finished = true;
                continue;
            }

            animation->data->colored_area[4] = static_cast<int32_t>(ekg::lerp(static_cast<float>(animation->data->colored_area[4]), static_cast<float>(animation->data->colored_area[3]), ekg::display::dt));
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

    bool scissor_enabled {};
    bool active_texture {};
    bool texture_enabled {};

    float depth_testing {this->depth_testing_preset};
    ekg::gpu::scissor* scissor {};

    for (ekg::gpu::data &data : this->data_list) {
        active_texture = data.texture != 0;

        if (active_texture) {
            glActiveTexture(GL_TEXTURE0 + data.texture_slot);
            glBindTexture(GL_TEXTURE_2D, data.texture);

            ekg::gpu::allocator::program.set("ActiveTextureSlot", data.texture_slot);
            ekg::gpu::allocator::program.set("ActiveTexture", true);
            texture_enabled = true;
        }

        if (texture_enabled && !active_texture) {
            ekg::gpu::allocator::program.set("ActiveTexture", false);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        this->current_color_pass[0] = static_cast<float>(data.colored_area[0]) / 255;
        this->current_color_pass[1] = static_cast<float>(data.colored_area[1]) / 255;
        this->current_color_pass[2] = static_cast<float>(data.colored_area[2]) / 255;
        this->current_color_pass[3] = static_cast<float>(data.colored_area[4]) / 255;

        ekg::gpu::allocator::program.set4("Color", this->current_color_pass);
        ekg::gpu::allocator::program.set4("Rect", data.rect_area);
        ekg::gpu::allocator::program.set("Depth", depth_testing);
        ekg::gpu::allocator::program.set("LineThickness", data.mode);

        /* allocator use 6 vertices to draw, no need for element buffer object */

        switch (data.scissor_id) {
            case -1: {
                if (scissor_enabled) {
                    glDisable(GL_SCISSOR_TEST);
                    scissor_enabled = false;
                }

                glDrawArrays(GL_TRIANGLES, data.begin_stride, data.end_stride);
                break;
            }

            default: {
                /* there is no reason for invoke gl calls every cpu tick */

                if (!scissor_enabled) {
                    scissor_enabled = true;
                    scissor = this->bind_scissor(data.scissor_id);

                    glEnable(GL_SCISSOR_TEST);
                    glScissor(scissor->rect[0], scissor->rect[1], scissor->rect[2], scissor->rect[3]);
                }

                glDrawArrays(GL_TRIANGLES, data.begin_stride, data.end_stride);
                break;
            }
        }

        /* plus: depth testing is needed for layout level */

        depth_testing += 0.001f;
    }

    if (scissor_enabled || texture_enabled) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_SCISSOR_TEST);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void ekg::gpu::allocator::init() {
    glGenVertexArrays(1, &this->vbo_array);
    glGenBuffers(1, &this->vbo_vertices);
    glGenBuffers(1, &this->vbo_uvs);
}

void ekg::gpu::allocator::clear_current_data() {
    /* allocator handle automatically the size of data */

    if (this->data_instance_index >= this->data_list.size()) {
        this->data_list.emplace_back();
        this->scissor_list.emplace_back();
    }

    this->scissor_list[this->data_instance_index].rect[0] = -1; // it means that this scissors cache element is no longer used.
    ekg::gpu::data &data = this->bind_current_data();

    data.mode = 0;
    data.texture = 0;
    data.texture_slot = 0;
    data.scissor_id = -1;

    this->previous_factor = data.factor;
}

ekg::gpu::data &ekg::gpu::allocator::bind_current_data() {
    return this->data_list[this->data_instance_index];
}

uint32_t ekg::gpu::allocator::get_current_data_id() {
    return this->data_instance_index;
}

ekg::gpu::data* ekg::gpu::allocator::bind_data(int32_t id) {
    if (id < 0 || id > this->data_instance_index) {
        return nullptr;
    }

    return &this->data_list[id];
}

void ekg::gpu::allocator::quit() {
    glDeleteTextures((GLint) this->cached_textures.size(), &this->cached_textures[0]);
    glDeleteBuffers(1, &this->vbo_array);
    glDeleteBuffers(1, &this->vbo_uvs);
    glDeleteVertexArrays(1, &this->vbo_array);
}

ekg::gpu::scissor* ekg::gpu::allocator::bind_scissor(int32_t id) {
    if (id < 0 || id > this->data_instance_index) {
        return nullptr;
    }

    return &this->scissor_list[id];
}

uint32_t ekg::gpu::allocator::get_instance_scissor_id() {
    return this->scissor_instance_id;
}

void ekg::gpu::allocator::invoke_scissor() {
    this->scissor_instance_id = static_cast<int32_t>(this->data_instance_index);
}

void ekg::gpu::allocator::scissor(int32_t x, int32_t y, int32_t w, int32_t h) {
    auto &scissor {this->scissor_list[this->data_instance_index]};
    scissor.rect[0] = x;
    scissor.rect[1] = ekg::display::height - (y + h);
    scissor.rect[2] = w;
    scissor.rect[3] = h;
}

void ekg::gpu::allocator::revoke_scissor() {
    this->scissor_instance_id = -1;
}

void ekg::gpu::allocator::vertex2f(float x, float y) {
    if (this->check_simple_shape()) {
        return;
    }

    this->cached_vertices.push_back(x);
    this->cached_vertices.push_back(y);
    this->end_stride_count++;
}

void ekg::gpu::allocator::coord2f(float x, float y) {
    if (this->check_simple_shape()) {
        return;
    }

    this->cached_uvs.push_back(x);
    this->cached_uvs.push_back(y);
}

void ekg::gpu::allocator::bind_animation(uint32_t id_tag) {
    this->active_animation = &this->animation_map[id_tag];
    this->animation_instance_id = static_cast<int32_t>(id_tag);
    this->animation_index = 0;
}

void ekg::gpu::allocator::bind_off_animation() {
    this->active_animation = nullptr;
}

bool ekg::gpu::allocator::check_simple_shape() {
    auto &data = this->bind_current_data();
    this->simple_shape = static_cast<int32_t>(data.rect_area[2]) != 0 && static_cast<int32_t>(data.rect_area[3]) != 0;
    return this->simple_shape_index != -1 && this->simple_shape;
}
