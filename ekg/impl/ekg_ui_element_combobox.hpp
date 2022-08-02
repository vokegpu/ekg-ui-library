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
#ifndef EKG_COMBOBOX_H
#define EKG_COMBOBOX_H

#include "ekg_ui_element_abstract.hpp"

/**
 * # UI \n
 * # Author: Rina \n
 * # Since: 31/07/2022 | 05:08 am \n
 * # Type: Widget \n
 * # Description: Select many options in a box. \n
 **/
class ekg_combobox : public ekg_element {
protected:
    std::string value;
    std::vector<std::string> value_list;

    float text_offset_x;
    float text_offset_y;
    uint16_t enum_flags_text_dock;

    float min_text_width;
    float min_text_height;

    uint32_t popup_id;
    bool contains(const std::string &str);
public:
    float get_min_text_width();
    float get_min_text_height();

    void set_value(const std::string &val);
    std::string get_value();

    void set_text_dock(uint16_t flags);
    uint16_t get_text_dock();

    void set_width(float width);
    void set_height(float height);

    void add(const std::vector<std::string> &vec);
    void remove(const std::string &pattern);

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