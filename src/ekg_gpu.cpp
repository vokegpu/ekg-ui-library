#include <ekg/ekg_gpu.hpp>

void ekg_gpu_data_handler::init() {
    glGenVertexArrays(1, &this->gpu_data_vao.data);

    this->primitive_draw_size = 6;
    this->primitive_draw_mode = GL_TRIANGLES;
}

void ekg_gpu_data_handler::remove_stored_data(ekg_gpu_data &data) {

}

void ekg_gpu_data_handler::store_data_or_reload(ekg_gpu_data &data) {

}

void ekg_gpu_data_handler::draw() {
    glBindVertexArray(this->gpu_data_vao.data);
    glDrawArrays(this->primitive_draw_mode, 0, this->primitive_draw_size);
    glBindVertexArray(0);
}