#pragma once
#ifndef EKG_GPU_H
#define EKG_GPU_H

#include "ekg_api.hpp"
#include "ekg_utility.hpp"
#include <array>

/**
 * Store GPU data for update at once.
 **/
struct ekg_gpu_data {
    GLint data = 0;
    GLint raw = 0;
};

/**
 * Store active texture.
 **/


/**
 * Handler all data sent to GPU.
 **/
class ekg_gpu_data_handler {
protected:
    std::vector<GLfloat> cached_vertices;
    std::vector<GLfloat> cached_vertices_materials;
    std::vector<ekg_gpu_data> cached_data;
    std::vector<GLuint> cached_textures;

    GLint index_start_arr[1024];
    GLint index_end_arr[1024];
    GLint index_texture_active_arr[1024];

    GLuint vertex_buffer_arr;
    GLuint vertex_buf_object_vertex_positions;
    GLuint vertex_buf_object_vertex_materials;

    uint32_t amount_of_draw_iterations;
    uint32_t amount_of_data;
    uint8_t amount_of_texture_data_allocated;

    ekgapi::OpenGL::program default_program;
    float mat4x4_ortho[16];
public:
    /*
     * Get current GPU cached vertices.
     */
    std::vector<float> &get_cached_vertices();

    /*
     * Get current GPU cached vertices with materials (texture coordinates UV or color RGBA).
     */
    std::vector<float> &get_cached_vertices_materials();

    /*
     * Init the GPU handler.
     */
    void init();

    /*
     * Free memory GPU buffers.
     */
    void quit();

    /*
     * Bind GPU data.
     */
    void bind(ekg_gpu_data &gpu_data);

    /*
     * Alloc texture to the concurrent cached texture list.
     */
    void bind_texture(GLuint &object_id);

    /*
     * Start GPU access section and setup GPU flags before access.
     */
    void start();

    /*
     * End GPU access section.
     */
    void end();

    /*
     * Update matrix.
     */
    void calc_view_ortho_2d();

    /*
     * Draw ALLs GPU data.
     */
    void draw();
};

/**
 * Functions to draw shapes.
 **/
namespace ekggpu {
    /*
     * Bind texture to be used after.
     */
    void alloc_texture(GLuint gl_object);

    /*
     * Invoke GPU to transfer data and cache data.
     */
    void invoke();

    /*
     * Confirm GPU changes.
     */
    void revoke();
    
    /*
     * Push modal shape to CPU before GPU.
     */
    void push_arr_vertex(std::vector<float> &vec_arr, float x, float y, float w, float h);

    /*
     * Push color rgba to CPU before GPU.
     */
    void push_arr_vertex_color_rgba(std::vector<float> &vec_arr, float r, float g, float b, float a);

    /*
     * Push texture coordinates to CPU before GPU.
     */
    void push_arr_vertex_tex_coords(std::vector<float> &vec_arr, float x, float y, float w, float h);

    /*
     * Draw rectangle.
     */
    void rectangle(ekgmath::rect &rect, ekgmath::vec4f &color_vec);

    /*
     * Draw custom pos and size rectangle
     */
    void rectangle(float x, float y, float w, float h, ekgmath::vec4f &color_vec);

    /*
     * Draw circle.
     */
    void circle(float x, float y, float r, ekgmath::vec4f &color_vec4);
};

#endif