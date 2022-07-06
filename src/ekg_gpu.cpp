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

    glGenVertexArrays(1, &this->vertex_buffer_arr);
    glGenBuffers(1, &this->vertex_buf_object_vertex_positions);
    glGenBuffers(1, &this->vertex_buf_object_vertex_materials);

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

void ekg_gpu_data_handler::remove_stored_data(uint32_t data_id) {
    int32_t index = -1;

    for (uint32_t i = 0; i < this->gpu_data_list.size(); i++) {
        ekg_gpu_data data = this->gpu_data_list.at(i);

        if (data.id == data_id) {
            index = (int32_t) i;
            break;
        }
    }

    if (index != -1) {
        this->gpu_data_list.erase(this->gpu_data_list.begin() + index);
    }
}

void ekg_gpu_data_handler::draw() {
    this->default_program.use();
    this->default_program.set_mat4x4("u_matrix", this->mat4x4_ortho);

    glBindVertexArray(this->vertex_buffer_arr);
    glMultiDrawArrays(GL_TRIANGLE_FAN, this->fans_starts, this->fans_sizes, this->sizeof_vertices);
    glBindVertexArray(0);
}

bool ekg_gpu_data_handler::get_data_by_id(ekg_gpu_data &data, uint32_t id) {
    for (ekg_gpu_data &gpu_data : this->gpu_data_list) {
        if (gpu_data.id == id) {
            data = gpu_data;
            return true;
        }
    }

    return false;
}

void ekg_gpu_data_handler::access_or_store(ekg_gpu_data &data, uint32_t id) {
    if (!this->get_data_by_id(data, id)) {
        this->gpu_data_list.push_back(data);
    }
}

void ekg_gpu_data_handler::start() {
    this->sizeof_vertices = 0;
    this->flag = true;
}

void ekg_gpu_data_handler::end() {
    this->flag = false;

    uint32_t sizeof_index = 0;
    uint32_t sizeof_i = 0;

    for (ekg_gpu_data &gpu_data : this->gpu_data_list) {
        this->sizeof_vertices += sizeof(float) * gpu_data.vertex_count;
    }

    // Refresh VAO.
    glBindVertexArray(this->vertex_buffer_arr);
    glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buf_object_vertex_positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * );
}

bool ekg_gpu_data_handler::get_flag() {
    return this->flag;
}

uint32_t ekg_gpu_data_handler::get_flag_id() {
    return this->flag_id;
}

void ekg_gpu_data_handler::bind(uint32_t id) {
    this->flag_id = id;
}

void ekg_gpu_data_handler::redirect_data(ekg_gpu_data &data) {
    for (ekg_gpu_data &gpu_data : this->gpu_data_list) {
        if (gpu_data.id == data.id) {
            gpu_data = data;
            break;
        }
    }
}

ekg_gpu_data &ekg_gpu_data_handler::get_concurrent_gpu_data() {
    return this->concurrent_gpu_data;
}

void ekg_gpu_data_handler::calc_view_ortho_2d() {
    float viewport[4];
    glGetFloatv(GL_VIEWPORT, viewport);

    ekgmath::ortho2d(this->mat4x4_ortho, 0, viewport[2], viewport[3], 0);
}

void ekg_gpu_data_handler::bind() {

}

void ekggpu::rectangle(float x, float y, float w, float h, ekgmath::vec4 &color_vec) {
    // Alloc arrays in CPU.
    ekggpu::push_arr_vertex(ekggpu::data().vertices, x, y, w, h);
    ekggpu::push_arr_vertex_color_rgba(ekggpu::data().materials, color_vec.x, color_vec.y, color_vec.z, color_vec.w);
    ekggpu::data().vertex_count += 6;

    // End data catch.
    ekg::core::instance.get_gpu_handler().redirect_data(ekggpu::data());
}

void ekggpu::rectangle(ekgmath::rect &rect, ekgmath::vec4 &color_vec) {
    ekggpu::rectangle(rect.x, rect.y, rect.w, rect.h, color_vec);
}

void ekggpu::circle(float x, float y, float r, ekgmath::vec4 &color_vec4) {

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

void ekggpu::inject(uint8_t id) {
    ekg::core::instance.get_gpu_handler().bind(id);
    ekg::core::instance.get_gpu_handler().access_or_store(ekg::core::instance.get_gpu_handler().get_concurrent_gpu_data(), id);

    ekggpu::data().batch();
}

ekg_gpu_data &ekggpu::data() {
    return ekg::core::instance.get_gpu_handler().get_concurrent_gpu_data();
}

void ekg_gpu_data::batch() {
    this->vertex_count = 0;
    this->vertices.clear();
    this->materials.clear();
}
