#pragma once
#ifndef EKG_GPU_H

#include "ekg_api.hpp"
#include "ekg_utility.hpp"

/**
 * Instead we draw directly everything we just draw using batch mode.
 **/
struct ekg_gpu_data {
    std::vector<GLuint> data;
    uint32_t id;

    void alloc();
    void free(uint32_t index);
};

/**
 * Handler all data sent to GPU.
 **/
class ekg_gpu_data_handler {
protected:
    std::vector<ekg_gpu_data> gpu_data_list;
    api::OpenGL::program default_program;

    uint8_t primitive_draw_size;
    GLuint primitive_draw_mode;
    GLuint vertex_arr_attrib;

    bool should_reload_element_array_buffer;
public:
    void init();
    void remove_stored_data(ekg_gpu_data &data);
    void store_data_or_reload(ekg_gpu_data &data);
    void draw();
};

/**
 * Functions to draw shapes and stuff.
 **/
namespace gpu {
    void shape();
};

#endif