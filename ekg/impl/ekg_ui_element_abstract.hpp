#pragma once
#ifndef EKG_UI_ELEMENT_ABSTRACT_H
#define EKG_UI_ELEMENT_ABSTRACT_H

#include <iostream>
#include <string>
#include <SDL2/SDL.h>

#include <ekg/api/ekg_utility.hpp>

/**
 * # UI \n
 * # Type: Abstract \n
 * # Description: Invisible element, abstract \n
 **/
class ekg_element {
protected:
    uint8_t type;

    uint32_t id;
    uint32_t visibility;
    uint32_t master_id;

    ekgutil::flag flag;
    ekgutil::stack children_stack;

    ekgmath::rect scaled;
    ekgmath::rect cache;
    ekgmath::rect rect;
public:
    ekg_element();
    ~ekg_element();

    void set_id(uint32_t element_id);
    uint32_t get_type();

    void set_visibility(uint32_t visible);
    uint32_t get_visibility();

    void set_master_id(uint32_t element_id);
    uint32_t get_id();

    ekgutil::flag &access_flag();
    ekgutil::stack &access_children_stack();

    float get_x();
    float get_y();

    float get_width();
    float get_height();

    void collect_stack(ekgutil::stack &stack);

    /* Start of abstract methods. */

    /*
     * Called when the element just die.
     */
    virtual void on_killed();

    /*
     * Sync data element stuff.
     */
    virtual void on_sync();

    /*
     * Update before process poll_event.
     */
    virtual void on_pre_event_update(SDL_Event &sdl_event);

    /*
     * Process poll_event.
     */
    virtual void on_event(SDL_Event &sdl_event);

    /*
     * Update after process poll_event.
     */
    virtual void on_post_event_update(SDL_Event &sdl_event);

    /*
     * Update stuff in element.
     */
    virtual void on_update();

    /*
     * "Draw" components present in element.
     */
    virtual void on_draw_refresh();

    /* End of abstract methods. */
};

#endif