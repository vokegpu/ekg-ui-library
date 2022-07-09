#ifndef EKG_UI_ELEMENT_BUTTON_H
#define EKG_UI_ELEMENT_BUTTON_H

#include "ekg_ui_element_abstract.hpp"

/**
 * Element GUI Button \n
 * Type: Widget
 * Description: Button widget, check box or boolean mode.
 **/
class ekg_button : public ekg_element  {
protected:
    std::string text;

    float min_text_width;
    float min_text_height;
public:
    ekg_button();
    ~ekg_button();

    void set_text(std::string &string);
    std::string get_text();

    void on_killed() override;
    void on_sync() override;
    void on_pre_event_update(SDL_Event &sdl_event) override;
    void on_event(SDL_Event &sdl_event) override;
    void on_post_event_update(SDL_Event &sdl_event) override;
    void on_draw_refresh() override;
};

#endif