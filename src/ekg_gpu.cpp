#include <ekg/ekg.hpp>
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
                                     "uniform mat4 u_mat_matrix;\n"
                                     "uniform vec2 u_vec2_pos;\n"
                                     "\n"
                                     "void main() {\n"
                                     "    gl_Position = u_mat_matrix * vec4(u_vec2_pos + attrib_pos, 0, 1);\n"
                                     "    varying_material = attrib_material;\n"
                                     "}";

            const char* fragment_src = "#version 330 core\n"
                                       "\n"
                                       "in vec2 varying_material;\n"
                                       "\n"
                                       "uniform sampler2D u_sampler2d_texture_active;\n"
                                       "uniform bool u_bool_set_texture;\n"
                                       "uniform vec4 u_vec4_color;\n"
                                       "\n"
                                       "void main() {\n"
                                       "    vec4 fragcolor = u_vec4_color;\n"
                                       "\n"
                                       "    if (u_bool_set_texture) {\n"
                                       "        fragcolor = texture2D(u_sampler2d_texture_active, varying_material.xy);\n"
                                       "        fragcolor = vec4(fragcolor.xyz - ((1.0f - u_vec4_color.xyz) - 1.0f), fragcolor.w - (1.0f - u_vec4_color.w));\n"
                                       "    }\n"
                                       "\n"
                                       "    gl_FragColor = fragcolor;\n"
                                       "}";

            ekgapi::OpenGL::compile_program(this->default_program, vertex_src, fragment_src);
            break;
        }
    }

    // Gen the objects (VAO and VBO).
    glGenVertexArrays(1, &this->vertex_buffer_arr);
    glGenBuffers(1, &this->vertex_buf_object_vertex_positions);
    glGenBuffers(1, &this->vertex_buf_object_vertex_materials);

    // Configure the buffers (peek ekg_gpu_data_handler::end to explain).
    glBindVertexArray(this->vertex_buffer_arr);
    glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buf_object_vertex_positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, 0, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buf_object_vertex_materials);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, 0, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ekg_gpu_data_handler::draw() {
    this->default_program.use();
    this->default_program.set_mat4x4("u_mat_matrix", this->mat4x4_ortho);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Bind VAO and draw the two VBO(s).
    glBindVertexArray(this->vertex_buffer_arr);

    ekg_gpu_data gpu_data;

    // Simulate glMultiDrawArrays.
    for (uint32_t i = 0; i < this->amount_of_draw_iterations; i++) {
        gpu_data = this->gpu_data_list[i];
        
        this->default_program.set_int("u_bool_set_texture", gpu_data.texture != 0);
        this->default_program.set_vec4f("u_vec4_color", gpu_data.color);
        this->default_program.set_vec2f("u_vec2_pos", gpu_data.pos);

        if (gpu_data.texture != 0) {
            glActiveTexture(GL_TEXTURE0 + gpu_data.texture_slot);
            glBindTexture(GL_TEXTURE_2D, gpu_data.texture);

            this->default_program.set_int("u_sampler2d_texture_active", gpu_data.texture_slot);
        }

        glDrawArrays(GL_TRIANGLES, gpu_data.raw, gpu_data.data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glBindVertexArray(0);
}

void ekg_gpu_data_handler::start() {
    this->amount_of_draw_iterations = 0;
    this->amount_of_data = 0;
    this->amount_of_texture_data_allocated = 0;
}

void ekg_gpu_data_handler::end() {
    bool should_realloc = this->previous_data_size != this->amount_of_data;
    this->previous_data_size = this->amount_of_data;

    // Iterate all cached GPU data and pass to CPU arr.
    for (uint32_t i = 0; i < this->cached_data.size(); i++) {

        if (this->gpu_data_list[i].factor != this->cached_data.at(i).factor) {
            should_realloc = true;
        }

        this->gpu_data_list[i] = this->cached_data.at(i);
    }

    if (should_realloc) {
        // Bind the vertex positions vbo and alloc new data to GPU.
        glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buf_object_vertex_positions);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->cached_vertices.size(), &this->cached_vertices[0], GL_STATIC_DRAW);

        // Bind vao and pass attrib data to VAO.
        glBindVertexArray(this->vertex_buffer_arr);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
        glEnableVertexAttribArray(0);

        // Bind vertex materials and alloc new data to GPU.
        glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buf_object_vertex_materials);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->cached_vertices_materials.size(), &this->cached_vertices_materials[0], GL_STATIC_DRAW);

        // Enable the second location attrib (pass to VAO) from shader.
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
        glEnableVertexAttribArray(1);

        // Unbind vbo(s) and vao.
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
    }

    // Clean the previous data.
    this->cached_vertices.clear();
    this->cached_vertices_materials.clear();
    this->cached_data.clear();
    this->cached_textures.clear();
}

void ekg_gpu_data_handler::calc_view_ortho_2d() {
    float viewport[4];
    glGetFloatv(GL_VIEWPORT, viewport);

    ekgmath::ortho2d(this->mat4x4_ortho, 0, viewport[2], viewport[3], 0);
}

std::vector<float> &ekg_gpu_data_handler::get_cached_vertices() {
    return this->cached_vertices;
}

std::vector<float> &ekg_gpu_data_handler::get_cached_vertices_materials() {
    return this->cached_vertices_materials;
}

void ekg_gpu_data_handler::bind(ekg_gpu_data &gpu_data) {
    // Set the raw with previous amount of data.
    gpu_data.raw = (GLint) this->amount_of_data;
    this->cached_data.push_back(gpu_data);

    // Update draw calls time and amount of data handled.
    this->amount_of_draw_iterations += 1;
    this->amount_of_data += gpu_data.data;
}

void ekg_gpu_data_handler::bind_texture(ekg_gpu_data &gpu_data, GLuint &object_id) {
    bool flag_has_cached = false;

    for (uint8_t i = 0; i < (uint8_t) this->cached_textures.size(); i++) {
        GLuint &textures = this->cached_textures.at(i);

        if (textures == object_id) {
            flag_has_cached = true;
            gpu_data.texture_slot = (uint8_t) i;
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
    // TODO delete the current VAO and VBO(s) buffers
}

void ekggpu::rectangle(float x, float y, float w, float h, ekgmath::vec4f &color_vec) {
    // Alloc arrays in CPU.
    ekggpu::push_arr_rect(ekg::core::instance.get_gpu_handler().get_cached_vertices(), 0.0f, 0.0f, w, h);
    ekggpu::push_arr_rect(ekg::core::instance.get_gpu_handler().get_cached_vertices_materials(), 0.0f, 0.0f, 0.0f, 0.0f);

    // Generate a GPU data.
    ekg_gpu_data gpu_data;

    // Configure the GPU data.
    gpu_data.data = 6;
    gpu_data.factor = w * h;

    gpu_data.pos[0] = x;
    gpu_data.pos[1] = y;

    gpu_data.color[0] = color_vec.x;
    gpu_data.color[1] = color_vec.y;
    gpu_data.color[2] = color_vec.z;
    gpu_data.color[3] = color_vec.w;

    // Bind GPU data into GPU handler.
    ekg::core::instance.get_gpu_handler().bind(gpu_data);
}

void ekggpu::rectangle(ekgmath::rect &rect, ekgmath::vec4f &color_vec) {
    ekggpu::rectangle(rect.x, rect.y, rect.w, rect.h, color_vec);
}

void ekggpu::circle(float x, float y, float r, ekgmath::vec4f &color_vec4) {

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
    ekg::core::instance.get_gpu_handler().start();
}

void ekggpu::revoke() {
    ekg::core::instance.get_gpu_handler().end();
}