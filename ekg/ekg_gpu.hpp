#pragma once
#ifndef EKG_GPU_H

#include "ekg_api.hpp"
#include "ekg_utility.hpp"
#include <array>

/**
 * Store GPU data.
 **/
struct ekg_gpu_data {
    uint32_t id = 0;
    uint32_t vertex_count = 0;

    std::vector<float> vertices;
    std::vector<float> materials;

    /*
     * Reset vertex count and clean data.
     */
    void batch();
};

/**
 * Store GPU data VAO final.
 **/
struct ekg_gpu_vao {
    GLuint vao = 0;
    uint16_t size = 0;
};

/**
 * Handler all data sent to GPU.
 **/
class ekg_gpu_data_handler {
protected:
    std::vector<ekg_gpu_data> gpu_data_list;
    ekgapi::OpenGL::program default_program;
    ekg_gpu_data concurrent_gpu_data;

    GLuint vertex_buffer_arr;
    GLuint vertex_buf_object_vertex_positions;
    GLuint vertex_buf_object_vertex_materials;

    uint32_t sizeof_vertices;
    uint32_t sizeof_buffer_vertex_positions;
    uint32_t sizeof_buffer_vertex_materials;

    GLint fans_starts[1024];
    GLsizei fans_sizes[1024];

    float mat4x4_ortho[16];

    bool flag;
    uint32_t flag_id;
public:
    /*
     * Get if GPU handler is performing some task.
     */
    bool get_flag();

    /*
     * Get focused GPU data id from cache.
     */
    uint32_t get_flag_id();

    /*
     * deprecated ...
     */
    GLuint &get_vertex_array_object();

    /*
     * Get the current GPU data.
     */
    ekg_gpu_data &get_concurrent_gpu_data();


    /*
     * Bind VAO to GPU vao list.
     */
    void bind();

    /*
     * Init the GPU handler.
     */
    void init();

    /*
     * Get the stored GPU data from cache.
     */
    bool get_data_by_id(ekg_gpu_data &data, uint32_t id);

    /*
     * Remove GPU data stored from cache.
     */
    void remove_stored_data(uint32_t data_id);

    /*
     * Get GPU data from cache or store if not exists.
     */
    void access_or_store(ekg_gpu_data &data, uint32_t id);

    /*
     * Redirect concurrent GPU data to cache.
     */
    void redirect_data(ekg_gpu_data &data);

    /*
     * Start GPU access section and setup GPU flags before access.
     */
    void start();

    /*
     * Bind GPU data.
     */
    void bind(uint32_t id);

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
     * Invoke GPU to transfer data and cache data.
     */
    void invoke();

    /*
     * Inject data.
     */
    void inject(uint8_t id);

    /*
     * Confirm GPU changes.
     */
    void revoke();

    /*
     * Return concurrent GPU data synchronized with CPU cache.
     */
    ekg_gpu_data &data();
    
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