#pragma once
#ifndef EKG_UI_ELEMENT_FRAME_H
#define EKG_UI_ELEMENT_FRAME_H

#include "ekg_ui_element_abstract.hpp"

/**
 * # UI \n
 * # Author: Rina \n
 * # Since: 09/07/2022 | 00:38 am \n
 * # Type: Frame \n
 * # Description: Drag/resize/move or place elements in \n
 **/
class ekg_frame : public ekg_element {
private:
    float min_width;
    float min_height;

    bool no_resizable = false, dragging = false;
    bool no_draggable = false, resizing = false;

    uint16_t enum_target_resize_dock;
    uint16_t enum_target_drag_dock;

    uint16_t enum_flags_resize_dock;
    uint16_t enum_flags_drag_dock;

    float offset_drag_dock;
    float offset_resize_dock;

    std::string tag;
public:
    ekgmath::vec4f bg_color;

    void set_tag(std::string &string);
    std::string get_tag();

    void set_width(float width);
    void set_height(float height);

    void set_min_width(float min);
    float get_min_width();

    void set_min_height(float min);
    float get_min_height();

    void set_resize_dock(uint16_t dock);
    uint16_t get_resize_dock();

    void set_resize_offset(float offset);
    float get_resize_offset();

    void set_drag_dock(uint16_t dock);
    uint16_t get_drag_dock();

    void set_drag_offset(float offset);
    float get_drag_offset();

    bool is_dragging();
    bool is_resizing();
    
    void set(float x, float y, float width, float height);
    void place(ekg_element* element, float x, float y);

    void set_size(float width, float height) override;
    void set_pos(float x, float y) override;
    void on_sync() override;
    void on_pre_event_update(SDL_Event &sdl_event) override;
    void on_event(SDL_Event &sdl_event) override;
    void on_post_event_update(SDL_Event &sdl_event) override;
    void on_update() override;
    void on_draw_refresh() override;
};

#endif