/**
 * EKG-LICENSE - this software/library LICENSE can not be modified in any instance.
 *
 * --
 * ANY NON-OFFICIAL MODIFICATION IS A CRIME.
 * DO NOT SELL THIS CODE SOFTWARE, FOR USE EKG IN A COMMERCIAL PRODUCT ADD EKG-LICENSE TO PROJECT,
 * RESPECT THE COPYRIGHT TERMS OF EKG, NO SELL WITHOUT EKG-LICENSE (IT IS A CRIME).
 * DO NOT FORK THE PROJECT SOURCE WITHOUT EKG-LICENSE.
 *
 * END OF EKG-LICENSE.
 **/
#pragma once
#ifndef EKG_UI_ELEMENT_SLIDER_H
#define EKG_UI_ELEMENT_SLIDER_H

#include "ekg_ui_element_abstract.hpp"

/**
 * # UI \n
 * # Author: Rina \n
 * # Since: 18/07/2022 | 06:66 pm \n
 * # Type: Widget \n
 * # Description: Drag bar and get value from. \n
 **/
class ekg_slider : public ekg_element {
protected:
    std::string text;

    float min_text_width;
    float min_text_height;

    float value;
    float min;
    float max;

    uint16_t enum_flag_bar_axis_dock;

    float bar_width;
    float bar_height;
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

    void set_width(float width);
    void set_height(float height);

    void set_bar_axis(uint16_t dock);
    float get_bar_axis();

    bool is_dragging();

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
