#pragma once
#ifndef EKG_ABSTRACT_ELEMENT_H
#define EKG_ABSTRACT_ELEMENT_H

#include <iostream>
#include <string>
#include <SDL2/SDL.h>

#include "ekg_utility.hpp"

/**
 * Create elements connected by abstract element class.
 **/
class ekg_abstract_element {
public:
    uint32_t master_id;

    // Element description.
    uint8_t type;
    uint32_t id;

    // States and collections.
    utility::flag flag;
    utility::visibility visibility;
    utility::stack children_stack;

    // Metrics of element.
    ekgmath::rect rect;
public:
    ekg_abstract_element();
    ~ekg_abstract_element();

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
     * Update before process events.
     */
    virtual void on_pre_event_update(SDL_Event &sdl_event);

    /*
     * Process events.
     */
    virtual void on_event(SDL_Event &sdl_event);

    /*
     * Update after process events.
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