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
float ekg::gpu::allocator::mat4x4orthographic[16] {};
float ekg::gpu::allocator::viewport[4] {};

void ekg::gpu::allocator::invoke() {
    /* reset all "flags", everything is used tick by tick to compare factories */

    this->data_instance_index = 0;
    this->begin_stride_count = 0;
    this->end_stride_count = 0;
    this->simple_shape_index = -1;

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
        glBindVertexArray(this->vbo_array);
        /* set shader binding location 0 and dispatch mesh of vertices collected by allocator */

        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
        glEnableVertexAttribArray(0);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->cached_vertices.size(), &this->cached_vertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

        /* set shader binding location 1 and dispatch mesh of texttre coordinates collected by allocator */
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_uvs);
        glEnableVertexAttribArray(1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->cached_uvs.size(), &this->cached_uvs[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
        glBindVertexArray(0);
    }

    this->cached_textures.clear();
    this->cached_vertices.clear();
    this->cached_uvs.clear();
}

void ekg::gpu::allocator::on_update() {
}

void ekg::gpu::allocator::draw() {
    ekg::gpu::invoke(ekg::gpu::allocator::program);
    glBindVertexArray(this->vbo_array);

    bool active_texture {}, texture_enabled {};
    ekg::gpu::scissor *scissor {};
    glBindTexture(GL_TEXTURE_2D, 0);

    GLboolean is_depth_test_previous_enable {};
    glGetBooleanv(GL_DEPTH_TEST, &is_depth_test_previous_enable);
    glDisable(GL_DEPTH_TEST);

    /*
     * The batching system of gpu allocator use instanced rendering concept, if there is some simple shape rect
     * (gpu data rect that contains x, y, w & h rectangle), allocator reuse this to every draw call.
     * For text rendering, allocator do draw calls per text, or be, rendering a long text only use one draw call!
     *
     * Why there is glUniforms direct calls? I think it can reduce some wrappers runtime calls, but only here.
     * What is the depth level? That is the layer level of current gpu data rendered, processing layer depth testing.
     *
     * VokeGpu coded powerfully gpu allocator for drawing UIs, the batching system has scissor & others
     * important draw features for UI context.
     */

    ekg::gpu::data data {};
    for (uint64_t it {}; it < this->data_instance_index; it++) {
        data = this->data_list[it];
        active_texture = data.material_texture != 0;
        if (active_texture) {
            glActiveTexture(GL_TEXTURE0 + static_cast<int32_t>(data.active_tex_slot));
            glBindTexture(GL_TEXTURE_2D, data.material_texture);
            glUniform1i(this->uniform_active_texture, true);
            texture_enabled = true;
        }

        if (texture_enabled && !active_texture) {
            glUniform1i(this->uniform_active_texture, false);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        glUniform4fv(this->uniform_color, GL_TRUE, data.material_color);
        glUniform4fv(this->uniform_rect, GL_TRUE, data.shape_rect);
        glUniform1i(this->uniform_line_thickness, data.line_thickness);

        /* allocator use 6 vertices to draw, no need element buffer object */

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
                glUniform4fv(this->uniform_scissor, GL_TRUE, scissor->rect);
                break;
            }
        }

        glDrawArrays(GL_TRIANGLES, data.begin_stride, data.end_stride);
    }

    if (texture_enabled) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (is_depth_test_previous_enable) {
        glEnable(GL_DEPTH_TEST);
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
    this->uniform_active_texture = glGetUniformLocation(shading_program_id, "uActiveTexture");
    this->uniform_color = glGetUniformLocation(shading_program_id, "uColor");
    this->uniform_rect = glGetUniformLocation(shading_program_id, "uRect");
    this->uniform_line_thickness = glGetUniformLocation(shading_program_id, "uLineThickness");
    this->uniform_scissor = glGetUniformLocation(shading_program_id, "uScissor");
    this->uniform_enable_scissor = glGetUniformLocation(shading_program_id, "uEnableScissor");

    ekg::log() << "GPU allocator initialised";
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

ekg::gpu::data *ekg::gpu::allocator::get_data_by_id(int32_t id) {
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

    scissor.rect[0] = rect_child.x;
    scissor.rect[1] = rect_child.y;
    scissor.rect[2] = rect_child.w;
    scissor.rect[3] = rect_child.h;

    /*
     * Scissor is a great feature from OpenGL, but it
     * does not stack, means that GL context does not
     * accept scissor inside scissor.

     * After 1 year studying scissor, I  built one scheme,
     * compute bounds of all parent widgets with the parent
     * master, obvious it takes some ticks but there is no
     * other way (maybe I am wrong).

     * Two things important:
     * 1 - This scissors scheme use scissor IDs from widgets.
     * 2 - Iteration collect ALL parent families and sub parent of target.
     */
    if (mother_parent_id == 0) {
        return;
    }

    auto &mother_rect {this->scissor_map[mother_parent_id]};

    if (scissor.rect[0] < mother_rect.rect[0]) {
        scissor.rect[2] -= mother_rect.rect[0] - scissor.rect[0];
        scissor.rect[0] = mother_rect.rect[0];
    }

    if (scissor.rect[1] < mother_rect.rect[1]) {
        scissor.rect[3] -= mother_rect.rect[1] - scissor.rect[1];
        scissor.rect[1] = mother_rect.rect[1];
    }

    if (scissor.rect[0] + scissor.rect[2] > mother_rect.rect[0] + mother_rect.rect[2]) {
        scissor.rect[2] -= (scissor.rect[0] + scissor.rect[2]) - (mother_rect.rect[0] + mother_rect.rect[2]);
    }

    if (scissor.rect[1] + scissor.rect[3] > mother_rect.rect[1] + mother_rect.rect[3]) {
        scissor.rect[3] -= (scissor.rect[1] + scissor.rect[3]) - (mother_rect.rect[1] + mother_rect.rect[3]);
    }
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

    this->cached_vertices.emplace_back(x);
    this->cached_vertices.emplace_back(y);
    this->end_stride_count++;
}

void ekg::gpu::allocator::coord2f(float x, float y) {
    if (this->check_convex_shape()) {
        return;
    }

    this->cached_uvs.emplace_back(x);
    this->cached_uvs.emplace_back(y);
}

bool ekg::gpu::allocator::check_convex_shape() {
    auto &data {this->bind_current_data()};
    this->simple_shape = static_cast<int32_t>(data.shape_rect[2]) != ekg::concave && static_cast<int32_t>(data.shape_rect[3]) != ekg::concave;
    return this->simple_shape_index != -1 && this->simple_shape;
}
