#include <ekg/ekg.hpp>
#include <ekg/ekg_gpu.hpp>

void ekg_gpu_data_handler::init() {
    switch (EKG_CPU_PLATFORM) {
        case api::cpu::X86: {
            const char* vertex_src = "#version 330 core\n"
                                     "\n"
                                     "layout (location = 0) in vec2 attrib_pos;\n"
                                     "layout (location = 1) in vec4 attrib_fragcolor;\n"
                                     "\n"
                                     "out vec4 varying_fragcolor;\n"
                                     "uniform mat4 u_matrix;\n"
                                     "\n"
                                     "void main() {\n"
                                     "\tgl_Position = u_matrix * vec4(attrib_pos, 0, 1.0f);\n"
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

            api::OpenGL::compile_program(this->default_program, vertex_src, fragment_src);
            break;
        }
    }
    
    glGenVertexArrays(1, &this->vertex_arr_object);

    this->primitive_draw_size = 0; // 6 vertex.
    this->primitive_draw_mode = GL_TRIANGLES;
}

void ekg_gpu_data_handler::remove_stored_data(uint32_t data_id) {
    int32_t index = -1;

    for (uint32_t i = 0; i < this->gpu_data_list.size(); i++) {
        ekg_gpu_data data = this->gpu_data_list.at(i);

        if (data.id == data_id) {
            data.free();
            index = (int32_t) i;
            break;
        }
    }

    if (index != -1) {
        this->gpu_data_list.erase(this->gpu_data_list.begin() + index);
    }
}

void ekg_gpu_data_handler::draw() {
    glBindVertexArray(this->vertex_arr_object);

    this->default_program.use();
    this->default_program.set_mat4x4("u_matrix", this->mat4x4_ortho);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(this->primitive_draw_mode, 0, this->primitive_draw_size);
    glUseProgram(0);
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
    this->primitive_draw_size = 0;
    this->flag = true;

    // Use us shader program.
    this->default_program.use();
}

void ekg_gpu_data_handler::end() {
    // Calc final draw size.
    for (ekg_gpu_data &gpu_data : this->gpu_data_list) {
        this->primitive_draw_size += gpu_data.vertex_size;
    }

    this->flag = false;
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

GLuint &ekg_gpu_data_handler::get_vertex_array_object() {
    return this->vertex_arr_object;
}

void ekg_gpu_data::free(uint32_t index) {
    //glDeleteBuffers(1, &this->data.at(index));
}

void ekg_gpu_data::batch() {
    this->iterator_call_buffer = 0;
}

void ekg_gpu_data::bind() {
    GLuint buffer;
    this->flag_has_gen_buffer = false;

    if (this->data.size() == this->iterator || this->data.empty()) {
        glGenBuffers(1, &buffer);
        this->data.push_back(buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);

        this->flag_has_gen_buffer = true;
        this->iterator++;
        this->iterator_call_buffer = this->iterator;
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, this->data.at(this->iterator_call_buffer));
        this->iterator_call_buffer++;
    }
}

void ekg_gpu_data::free() {
    for (GLuint ids : this->data) {
        //glDeleteBuffers(1, &ids);
    }

    this->data.clear();
}

void ekg_gpu_data::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void gpu::rectangle(float x, float y, float w, float h, ekgmath::vec4 &color_vec) {
    // Alloc arrays in CPU.
    gpu::push_arr_vertex(x, y, w, h);
    gpu::push_arr_vertex_color_rgba(color_vec.x, color_vec.y, color_vec.z, color_vec.w);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Start data catch.
    gpu::data().bind();
    gpu::data().vertex_size = 6;

    // Bind VAO.
    gpu::vao();

    // Pass vertex positions to GPU.
    if (gpu::data().flag_has_gen_buffer) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, ALLOCATE_ARR_VERTEX, GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 12, ALLOCATE_ARR_VERTEX);
    }

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

    gpu::data().bind();
    gpu::vao();

    // Pass vertex colors to GPU.
    if (gpu::data().flag_has_gen_buffer) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, ALLOCATE_ARR_VERTEX_COLOR_RGBA, GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 24, ALLOCATE_ARR_VERTEX_COLOR_RGBA);
    }

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);

    // End data catch.
    ekg::core::instance.get_gpu_handler().redirect_data(gpu::data());
}

void gpu::rectangle(ekgmath::rect &rect, ekgmath::vec4 &color_vec) {
    gpu::rectangle(rect.x, rect.y, rect.w, rect.h, color_vec);
}

void gpu::circle(float x, float y, float r, ekgmath::vec4 &color_vec4) {

}
void gpu::push_arr_vertex(float &x, float &y, float &w, float &h) {
    uint8_t it = 0;

    ALLOCATE_ARR_VERTEX[it++] = x;
    ALLOCATE_ARR_VERTEX[it++] = y;

    ALLOCATE_ARR_VERTEX[it++] = x;
    ALLOCATE_ARR_VERTEX[it++] = y + h;

    ALLOCATE_ARR_VERTEX[it++] = x + w;
    ALLOCATE_ARR_VERTEX[it++] = y + h;

    ALLOCATE_ARR_VERTEX[it++] = x + w;
    ALLOCATE_ARR_VERTEX[it++] = y + h;

    ALLOCATE_ARR_VERTEX[it++] = x + w;
    ALLOCATE_ARR_VERTEX[it++] = y;

    ALLOCATE_ARR_VERTEX[it++] = x;
    ALLOCATE_ARR_VERTEX[it++] = y;
}

void gpu::push_arr_vertex_color_rgba(float &r, float &g, float &b, float &a) {
    uint8_t it = 0;

    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = r;
    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = g;
    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = b;
    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = a;

    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = r;
    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = g;
    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = b;
    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = a;

    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = r;
    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = g;
    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = b;
    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = a;

    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = r;
    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = g;
    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = b;
    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = a;

    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = r;
    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = g;
    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = b;
    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = a;

    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = r;
    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = g;
    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = b;
    ALLOCATE_ARR_VERTEX_COLOR_RGBA[it++] = a;
}

void gpu::push_arr_vertex_tex_coords(float &x, float &y, float &w, float &h) {
    uint8_t it = 0;

    ALLOCATE_ARR_TEX_COORDS[it++] = x;
    ALLOCATE_ARR_TEX_COORDS[it++] = y;
    ALLOCATE_ARR_TEX_COORDS[it++] = x;
    ALLOCATE_ARR_TEX_COORDS[it++] = y + h;
    ALLOCATE_ARR_TEX_COORDS[it++] = x + w;
    ALLOCATE_ARR_TEX_COORDS[it++] = y + h;
    ALLOCATE_ARR_TEX_COORDS[it++] = x + w;
    ALLOCATE_ARR_TEX_COORDS[it++] = y + h;
    ALLOCATE_ARR_TEX_COORDS[it++] = x + w;
    ALLOCATE_ARR_TEX_COORDS[it++] = y;
    ALLOCATE_ARR_TEX_COORDS[it++] = x;
    ALLOCATE_ARR_TEX_COORDS[it++] = y;
}

void gpu::invoke() {
    ekg::core::instance.get_gpu_handler().start();
}

void gpu::revoke() {
    ekg::core::instance.get_gpu_handler().end();
}

void gpu::inject(uint8_t id) {
    ekg::core::instance.get_gpu_handler().bind(id);
    ekg::core::instance.get_gpu_handler().access_or_store(ekg::core::instance.get_gpu_handler().get_concurrent_gpu_data(), id);
}

ekg_gpu_data &gpu::data() {
    return ekg::core::instance.get_gpu_handler().get_concurrent_gpu_data();
}

void gpu::vao() {
    glBindVertexArray(ekg::core::instance.get_gpu_handler().get_vertex_array_object());
}
