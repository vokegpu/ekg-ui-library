#pragma once

#ifndef EKG_UI_ELEMENT_SLIDER_H
#define EKG_UI_ELEMENT_SLIDER_H

#include "ekg_ui_element_abstract.hpp"

/**
 * Element GUI Slider \n
 * Type: Widget
 * Description: Bar progress or normal slider.
 **/
class ekg_slider : public ekg_element {
protected:
    float min_text_width;
    float min_text_height;

    float value;
    float min_value;
    float max_value;

    uint16_t enum_flags_text_dock;

    float text_offset_x;
    float text_offset_y;

    float bar_offset_x;
    float bar_offset_y;
public:
    void set_value(float val);
    float get_value();

    void set_min_value(float min);
    float get_min_value();

    void set_max_value(float max);
    float get_max_value();

    float get_min_text_width();
    float get_min_text_height();

    void set_text_dock(uint16_t flags);
    uint16_t get_text_dock();

    void set_width(float width);
    void set_height(float height);

    void set_size(float width, float height) override;
    void set_pos(float x, float y) override;
    void on_killed() override;
    void on_sync() override;
    void on_pre_event_update(SDL_Event &sdl_event) override;
    void on_event(SDL_Event &sdl_event) override;
    void on_post_event_update(SDL_Event &sdl_event) override;
    void on_draw_refresh() override;
};

#endif