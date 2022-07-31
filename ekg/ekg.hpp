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
#ifndef EKG_H
#define EKG_H

#include <iostream>
#include <string>

#include <ekg/api/ekg_core.hpp>
#include <ekg/api/ekg_event.hpp>
#include <ekg/impl/ekg_ui_element_button.hpp>
#include <ekg/impl/ekg_ui_element_frame.hpp>
#include <ekg/impl/ekg_ui_element_check_box.hpp>
#include <ekg/impl/ekg_ui_element_slider.hpp>
#include <ekg/impl/ekg_ui_element_popup.hpp>

/**
 * The global variable ptr for all.
 **/
extern ekg_core* const the_ekg_core;

/**
 * EKG - ez build gui \n
 * Last commentary update: 28/06/22 at 23:45pm (11:45pm) \n
 * \n
 * For help/information or suggestion see: \n
 * https://github/MrsRina/ekg-ez-build-gui to information. \n
 * https://github/MrsRina/ekg-ez-build-gui/ekg-docs docs, there is how use the library EKG. \n
 * https://github/MrsRina/ekg-ez-build-gui/ekg source code of library EKG. \n
 * @license
 * ALL RIGHTS RESERVED TO RINA (aka MrsRina), \n
 * DO NOT REMOVE/SKID/STOLE CREDITS FROM RINA (OWNER OF EKG). \n
 * YOU ARE ABLE TO USE FOR COMMERCIAL PURPOSE. \n
 * \n
 * Last license update: 28/06/22 at 23:53pm (11:53pm) \n
 **/
namespace ekg {
    /*
     * Get hovered element id.
     */
    uint32_t hovered_element_id();

    /*
     * Get hovered element type.
     */
    uint16_t hovered_element_type();

    /*
     * Get current projection version.
     */
    std::string get_version();

    /*
     * Get the depth level of draw context.
     */
    float depth();

    /*
     * Set the depth level.
     */
    void depth(float depth_level);

    /*
     * Init EKG.
     */
    void init(SDL_Window* &sdl_window);

    /*
     * Shutdown and delete stuff of EKG.
     */
    void quit();

    /*
     * Poll poll_event of GUI.
     */
    void poll_event(SDL_Event &sdl_event);

    /*
     * Update the GUI context.
     */
    void update(float dt);

    /*
     * Render all GUI.
     */
    void render();

    /*
     * Set the font used in render.
     */
    void set_font(const char* path);

    /*
     * Set the font size.
     */
    void set_font_size(uint32_t size);

    /*
     * Create popup UI.
     */
    ekg_popup* popup(const std::string &text, const std::vector<std::string> &vec);

    /*
     * Create slider UI.
     */
    ekg_slider* slider(float val, float min, float max);

    /*
     * Create check box UI.
     */
    ekg_check_box* check_box(const std::string &text);

    /*
     * Create button UI.
     */
    ekg_button* button(const std::string &text);

    /*
     * Create frame UI.
     */
    ekg_frame* frame();

    /*
     * Get the current UI event.
     */
    ekg_event &event();

    /**
     * Visibility of elements in screen.
     **/
    enum visibility {
        LOW_PRIORITY, VISIBLE, DISABLED
    };

    /**
     * Dock areas.
     **/
    enum dock {
        UNDEFINED = 1 << 0,
        CENTER    = 1 << 1,
        TOP       = 1 << 2,
        BOTTOM    = 1 << 3,
        LEFT      = 1 << 4,
        RIGHT     = 1 << 5,
        FULL      = 1 << 6
    };

    /**
     * UI types.
     */
    enum ui {
        NONE = 0,
        SLIDER = 1,
        COMBOBOX = 2,
        POPUP = 3,
        BUTTON = 4,
        CHECKBOX = 5,
        FRAME = 6,
        ABSTRACT = 7
    };

    /*
     * Get current theme.
     */
    ekg_theme &theme();

    /**
     * The core instance of EKG.
     **/
    struct core {
    public:
        static float delta_time;

        /*
         * Init the core of EKG.
         */
        static void init();

        /*
         * Shutdown free all.
         */
        static void quit();
    };
};

#endif