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
    bool dragging = false;

    float min_text_width;
    float min_text_height;

    float value;
    float min;
    float max;

    uint16_t enum_flags_text_dock;
    uint16_t enum_flag_bar_axis_dock;

    float text_offset_x;
    float text_offset_y;

    float bar_size;
    float bar_width;
    float bar_height;
    float bar[4];
public:
    ekg_slider();
    ~ekg_slider();

    void set_value(float val);
    float get_value();

    void set_min(float val_min);
    float get_min();

    void set_max(float val_max);
    float get_max();

    float get_min_text_width();
    float get_min_text_height();

    void set_text_dock(uint16_t flags);
    uint16_t get_text_dock();

    void set_width(float width);
    void set_height(float height);

    void set_bar_axis(uint16_t dock);
    float get_bar_axis();

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
