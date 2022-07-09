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

    uint32_t enum_flags_draggable_dock;
    uint32_t enum_flags_resizable_dock;

    std::string tag;
public:
    void set_tag(std::string &string);
    std::string get_tag();

    void set_width(float width);
    void set_height(float height);

    void set_min_width(float min);
    float get_min_width();

    void set_min_height(float min);
    float get_min_height();

    void set_size(float width, float height);
    void set_pos(float x, float y);

    void on_sync() override;
    void on_pre_event_update(SDL_Event &sdl_event) override;
    void on_event(SDL_Event &sdl_event) override;
    void on_post_event_update(SDL_Event &sdl_event) override;
    void on_update() override;
    void on_draw_refresh() override;
};

#endif