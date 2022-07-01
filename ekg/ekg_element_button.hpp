#ifndef EKG_ELEMENT_BUTTON_H
#define EKG_ELEMENT_BUTTON_H

#include "ekg_abstract_element.hpp"

/**
 * Element UI Button \n
 * Type: Widget
 * Description: Button widget, check box or boolean mode.
 **/
class ekg_button : public ekg_abstract_element  {
public:
    ekg_button();
    ~ekg_button();

    void on_killed() override;
    void on_sync() override;
    void on_pre_event_update(SDL_Event &sdl_event) override;
    void on_event(SDL_Event &sdl_event) override;
    void on_post_event_update(SDL_Event &sdl_event) override;
    void on_draw_refresh() override;
};

#endif