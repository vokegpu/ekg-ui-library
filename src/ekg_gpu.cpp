#include <ekg/ekg.hpp>
#include <ekg/ekg_gpu.hpp>

void ekg_gpu_data_handler::init() {
    switch (EKG_CPU_PLATFORM) {
        case ekgapi::cpu::X86: {
            const char* vertex_src = "#version 330 core\n"
                                     "\n"
                                     "layout (location = 0) in vec2 attrib_pos;\n"
                                     "layout (location = 1) in vec4 attrib_fragcolor;\n"
                                     "\n"
                                     "out vec4 varying_fragcolor;\n"
                                     "uniform mat4 u_matrix;\n"
                                     "\n"
                                     "void main() {\n"
                                     "\tgl_Position = u_matrix * vec4(attrib_pos, 0, 1);\n"
                                     "\tvarying_fragcolor = attrib_fragcolor;\n"
                                     "}";

            const char* fragment_src = "#version 330 core\n"
                                       "\n"
                                       "in vec4 varying_fragcolor;\n"
                                       "\n"
                                       "uniform vec4 u_texture_color;\n"
                                       "uniform float u_viewport_height;\n"
                                       "\n"
                                       "uniform bool u_set_texture, u_set_texture_color_filter, u_set_radius, u_set_outline;\n"
                                       "uniform float u_center_x, u_center_y;\n"
                                       "\n"
                                       "uniform float u_radius_dist, u_outline_thickness;\n"
                                       "uniform sampler2D u_active_texture;\n"
                                       "\n"
                                       "float most_longest_fragmentcoord;\n"
                                       "\n"
                                       "void main() {\n"
                                       "vec4 fragcolor = varying_fragcolor;\n"
                                       "gl_FragColor = fragcolor;\n"
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, 0, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buf_object_vertex_materials);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, 0, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ekg_gpu_data_handler::draw() {
    this->default_program.use();
    this->default_program.set_mat4x4("u_matrix", this->mat4x4_ortho);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Bind VAO and draw the two VBO(s).
    glBindVertexArray(this->vertex_buffer_arr);

    // Simulate glMultiDrawArrays.
    for (uint32_t i = 0; i < this->amount_of_draw_iterations; i++) {
        glDrawArrays(GL_TRIANGLE_FAN, this->index_start_arr[i], this->index_end_arr[i]);
    }

    glBindVertexArray(0);

    //glMultiDrawArrays(GL_TRIANGLE_FAN, this->index_start_arr, this->index_end_arr, this->amount_of_draw_iterations);
}

void ekg_gpu_data_handler::start() {
    this->amount_of_draw_iterations = 0;
    this->amount_of_data = 0;
    this->amount_of_texture_data_allocated = 0;
}

void ekg_gpu_data_handler::end() {
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
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(1);

    // Unbind vbo(s) and vao.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Iterate all cached GPU data and pass to draw fans.
    for (uint32_t i = 0; i < this->cached_data.size(); i++) {
        ekg_gpu_data &gpu_data = this->cached_data.at(i);

        // Pass start and end reference.
        this->index_start_arr[i] = gpu_data.raw;
        this->index_end_arr[i] = gpu_data.data;
    }

    // Clean the previous data.
    this->cached_vertices.clear();
    this->cached_vertices_materials.clear();
    this->cached_data.clear();
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

void ekg_gpu_data_handler::bind_texture(GLuint &object_id) {
    this->alloc_texture_arr[this->amount_of_texture_data_allocated++] = object_id;
}

void ekg_gpu_data_handler::quit() {
    // TODO delete the current VAO and VBO(s) buffers
}

void ekggpu::rectangle(float x, float y, float w, float h, ekgmath::vec4f &color_vec) {
    // Alloc arrays in CPU.
    ekggpu::push_arr_vertex(ekg::core::instance.get_gpu_handler().get_cached_vertices(), x, y, w, h);
    ekggpu::push_arr_vertex_color_rgba(ekg::core::instance.get_gpu_handler().get_cached_vertices_materials(), color_vec.x, color_vec.y, color_vec.z, color_vec.w);

    // Generate a GPU data.
    ekg_gpu_data gpu_data;
    gpu_data.data = 6;

    // Bind GPU data into GPU handler.
    ekg::core::instance.get_gpu_handler().bind(gpu_data);
}

void ekggpu::rectangle(ekgmath::rect &rect, ekgmath::vec4f &color_vec) {
    ekggpu::rectangle(rect.x, rect.y, rect.w, rect.h, color_vec);
}

void ekggpu::circle(float x, float y, float r, ekgmath::vec4f &color_vec4) {

}

void ekggpu::push_arr_vertex(std::vector<float> &vec_arr, float x, float y, float w, float h) {
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

void ekggpu::push_arr_vertex_color_rgba(std::vector<float> &vec_arr, float r, float g, float b, float a) {
    vec_arr.push_back(r);
    vec_arr.push_back(g);
    vec_arr.push_back(b);
    vec_arr.push_back(a);

    vec_arr.push_back(r);
    vec_arr.push_back(g);
    vec_arr.push_back(b);
    vec_arr.push_back(a);

    vec_arr.push_back(r);
    vec_arr.push_back(g);
    vec_arr.push_back(b);
    vec_arr.push_back(a);

    vec_arr.push_back(r);
    vec_arr.push_back(g);
    vec_arr.push_back(b);
    vec_arr.push_back(a);

    vec_arr.push_back(r);
    vec_arr.push_back(g);
    vec_arr.push_back(b);
    vec_arr.push_back(a);

    vec_arr.push_back(r);
    vec_arr.push_back(g);
    vec_arr.push_back(b);
    vec_arr.push_back(a);
}

void ekggpu::push_arr_vertex_tex_coords(std::vector<float> &vec_arr, float x, float y, float w, float h) {
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