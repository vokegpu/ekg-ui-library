#include <ekg/ekg.hpp>
#include <ekg/ekg_gpu.hpp>

void ekg_gpu_data_handler::init() {
    switch (EKG_CPU_PLATFORM) {
        case api::cpu::X86: {
            const char* vertex_src = "#version 330 core\n"
                                     "\n"
                                     "layout (location = 0) in vec4 attrib_pos;\n"
                                     "layout (location = 1) in vec4 attrib_fragcolor;\n"
                                     "\n"
                                     "out vec4 varying_fragcolor;\n"
                                     "uniform mat4 u_matrix;\n"
                                     "\n"
                                     "void main() {\n"
                                     "\tgl_Position = u_matrix * attrib_pos;\n"
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
                                       "\tvec4 fragcolor = varying_fragcolor;\n"
                                       "\n"
                                       "\tif (u_set_texture) {\n"
                                       "\t\tfragcolor = texture2D(u_active_texture, varying_fragcolor.xy);\n"
                                       "\t}\n"
                                       "\n"
                                       "\t// Get the fragment color position.\n"
                                       "\tvec2 fragcoord = vec2(gl_FragCoord.x, u_viewport_height - gl_FragCoord.y);\n"
                                       "\n"
                                       "\tif (u_set_radius) {\n"
                                       "\t\tfloat r = u_radius_dist / 2.0f;\n"
                                       "\n"
                                       "\t\t// First calc. the diff from the center and middle of top square.\n"
                                       "\t\tfloat square_x = u_center_x;\n"
                                       "\t\tfloat square_y = u_center_y - r;\n"
                                       "\n"
                                       "\t\tfloat diff_square_x = u_center_x - square_x;\n"
                                       "\t\tfloat diff_square_y = u_center_y - square_y;\n"
                                       "\n"
                                       "\t\t// Get the fragment pos diff from the center.\n"
                                       "\t\tfloat dx = u_center_x - fragcoord.x;\n"
                                       "\t\tfloat dy = u_center_y - fragcoord.y;\n"
                                       "\n"
                                       "\t\t// Calc. the distances.\n"
                                       "\t\tfloat dist_square_center = (diff_square_x * diff_square_x + diff_square_y * diff_square_y);\n"
                                       "\t\tfloat dist_fragment_center = (dx * dx + dy * dy);\n"
                                       "\n"
                                       "\t\t// Compare them and remove alpha if out of bounding circle.\n"
                                       "\t\tif (dist_fragment_center > dist_square_center) {\n"
                                       "\t\t\tfragcolor.a = 0.0f;\n"
                                       "\t\t}\n"
                                       "\t}\n"
                                       "}";

            api::OpenGL::compile_program(this->default_program, vertex_src, fragment_src);
            break;
        }
    }
    
    glGenVertexArrays(1, &this->vertex_arr_attrib);

    this->primitive_draw_size = 6; // 6 vertex.
    this->primitive_draw_mode = GL_TRIANGLES;
}

void ekg_gpu_data_handler::remove_stored_data(uint32_t data_id) {
    int32_t index = -1;

    for (uint32_t i = 0; i < this->gpu_data_list.size(); i++) {
        ekg_gpu_data data = this->gpu_data_list.at(i);

        if (data.id == data_id) {
            data.free();
            index = i;
        }
    }

    if (index != -1) {
        this->gpu_data_list.erase(this->gpu_data_list.begin() + index);
    }
}

void ekg_gpu_data_handler::draw() {
    utility::log("hello sou linda " + std::to_string(this->primitive_draw_size));

    glBindVertexArray(this->vertex_arr_attrib);
    glDrawArrays(this->primitive_draw_mode, 0, this->primitive_draw_size);
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
    bool flag = this->get_data_by_id(data, id);

    if (!flag) {
        this->gpu_data_list.push_back(data);
    }
}

void ekg_gpu_data_handler::start() {
    this->primitive_draw_size = 0;
    this->flag = true;
}

void ekg_gpu_data_handler::end() {
    for (ekg_gpu_data &gpu_data : this->gpu_data_list) {
        utility::log(std::to_string(gpu_data.data.size()));

        this->primitive_draw_size += 6 * gpu_data.data.size();
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
        glBindBuffer(GL_ARRAY_BUFFER, buffer);

        this->flag_has_gen_buffer = true;
        this->data.push_back(buffer);
        this->iterator++;
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
    if (!ekg::core::instance.get_gpu_handler().get_flag()) {
        return;
    }

    // Alloc arrays in CPU.
    gpu::push_arr_vertex(x, y, w, h);
    gpu::push_arr_vertex_color_rgba(color_vec.x, color_vec.y, color_vec.z, color_vec.w);

    // Create or get a current instance.
    ekg_gpu_data data;
    ekg::core::instance.get_gpu_handler().access_or_store(data, ekg::core::instance.get_gpu_handler().get_flag_id());

    // Start data catch.
    data.batch();
    data.bind();

    // Pass vertex positions to GPU.
    if (data.flag_has_gen_buffer) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, ALLOCATE_ARR_VERTEX, GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 12, ALLOCATE_ARR_VERTEX);
    }

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);

    data.bind();

    // Pass vertex colors to GPU.
    if (data.flag_has_gen_buffer) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 18, ALLOCATE_ARR_VERTEX_COLOR_RGBA, GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 18, ALLOCATE_ARR_VERTEX_COLOR_RGBA);
    }

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);

    // End data catch.
    ekg::core::instance.get_gpu_handler().redirect_data(data);
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

    ALLOCATE_ARR_VERTEX[it++] = r;
    ALLOCATE_ARR_VERTEX[it++] = g;
    ALLOCATE_ARR_VERTEX[it++] = b;
    ALLOCATE_ARR_VERTEX[it++] = a;

    ALLOCATE_ARR_VERTEX[it++] = r;
    ALLOCATE_ARR_VERTEX[it++] = g;
    ALLOCATE_ARR_VERTEX[it++] = b;
    ALLOCATE_ARR_VERTEX[it++] = a;

    ALLOCATE_ARR_VERTEX[it++] = r;
    ALLOCATE_ARR_VERTEX[it++] = g;
    ALLOCATE_ARR_VERTEX[it++] = b;
    ALLOCATE_ARR_VERTEX[it++] = a;

    ALLOCATE_ARR_VERTEX[it++] = r;
    ALLOCATE_ARR_VERTEX[it++] = g;
    ALLOCATE_ARR_VERTEX[it++] = b;
    ALLOCATE_ARR_VERTEX[it++] = a;

    ALLOCATE_ARR_VERTEX[it++] = r;
    ALLOCATE_ARR_VERTEX[it++] = g;
    ALLOCATE_ARR_VERTEX[it++] = b;
    ALLOCATE_ARR_VERTEX[it++] = a;

    ALLOCATE_ARR_VERTEX[it++] = r;
    ALLOCATE_ARR_VERTEX[it++] = g;
    ALLOCATE_ARR_VERTEX[it++] = b;
    ALLOCATE_ARR_VERTEX[it++] = a;
}

void gpu::push_arr_vertex_tex_coords(float &x, float &y, float &w, float &h) {
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

void gpu::invoke(uint32_t id) {
    ekg::core::instance.get_gpu_handler().start();
    ekg::core::instance.get_gpu_handler().bind(id);
}

void gpu::revoke() {
    ekg::core::instance.get_gpu_handler().end();
}