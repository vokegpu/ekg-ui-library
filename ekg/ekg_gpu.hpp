#pragma once
#ifndef EKG_GPU_H

#include "ekg_api.hpp"
#include "ekg_utility.hpp"
#include <array>

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

    uint32_t id = 0, iterator = 0, iterator_call_buffer = 0, vertex_size = 0;
    bool flag_has_gen_buffer = false;

    void free(uint32_t index);
    void free();

    void batch();
    void bind();
    void unbind();
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
    std::array<ekg_gpu_vao, 2048> vao_buffer;

    ekgapi::OpenGL::program default_program;
    ekg_gpu_data concurrent_gpu_data;

    uint16_t vao_iterator;
    uint16_t sizeof_vao_buffer;

    uint8_t primitive_draw_size;
    GLuint primitive_draw_mode;
    GLuint vertex_arr_object;

    bool flag;
    uint32_t flag_id;

    float mat4x4_ortho[16];
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
     * Bind immediate vertices vertex array buffer.
     */
    void vertex();

    /*
     * Bind immediate vertices material vertex array object.
     */
    void material();

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
     * Bind main vertex array object.
     */
    void vao();
    
    /*
     * Push modal shape to CPU before GPU.
     */
    void push_arr_vertex(float x, float y, float w, float h);

    /*
     * Push color rgba to CPU before GPU.
     */
    void push_arr_vertex_color_rgba(float r, float g, float b, float a);

    /*
     * Push texture coordinates to CPU before GPU.
     */
    void push_arr_vertex_tex_coords(float x, float y, float w, float h);

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