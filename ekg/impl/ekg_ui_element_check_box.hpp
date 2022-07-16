#pragma once
#ifndef EKG_UI_ELEMENT_CHECK_BOX_H

#include "ekg_ui_element_abstract.hpp"

/**
 * Element GUI Check Box \n
 * Type: Widget
 * Description: The toggleable non final bool mode.
 **/
class ekg_check_box : public ekg_element  {
protected:
    std::string text;

    float min_text_width;
    float min_text_height;

    bool value;

    uint16_t enum_flags_text_dock;
    float text_offset_x, text_offset_y;
public:
    ekg_check_box();
    ~ekg_check_box();

    void set_text(const std::string &string);
    std::string get_text();

    void set_callback_flag(bool val);
    bool get_callback_flag();

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