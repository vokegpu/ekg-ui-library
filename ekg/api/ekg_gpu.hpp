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
    uint8_t category = ekgutil::shape_category::RECTANGLE;

    GLint texture_slot = 0;
    GLuint texture = 0;

    GLint data = 0;
    GLint raw = 0;

    float color[4];
    float pos[2];
    float factor = 0;
};

/**
 * Store dynamic GPU data.
 **/
struct ekg_gpu_dynamic_data {
    float normalised_value;
    uint16_t category;
};

/**
 * Handler all data sent to GPU.
 **/
class ekg_gpu_data_handler {
protected:
    uint32_t ticked_refresh_buffers_count;
    GLint previous_data_size;
    GLint amount_of_texture_data_allocated;
    GLint amount_of_data;
    uint32_t amount_of_draw_iterations;

    std::vector<GLfloat> cached_vertices;
    std::vector<GLfloat> cached_vertices_materials;
    std::vector<GLuint> cached_textures;

    GLuint vertex_buffer_arr;
    GLuint vertex_buf_object_vertex_positions;
    GLuint vertex_buf_object_vertex_materials;

    float viewport[4];
    float mat4x4_ortho[16];
    float depth_level = 1.0f;

    ekg_gpu_data gpu_data_list[2048];
    ekg_gpu_dynamic_data gpu_dynamic_list[2048];

    bool should_alloc;
    float allocated_factor;

    ekgapi::OpenGL::program default_program;
public:
    /*
     * Get current count.
     */
    uint32_t get_ticked_refresh_buffers_count();

    /*
     * Set the draw z level.
     */
    void set_depth_level(float z_level);

    /*
     * Get the depth level.
     */
    float get_depth_level();

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
    ekg_gpu_data &bind();

    /*
     * Free the previous bind GPU data.
     */
    void free(ekg_gpu_data &gpu_data);

    /*
     * Alloc texture to the concurrent cached texture list.
     */
    void bind_texture(ekg_gpu_data &gpu_data, GLuint &object_id);

    /*
     * Start GPU access section and setup GPU flags before access.
     */
    void start();

    /*
     * End GPU access section.
     */
    void end();

    /*
     * Prepare to draw.
     */
    void prepare();

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
    void push_arr_rect(std::vector<float> &vec_arr, float x, float y, float w, float h);

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