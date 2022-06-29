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
protected:
    // Element description.
    std::string element_tag;
    uint8_t element_type;
    uint32_t element_id;

    // States and collections.
    utility::flag flag;
    utility::visibility visibility;
    utility::stack children_stack;
public:
    /* Start of setters and getters. */

    /*
     * Get the flags of element.
     */
    utility::flag &get_flag();

    /*
     * Set the element tag.
     */
    void set_tag(std::string &tag);

    /*
     * Get the current element tag.
     */
    std::string get_tag();

    /*
     * Set a new element id to the element.
     */
    void set_element_id(uint32_t new_element_id);

    /*
     * Get the current element id.
     */
    uint32_t get_element_id();

    /*
     * Get the type of element.
     */
    uint8_t get_element_type();

    /*
     * Set the current element visibility.
     */
    void set_visibility(utility::visibility enum_visibility);

    /*
     * Get the current element visibility.
     */
    utility::visibility get_visibility();

    /*
     * Get parent stack.
     */
    utility::stack &get_children_stack();

    /* End of setters and getters. */
    /* Start of abstract methods. */

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
    virtual void on_render();

    /* End of abstract methods. */
};

#endif