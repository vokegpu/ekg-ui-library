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
#ifndef EKG_UI_ELEMENT_TEXTBOX
#define EKG_UI_ELEMENT_TEXTBOX

#include "ekg_ui_element_abstract.hpp"

/**
 * # UI \n
 * # Author: Rina \n
 * # Since: 03/08/2022 | 01:36 am \n
 * # Type: Widget \n
 * # Description: Type box with unlimited chars. \n
 **/
class ekg_textbox : public ekg_element {
protected:
	std::string text;

	float min_text_width;
	float min_text_height;
    
	float last_pos_x;
	float last_pos_y;
    
    ekgtext::box box;
public:
	ekg_textbox();
	~ekg_textbox();

	void set_text(const std::string &string);
    std::string get_text();

    float get_min_text_width();
    float get_min_text_height();

    void set_max_columns(int32_t amount);
    int32_t get_max_columns();

    void set_max_rows(int32_t amount);
    int32_t get_max_rows();

    void set_width(float width);
    void set_height(float height);

    void set_size(float width, float height) override;
    void set_pos(float x, float y) override;
    void on_killed() override;
    void on_sync() override;
    void on_pre_event_update(SDL_Event &sdl_event) override;
    void on_event(SDL_Event &sdl_event) override;
    void on_post_event_update(SDL_Event &sdl_event) override;
    void on_update() override;
    void on_draw_refresh() override;
};

#endif