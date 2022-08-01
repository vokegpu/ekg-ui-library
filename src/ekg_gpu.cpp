/**
 * EKG-LICENSE - this software/library LICENSE can not be modified in any instance.
 *
 * --
 * ANY NON-OFFICIAL MODIFICATION IS A CRIME.
 * DO NOT SELL THIS CODE SOFTWARE, FOR USE EKG IN A COMMERCIAL PRODUCT ADD EKG-LICENSE TO PROJECT,
 * RESPECT THE COPYRIGHT TERMS OF EKG, NO SELL WITHOUT EKG-LICENSE (IT IS A CRIME).
 * DO NOT FORK THE PROJECT SOURCE WITHOUT EKG-LICENSE.
 *
 * END OF EKG-LICENSE.
 **/
#include <ekg/ekg.hpp>
#include <cmath>
#include "ekg/api/ekg_gpu.hpp"


void ekg_gpu_data_handler::init() {
    switch (EKG_CPU_PLATFORM) {
        case ekgapi::cpu::X86: {
            const char* vertex_src = "#version 330 core\n"
                                     "\n"
                                     "layout (location = 0) in vec2 attrib_pos;\n"
                                     "layout (location = 1) in vec2 attrib_material;\n"
                                     "\n"
                                     "out vec2 varying_material;\n"
                                     "out vec4 varying_rect;\n"
                                     "uniform mat4 u_mat_matrix;\n"
                                     "uniform vec4 u_vec4_rect;\n"
                                     "uniform float u_float_zdepth;\n"
                                     "\n"
                                     "void main() {\n"
                                     "    vec2 pos = (attrib_pos * u_vec4_rect.zw) + u_vec4_rect.xy;"
                                     "    if (u_vec4_rect.z == 0.0f && u_vec4_rect.w == 0.0f) pos = attrib_pos + u_vec4_rect.xy;"
                                     "    gl_Position = u_mat_matrix * vec4(pos, (u_float_zdepth / 1000), 1.0f);\n"
                                     "    varying_material = attrib_material;\n"
                                     "    varying_rect = u_vec4_rect;\n"
                                     "}";

            const char* fragment_src = "#version 330 core\n"
                                       "\n"
                                       "in vec2 varying_material;\n"
                                       "in vec4 varying_rect;\n"
                                       "out vec4 out_frag_color;\n"
                                       "\n"
                                       "uniform sampler2D u_sampler2d_texture_active;\n"
                                       "uniform bool u_bool_set_texture;\n"
                                       "uniform vec4 u_vec4_color;\n"
                                       "uniform int u_int_shape_category;\n"
                                       "uniform float u_float_factor;\n"
                                       "uniform float u_float_viewport_height;\n"
                                       "\n"
                                       "void main() {\n"
                                       "    vec4 fragcolor = u_vec4_color;\n"
                                       "\n"
                                       "    if (u_bool_set_texture) {\n"
                                       "        fragcolor = texture(u_sampler2d_texture_active, varying_material.xy);\n"
                                       "        fragcolor = vec4(fragcolor.xyz - ((1.0f - u_vec4_color.xyz) - 1.0f), fragcolor.w - (1.0f - u_vec4_color.w));\n"
                                       "    }\n"
                                       "    if (u_int_shape_category == 1) {\n"
                                       "        vec2 center = vec2(varying_rect.x + (u_float_factor / 2), varying_rect.y + (u_float_factor / 2));\n"
                                       "        vec2 diff_center = center - vec2(varying_rect.x + ((u_float_factor / 2)), varying_rect.y);\n"
                                       "        vec2 diff = center - vec2(gl_FragCoord.x, u_float_viewport_height - gl_FragCoord.y);\n"
                                       "        float dist_to_frag = diff.x * diff.x + diff.y * diff.y;\n"
                                       "        if (dist_to_frag > (diff_center.x * diff_center.x + diff_center.y * diff_center.y)) {\n"
                                       "            fragcolor.a = 0;\n"
                                       "        }\n"
                                       "    } else if (u_int_shape_category == 2) {\n"
                                       "        float square_out = u_float_factor;"
                                       "        vec2 frag_pos = vec2(gl_FragCoord.x, u_float_viewport_height - gl_FragCoord.y);\n"
                                       "        vec4 rect = vec4(varying_rect.x + square_out, varying_rect.y + square_out, varying_rect.x + varying_rect.z - (square_out), varying_rect.y + varying_rect.w - (square_out));\n"
                                       "        if ((frag_pos.x > rect.x && frag_pos.x < rect.z && frag_pos.y > rect.y && frag_pos.y < rect.w)) {\n"
                                       "           fragcolor.a = 0;\n"
                                       "        }\n"
                                       "    }\n"
                                       "    out_frag_color = fragcolor;\n"
                                       "}";

            ekgapi::OpenGL::compile_program(this->default_program, vertex_src, fragment_src);
            break;
        }
    }

    // Gen the objects (VAO and VBO).
    glGenVertexArrays(1, &this->vertex_buffer_arr);
    glGenBuffers(1, &this->vertex_buf_object_vertex_positions);
    glGenBuffers(1, &this->vertex_buf_object_vertex_materials);
}

void ekg_gpu_data_handler::prepare() {
    // Bind VAO and draw the two VBO(s).
    glBindVertexArray(this->vertex_buffer_arr);
}

void ekg_gpu_data_handler::draw() {
    this->default_program.use();
    this->default_program.set_mat4x4("u_mat_matrix", this->mat4x4_ortho);
    this->default_program.set_float("u_float_viewport_height", this->viewport[3]);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ekg_gpu_data gpu_data;
    ekg_gpu_scissor gpu_scissor;

    // Simulate glMultiDrawArrays.
    for (int i = 0; i < this->amount_of_draw_iterations; i++) {
        gpu_data = this->allocated_gpu_data[i];

        this->default_program.set_int("u_bool_set_texture", gpu_data.texture != 0);
        this->default_program.set_vec4f("u_vec4_color", gpu_data.color);
        this->default_program.set_vec4f("u_vec4_rect", gpu_data.rect);
        this->default_program.set_float("u_float_zdepth", static_cast<float>(i + 1));
        this->default_program.set_int("u_int_shape_category", gpu_data.category);
        this->default_program.set_float("u_float_factor", gpu_data.factor);

        if (gpu_data.texture != 0) {
            glActiveTexture(GL_TEXTURE0 + gpu_data.texture_slot);
            glBindTexture(GL_TEXTURE_2D, gpu_data.texture);

            this->default_program.set_int("u_sampler2d_texture_active", gpu_data.texture_slot);
        }

        if (gpu_data.id_scissor != -1) {
            gpu_scissor = this->allocated_gpu_scissor[gpu_data.id_scissor];
            glEnable(GL_SCISSOR_TEST);
            glScissor(gpu_scissor.x, gpu_scissor.y, gpu_scissor.w, gpu_scissor.h);
        } else {
            glDisable(GL_SCISSOR_TEST);
        }

        glDrawArrays(GL_TRIANGLES, gpu_data.raw, gpu_data.data);
        glBindTexture(GL_TEXTURE_2D, 0);

    }

    glDisable(GL_SCISSOR_TEST);
    glBindVertexArray(0);
}

void ekg_gpu_data_handler::start() {
    this->amount_of_draw_iterations = 0;
    this->amount_of_data = 0;
    this->amount_of_texture_data_allocated = 0;
    this->allocated_factor = 0.0f;
    this->current_scissor_bind = -1;
    this->should_alloc = false;
}

void ekg_gpu_data_handler::end() {
    bool alloc_flag = this->previous_data_size != this->amount_of_data || this->should_alloc;
    this->previous_data_size = this->amount_of_data;

    if (alloc_flag) {
        this->ticked_refresh_buffers_count++;

        // Pass attrib data to VAO.
        glEnableVertexAttribArray(0);

        // Bind the vertex positions vbo and alloc new data to GPU.
        glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buf_object_vertex_positions);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->cached_vertices.size(), &this->cached_vertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

        // Enable the second location attrib (pass to VAO) from shader.
        glEnableVertexAttribArray(1);

        // Bind vertex materials and alloc new data to GPU.
        glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buf_object_vertex_materials);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->cached_vertices_materials.size(), &this->cached_vertices_materials[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    }

    // Clean the previous data.
    this->cached_vertices.clear();
    this->cached_vertices_materials.clear();
    this->cached_textures.clear();
}

void ekg_gpu_data_handler::calc_view_ortho_2d() {
    glGetFloatv(GL_VIEWPORT, this->viewport);
    ekgmath::ortho2d(this->mat4x4_ortho, 0, this->viewport[2], this->viewport[3], 0);
}

std::vector<float> &ekg_gpu_data_handler::get_cached_vertices() {
    return this->cached_vertices;
}

std::vector<float> &ekg_gpu_data_handler::get_cached_vertices_materials() {
    return this->cached_vertices_materials;
}

ekg_gpu_data &ekg_gpu_data_handler::bind() {
    // Push the data from array.
    ekg_gpu_data &gpu_data = this->allocated_gpu_data[this->amount_of_draw_iterations];

    // Set the raw with previous amount of data.
    gpu_data.raw = this->amount_of_data;

    // Flag the old factor.
    this->allocated_factor = gpu_data.factor;

    gpu_data.category = ekgutil::shape_category::RECTANGLE;
    gpu_data.texture_slot = 0;
    gpu_data.texture = 0;

    gpu_data.factor = 0.0f;
    gpu_data.rect[2] = 0.0f;
    gpu_data.rect[3] = 0.0f;
    gpu_data.id_scissor = this->current_scissor_bind;

    return gpu_data;
}

void ekg_gpu_data_handler::bind_texture(ekg_gpu_data &gpu_data, GLuint &object_id) {
    bool flag_has_cached = false;

    for (uint8_t i = 0; i < (uint8_t) this->cached_textures.size(); i++) {
        GLuint &textures = this->cached_textures.at(i);

        if (textures == object_id) {
            flag_has_cached = true;
            gpu_data.texture_slot = (GLint) i;
            break;
        }
    }

    gpu_data.texture = object_id;

    if (!flag_has_cached) {
        gpu_data.texture_slot = this->amount_of_texture_data_allocated++;
        this->cached_textures.push_back(object_id);
    }
}

void ekg_gpu_data_handler::quit() {
    glDeleteBuffers(1, &this->vertex_buf_object_vertex_positions);
    glDeleteBuffers(1, &this->vertex_buf_object_vertex_materials);
    glDeleteVertexArrays(1, &this->vertex_buffer_arr);
}

void ekg_gpu_data_handler::set_depth_level(float z_level) {
    this->depth_level = z_level;
}

float ekg_gpu_data_handler::get_depth_level() {
    return this->depth_level;
}

uint32_t ekg_gpu_data_handler::get_ticked_refresh_buffers_count() {
    return this->ticked_refresh_buffers_count;
}

void ekg_gpu_data_handler::free(ekg_gpu_data &gpu_data) {
    // Update draw calls time and amount of data handled.
    this->amount_of_data += gpu_data.data;

    if (this->allocated_factor != gpu_data.factor) {
        this->should_alloc = true;
    }

    this->amount_of_draw_iterations++;
}

void ekg_gpu_data_handler::scissor(int32_t x, int32_t y, int32_t w, int32_t h) {
    if (this->current_scissor_bind > this->amount_of_draw_iterations || this->current_scissor_bind < 0) {
        return;
    }

    ekg_gpu_scissor &scissor = this->allocated_gpu_scissor[this->current_scissor_bind];

    scissor.x = x;
    scissor.y = (static_cast<int32_t>(the_ekg_core->get_screen_height())) - (y + h);
    scissor.w = w;
    scissor.h = h;
}

void ekg_gpu_data_handler::bind_scissor(int32_t index) {
    this->current_scissor_bind = index;
}

int32_t ekg_gpu_data_handler::get_scissor_id() {
    return static_cast<int32_t>(this->amount_of_draw_iterations); // what? it is the instance and not the actual instance id.
}

void ekg_gpu_data_handler::next_scissor() {
    this->current_scissor_bind++;
}

void ekg_gpu_data_handler::end_scissor() {
    this->current_scissor_bind = -1;
}

void ekggpu::rectangle(float x, float y, float w, float h, ekgmath::vec4f &color_vec, int32_t line_thickness) {
    // Alloc arrays in CPU.
    ekggpu::push_arr_rect(the_ekg_core->get_gpu_handler().get_cached_vertices(), 0.0f, 0.0f, 1.0f, 1.0f);
    ekggpu::push_arr_rect(the_ekg_core->get_gpu_handler().get_cached_vertices_materials(), 0.0f, 0.0f, 0.0f, 0.0f);

    // Bind GPU data into GPU handler.
    ekg_gpu_data &gpu_data = the_ekg_core->get_gpu_handler().bind();

    // Configure the GPU data.
    gpu_data.data = 6;

    // Set
    gpu_data.category = line_thickness != 0 ? ekgutil::shape_category::OUTLINE : gpu_data.category;
    gpu_data.factor = static_cast<float>(line_thickness);

    gpu_data.rect[0] = static_cast<float>(static_cast<int32_t>(x));
    gpu_data.rect[1] = static_cast<float>(static_cast<int32_t>(y));
    gpu_data.rect[2] = static_cast<float>(static_cast<int32_t>(w));
    gpu_data.rect[3] = static_cast<float>(static_cast<int32_t>(h));

    gpu_data.color[0] = color_vec.x;
    gpu_data.color[1] = color_vec.y;
    gpu_data.color[2] = color_vec.z;
    gpu_data.color[3] = color_vec.w;

    // Free the GPU data.
    the_ekg_core->get_gpu_handler().free(gpu_data);
}

void ekggpu::rectangle(ekgmath::rect &rect, ekgmath::vec4f &color_vec, int32_t line_thickness) {
    ekggpu::rectangle(rect.x, rect.y, rect.w, rect.h, color_vec, line_thickness);
}

void ekggpu::circle(float x, float y, float r, ekgmath::vec4f &color_vec) {
    ekggpu::push_arr_rect(the_ekg_core->get_gpu_handler().get_cached_vertices(), 0.0f, 0.0f, 1.0f, 1.0f);
    ekggpu::push_arr_rect(the_ekg_core->get_gpu_handler().get_cached_vertices_materials(), 0.0f, 0.0f, 0.0f, 0.0f);

    // Bind GPU data into GPU handler.
    ekg_gpu_data &gpu_data = the_ekg_core->get_gpu_handler().bind();

    // Configure the GPU data.
    gpu_data.data = 6;
    gpu_data.category = ekgutil::shape_category::CIRCLE;
    gpu_data.factor = r;

    gpu_data.rect[0] = static_cast<float>(static_cast<int32_t>(x));
    gpu_data.rect[1] = static_cast<float>(static_cast<int32_t>(y));
    gpu_data.rect[2] = static_cast<float>(static_cast<int32_t>(r));
    gpu_data.rect[3] = static_cast<float>(static_cast<int32_t>(r));

    gpu_data.color[0] = color_vec.x;
    gpu_data.color[1] = color_vec.y;
    gpu_data.color[2] = color_vec.z;
    gpu_data.color[3] = color_vec.w;

    // Free the GPU data.
    the_ekg_core->get_gpu_handler().free(gpu_data);
}

void ekggpu::push_arr_rect(std::vector<float> &vec_arr, float x, float y, float w, float h) {
    vec_arr.push_back(x);
    vec_arr.push_back(y);

    vec_arr.push_back(x);
    vec_arr.push_back(y + h);

    vec_arr.push_back(x + w);
    vec_arr.push_back(y + h);

    vec_arr.push_back(x + w);
    vec_arr.push_back(y + h);

    vec_arr.push_back(x + w);
    vec_arr.push_back(y);

    vec_arr.push_back(x);
    vec_arr.push_back(y);
}

void ekggpu::invoke() {
    the_ekg_core->get_gpu_handler().start();
}

void ekggpu::revoke() {
    the_ekg_core->get_gpu_handler().end();
}

void ekggpu::bind_scissor(int32_t index) {
    the_ekg_core->get_gpu_handler().bind_scissor(index);
}

void ekggpu::scissor(int32_t x, int32_t y, int32_t w, int32_t h) {
    the_ekg_core->get_gpu_handler().scissor(x, y, w, h);
}

void ekggpu::scissor(ekgmath::rect &rect) {
    ekggpu::scissor(static_cast<int32_t>(rect.x), static_cast<int32_t>(rect.y), static_cast<int32_t>(rect.w), static_cast<int32_t>(rect.h));
}

int32_t ekggpu::start_scissor() {
    return the_ekg_core->get_gpu_handler().get_scissor_id();
}

void ekggpu::end_scissor() {
    the_ekg_core->get_gpu_handler().end_scissor();
}

void ekggpu::next_scissor() {
    the_ekg_core->get_gpu_handler().next_scissor();
}
