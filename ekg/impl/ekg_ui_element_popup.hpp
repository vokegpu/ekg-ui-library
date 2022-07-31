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
#ifndef EKG_UI_ELEMENT_POPUP_H
#define EKG_UI_ELEMENT_POPUP_H

#include "ekg_ui_element_abstract.hpp"

/**
 * # UI \n
 * # Author: Rina \n
 * # Since: 22/07/2022 | 03:30 am \n
 * # Type: Widget \n
 * # Description: Element list with many buttons. \n
 **/
class ekg_popup : public ekg_element {
protected:
    std::string tag;
    std::vector<ekgutil::component> component_list;
    std::string focused_component;
    std::string activy_component;

    uint16_t enum_flags_text_component_dock;
    uint32_t animation_elapsed_ticks = 0;

    float component_text_min_width = 0.0f;
    float component_text_min_height = 0.0f;

    float component_width = 0.0f;
    float component_height = 0.0f;

    float component_text_offset_x = 0.0f;
    float component_text_offset_y = 0.0f;

    float full_height = 0.0f;
    float offset_separator = 0.0;

    int32_t get_component_index(const std::string &text);
    void get_popup_path(std::string &path);
    bool get_component_pos(const std::string &text, float &x, float &y);
public:
    ekg_popup();
    ~ekg_popup();

    void set_tag(const std::string &str);
    std::string get_tag();

    void set_offset_separator(float offset);
    float get_offset_separator();

    void set_opened(bool opened);
    bool is_opened();

    float get_min_text_width();
    float get_min_text_height();

    void set_width(float width);
    void set_height(float height);

    void set_text_dock(uint16_t dock);
    uint16_t get_text_dock();

    void add(const std::vector<std::string> &vec);
    void remove(const std::string &pattern);
    void state(const std::string &pattern, bool enabled);
    void place(ekg_popup* popup);
    void destroy();

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