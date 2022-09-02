#pragma once
#ifndef EKG_UI_ELEMENT_TAB
#define EKG_UI_ELEMENT_TAB

#include "ekg_ui_element_abstract.hpp"

class ekg_tab : ekg_element {
public:
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