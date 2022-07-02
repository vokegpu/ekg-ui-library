#pragma once
#ifndef EKG_GPU_H

#include "ekg_api.hpp"
#include "ekg_utility.hpp"

/* Start of allocated arrays. */
static float ALLOCATE_ARR_VERTEX[12];
static float ALLOCATE_ARR_VERTEX_COLOR_RGBA[24];
static float ALLOCATE_ARR_TEX_COORDS[12];
/* End of allocated arrays. */

/**
 * Instead we draw directly everything we just draw using batch mode.
 **/
struct ekg_gpu_data {
    std::vector<GLuint> data;
    uint32_t id, iterator, iterator_call_buffer;
    bool flag_has_gen_buffer;

    void free(uint32_t index);
    void free();

    void batch();
    void bind();
    void unbind();
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

    bool flag;
    uint32_t flag_id;
public:
    bool get_flag();
    uint32_t get_flag_id();

    void init();
    bool get_gpu_data_by_id(ekg_gpu_data &data, uint32_t id);
    void remove_stored_data(uint32_t data_id);
    void store_or_push(ekg_gpu_data &data, uint32_t id);
    void start();
    void bind(uint32_t id);
    void end();
    void draw();
};

/**
 * Functions to draw shapes.
 **/
namespace gpu {
    /*
     * Push modal shape to CPU before GPU.
     */
    void push_arr_vertex(float &x, float &y, float &w, float &h);

    /*
     * Push color rgba to CPU before GPU.
     */
    void push_arr_vertex_color_rgba(float &r, float &g, float &b, float &a);

    /*
     * Push texture coordinates to CPU before GPU.
     */
    void push_arr_vertex_tex_coords(float &x, float &y, float &w, float &h);

    /*
     * Draw rectangle.
     */
    void rectangle(ekgmath::rect &rect, ekgmath::vec4 &color_vec);

    /*
     * Draw custom pos and size rectangle
     */
    void rectangle(float x, float y, float w, float h, ekgmath::vec4 &color_vec);

    /*
     * Draw circle.
     */
    void circle(float x, float y, float r, ekgmath::vec4 &color_vec4);
};

#endif