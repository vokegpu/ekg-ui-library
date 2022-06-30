#pragma once
#ifndef EKG_CORE_H
#define EKG_CORE_H

#include "ekg_abstract_element.hpp"
#include "ekg_gpu.hpp"
#include <array>

/**
 * Core of EKG, where everything is processed.
 */
class ekg_core {
protected:
    // For work with modern GPU we need to modify all buffers safe (thinks in performance).
    ekg_gpu_data_handler gpu;

    // Instances of SDL2.
    SDL_Window* sdl_window_instance;

    // Buffers to we handle in game.
    std::array<ekg_abstract_element*, 512> update_buffer;
    std::array<ekg_abstract_element*, 512> render_buffer;

    uint16_t sizeof_render_buffer, sizeof_update_buffer;
    uint32_t last_id_used;

    uint32_t focused_element_id;
    uint32_t last_focused_element_id;

    // Concurrent buffers to prevent memory leak/segment fault or invalid elements.
    std::vector<ekg_abstract_element*> concurrent_buffer;

    // 0 refresh; 1 fix stack;
    uint8_t todo_flags;

    /*
     * Pass new modified buffers to update buffers.
     */
    void swap_buffers();
public:
    void init();
    void set_instances(SDL_Window* &sdl_window);
    void add_element(ekg_abstract_element* element);

    void process_event_section(SDL_Event &event);
    void process_update_section();
    void process_render_section();
};

#endif