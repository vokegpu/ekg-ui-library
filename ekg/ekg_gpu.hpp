#pragma once
#ifndef EKG_GPU_H

#include "ekg_api.hpp"
#include <iostream>
#include <vector>

/**
 * Instead we draw directly everything we just draw using batch mode.
 **/
struct ekg_gpu_data {
    GLuint vbo;
    uint32_t id;
};

/**
 * Handler all data sent to GPU.
 **/
class ekg_gpu_data_handler {
protected:
    std::vector<ekg_gpu_data> list_of_vbo;
    bool should_reload_element_array_buffer;
public:
    void remove_vbo();
    void add_vbo();
    void draw();
};

#endif