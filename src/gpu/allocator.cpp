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
    this->allocated_size = 0;
    this->begin_stride_count = 0;
    this->end_stride_count = 0;

    // Set stride to 0 vertex position.
    this->clear_current_data();
    this->bind_current_data().begin_stride = this->begin_stride_count;
}

void ekg::gpu::allocator::bind_texture(GLuint &texture) {
    bool should_alloc_new_texture {};
    GLuint texture_slot {};

    for (std::size_t it = 0; it < this->loaded_texture_list.size(); it++) {
        GLuint &textures = this->loaded_texture_list.at(it);
        should_alloc_new_texture = textures != texture;

        if (!should_alloc_new_texture) {
            texture_slot = (uint32_t) it;
            break;
        }
    }

    if (should_alloc_new_texture) {
        this->loaded_texture_list.push_back(texture);
        texture_slot = (GLuint) this->loaded_texture_list.size() - 1;
    }

    ekg::gpu::data &data = this->bind_current_data();

    data.texture = texture;
    data.texture_slot = texture_slot;
}

void ekg::gpu::allocator::dispatch() {
    auto &data = this->bind_current_data();

    data.scissor_id = this->scissor_instance_id;
    data.begin_stride = this->begin_stride_count;
    data.end_stride = this->end_stride_count;

    if (!this->factor_changed) {
        this->factor_changed = this->previous_factor != data.factor;
    }

    this->begin_stride_count += this->end_stride_count;
    this->end_stride_count = 0;

    this->allocated_size++;
    this->clear_current_data();;
}

void ekg::gpu::allocator::revoke() {
    bool should_re_alloc_buffers = this->previous_allocated_size != this->allocated_size;

    if (should_re_alloc_buffers) {
        this->cpu_allocated_data.resize(this->allocated_size);
        this->cache_scissor.resize(this->allocated_size);
    }

    should_re_alloc_buffers = should_re_alloc_buffers || this->factor_changed;

    this->previous_allocated_size = this->allocated_size;
    this->factor_changed = false;

    if (should_re_alloc_buffers) {
        glBindVertexArray(this->buffer_list);
        glBindBuffer(GL_ARRAY_BUFFER, this->buffer_vertex);
        glEnableVertexAttribArray(0);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->loaded_vertex_list.size(), &this->loaded_vertex_list[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

        glBindBuffer(GL_ARRAY_BUFFER, this->buffer_uv);
        glEnableVertexAttribArray(1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->loaded_uv_list.size(), &this->loaded_uv_list[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
        glBindVertexArray(0);
    }

    this->loaded_texture_list.clear();
    this->loaded_vertex_list.clear();
    this->loaded_uv_list.clear();
}

void ekg::gpu::allocator::draw() {
    ekg::gpu::invoke(ekg::gpu::allocator::program);
    glBindVertexArray(this->buffer_list);

    bool scissor_enabled {};
    bool active_texture {};
    bool texture_enabled {};

    float depth_testing {this->depth_testing_preset};
    ekg::gpu::scissor* scissor {};

    for (ekg::gpu::data &data : this->cpu_allocated_data) {
        active_texture = data.texture != 0;

        if (active_texture) {
            glActiveTexture(GL_TEXTURE0 + data.texture_slot);
            glBindTexture(GL_TEXTURE_2D, data.texture);

            ekg::gpu::allocator::program.set("ActiveTextureSlot", data.texture_slot);
            texture_enabled = true;
        }

        if (texture_enabled && !active_texture) {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        ekg::gpu::allocator::program.set("ActiveTexture", active_texture);
        ekg::gpu::allocator::program.set4("Color", data.colored_area);
        ekg::gpu::allocator::program.set4("Rect", data.rect_area);
        ekg::gpu::allocator::program.set("Depth", depth_testing);
        ekg::gpu::allocator::program.set("LineThickness", data.outline);

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

        depth_testing += 1;
    }

    if (scissor_enabled || texture_enabled) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_SCISSOR_TEST);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void ekg::gpu::allocator::init() {
    glGenVertexArrays(1, &this->buffer_list);
    glGenBuffers(1, &this->buffer_vertex);
    glGenBuffers(1, &this->buffer_uv);
}

void ekg::gpu::allocator::clear_current_data() {
    if (this->allocated_size >= this->cpu_allocated_data.size()) {
        this->cpu_allocated_data.emplace_back();
        this->cache_scissor.emplace_back();
    }

    this->cache_scissor[this->allocated_size].rect[0] = -1; // it means that this scissor cache element no longer be used.
    ekg::gpu::data &data = this->bind_current_data();

    data.outline = 0;
    data.texture = 0;
    data.texture_slot = 0;
    data.scissor_id = -1;
    previous_factor = data.factor;
}

ekg::gpu::data &ekg::gpu::allocator::bind_current_data() {
    return this->cpu_allocated_data[this->allocated_size];
}

int32_t ekg::gpu::allocator::get_current_data_id() {
    return this->allocated_size;
}

ekg::gpu::data* ekg::gpu::allocator::bind_data(int32_t id) {
    if (id < 0 || id > this->allocated_size) {
        return nullptr;
    }

    return &this->cpu_allocated_data[id];
}

void ekg::gpu::allocator::quit() {
    glDeleteTextures((GLint) this->loaded_texture_list.size(), &this->loaded_texture_list[0]);
    glDeleteBuffers(1, &this->buffer_list);
    glDeleteBuffers(1, &this->buffer_uv);
    glDeleteVertexArrays(1, &this->buffer_list);
}

ekg::gpu::scissor* ekg::gpu::allocator::bind_scissor(int32_t id) {
    if (id < 0 || id > this->allocated_size) {
        return nullptr;
    }

    return &this->cache_scissor[id];
}

int32_t ekg::gpu::allocator::get_instance_scissor_id() {
    return this->scissor_instance_id;
}

void ekg::gpu::allocator::invoke_scissor() {
    this->scissor_instance_id = this->allocated_size;
}

void ekg::gpu::allocator::scissor(int32_t x, int32_t y, int32_t w, int32_t h) {
    auto &scissor {this->cache_scissor[this->allocated_size]};
    scissor.rect[0] = x;
    scissor.rect[1] = ekg::display::height - (y + h);
    scissor.rect[2] = w;
    scissor.rect[3] = h;
}

void ekg::gpu::allocator::revoke_scissor() {
    this->scissor_instance_id = -1;
}

void ekg::gpu::allocator::vertex2f(float x, float y) {
    this->loaded_vertex_list.push_back(x);
    this->loaded_vertex_list.push_back(y);
    this->end_stride_count++;
}

void ekg::gpu::allocator::coord2f(float x, float y) {
    this->loaded_uv_list.push_back(x);
    this->loaded_uv_list.push_back(y);
}