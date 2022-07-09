#pragma once
#ifndef EKG_ELEMENT_FRAME_H
#define EKG_ELEMENT_FRAME_H

#include "ekg_ui_element.hpp"

/**
 * # UI \n
 * # Author: Rina \n
 * # Since: 09/07/2022 | 00:38 am \n
 * # Type: Frame \n
 * # Description: Drag/resize/move or place elements in \n
 **/
class ekg_frame : public ekg_element {
public:
    float min_width;
    float min_height;

    uint8_t enum_flags_draggable_dock;
    uint8_t enum_flags_resizable_dock;

    void on_pre_event_update(SDL_Event &sdl_event) override;
    void on_event(SDL_Event &sdl_event) override;
    void on_post_event_update(SDL_Event &sdl_event) override;
    void on_update() override;
    void on_draw_refresh() override;
};

#endif