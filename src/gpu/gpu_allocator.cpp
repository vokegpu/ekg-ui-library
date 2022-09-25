#include "ekg/gpu/gpu_allocator.hpp"
#include "ekg/util/util.hpp"

ekg::gpu::program ekg::gpu::allocator::program {};
float ekg::gpu::allocator::orthographicm4[16] {};

void ekg::gpu::allocator::invoke() {
    this->iterate_ticked_count = 0;
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
        this->bind_texture(texture);
        texture_slot = (GLuint) this->loaded_texture_list.size();
    }

    ekg::gpu::data &data = this->bind_current_data();

    data.texture = texture;
    data.texture_slot = texture_slot;
}

void ekg::gpu::allocator::dispatch() {
    this->iterate_ticked_count++;
}

void ekg::gpu::allocator::revoke() {
    glBindVertexArray(this->buffer_list);
    glBindBuffer(GL_ARRAY_BUFFER, this->buffer_vertex);

    glEnableVertexAttribArray(0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->loaded_vertex_list.size(), &this->loaded_vertex_list[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

    glEnableVertexAttribArray(1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->loaded_uv_list.size(), &this->loaded_uv_list[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glBindVertexArray(0);
}

void ekg::gpu::allocator::draw() {
    ekg::gpu::invoke(ekg::gpu::allocator::program);
    ekg::gpu::allocator::program.setm4("MatrixProjection", ekg::gpu::allocator::orthographicm4);

    glBindVertexArray(this->buffer_list);
    bool scissor_enabled {};

    for (uint32_t data_iterations = 0; data_iterations < this->iterate_ticked_count; data_iterations++) {
        auto &data = this->cpu_allocated_data[data_iterations];

        if (data.texture != 0) {
            glActiveTexture(GL_TEXTURE0 + data.texture_slot);
            glBindTexture(GL_TEXTURE_2D, data.texture);

            ekg::gpu::allocator::program.set("ActiveTextureSlot", data.texture_slot);
        }

        ekg::gpu::allocator::program.set4("Color", data.colored_area);
        ekg::gpu::allocator::program.set4("Rect", data.rect_area);
        ekg::gpu::allocator::program.set("EnableOutline", data.outline[0]);

        if (data.outline[0]) {
            ekg::gpu::allocator::program.set("LineThickness", data.outline[1]);
        }

        switch (data.scissored_area[0]) {
            case -1: {
                glDrawArrays(GL_TRIANGLES, data.begin_stride, data.end_stride);

                if (scissor_enabled) {
                    glDisable(GL_SCISSOR_TEST);
                    scissor_enabled = false;
                }

                break;
            }

            default: {
                if (!scissor_enabled) {
                    glEnable(GL_SCISSOR_TEST);
                    scissor_enabled = true;
                }

                glScissor(data.scissored_area[0], ekg::display::height - (data.scissored_area[1] + data.scissored_area[3]), data.scissored_area[2], data.scissored_area[3]);
                glDrawArrays(GL_TRIANGLES, data.begin_stride, data.end_stride);

                break;
            }
        }
    }

    if (scissor_enabled) {
        glDisable(GL_SCISSOR_TEST);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void ekg::gpu::allocator::init() {
    glGenVertexArrays(1, &this->buffer_list);
    glGenBuffers(1, &this->buffer_vertex);
    glGenBuffers(1, &this->buffer_uv);

    const char* vsh_shader {};
    const char* fsh_shader {};

    gpu::create_basic_program(ekg::gpu::allocator::program, vsh_shader, fsh_shader);
}

ekg::gpu::data &ekg::gpu::allocator::bind_current_data() {
    return this->cpu_allocated_data[this->iterate_ticked_count];
}
