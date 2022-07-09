#pragma once
#ifndef EKG_CORE_H
#define EKG_CORE_H

#include "ekg/impl/ekg_ui_element_abstract.hpp"
#include "ekg_font.hpp"
#include <array>

/**
 * Core of EKG, where everything is processed.
 */
class ekg_core {
protected:
    // For work with modern GPU we need to modify all buffers safe (thinks in performance).
    ekg_gpu_data_handler gpu_handler;

    // Font manager to handle and display (draw) chars at screen.
    ekg_font font_manager;

    // Instances of SDL2.
    SDL_Window* sdl_window_instance;

    // Buffers to we handle in game.
    std::array<ekg_element*, 512> update_buffer;
    std::array<ekg_element*, 512> render_buffer;

    uint16_t sizeof_render_buffer = 0, sizeof_update_buffer = 0;
    uint32_t last_id_used;

    uint32_t focused_element_id;
    uint32_t last_focused_element_id;

    // Concurrent buffers to prevent memory leak/segment fault or invalid elements.
    std::vector<ekg_element*> concurrent_buffer;

    // 0 refresh; 1 fix stack;
    uint8_t todo_flags;

    /*
     * Pass new modified buffers to update buffers.
     */
    void swap_buffers();
public:
    /*
     * Dispatch an event in core.
     */
    void dispatch_todo_event(uint8_t flag);

    /*
     * Access core EKG GPU handler.
     */
    ekg_gpu_data_handler &get_gpu_handler();

    /*
     * Get the main font manager.
     */
    ekg_font &get_font_manager();

    /*
     * Init core of EKG.
     */
    void init();

    /*
     * Shutdown EKG core.
     */
    void quit();

    /*
     * Set concurrent SDL window instance.
     */
    void set_instances(SDL_Window* &sdl_window);

    /*
     * Add element into EKG context.
     */
    void add_element(ekg_element* &element);

    /*
     * Poll events in EKG.
     */
    void process_event_section(SDL_Event &event);

    /*
     * Update the GUI.
     */
    void process_update_section();

    /*
     * Draw the all EKG elements.
     */
    void process_render_section();
};

#endif