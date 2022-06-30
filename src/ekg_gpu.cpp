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

void ekg_gpu_data_handler::remove_stored_data(ekg_gpu_data &data) {

}

void ekg_gpu_data_handler::store_data_or_reload(ekg_gpu_data &data) {

}

void ekg_gpu_data_handler::draw() {
    glBindVertexArray(this->vertex_arr_attrib);
    glDrawArrays(this->primitive_draw_mode, 0, this->primitive_draw_size);
    glBindVertexArray(0);
}

void ekg_gpu_data::alloc() {

}

void ekg_gpu_data::free(uint32_t index) {
    glDeleteBuffers(GL_BUFFER_OB, this->data.at(index));
}
