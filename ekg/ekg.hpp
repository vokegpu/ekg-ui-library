#pragma once
#ifndef EKG_H
#define EKG_H

#include <iostream>
#include <string>

#include <ekg/api/ekg_core.hpp>
#include <ekg/impl/ekg_ui_element_button.hpp>
#include <ekg/impl/ekg_ui_element_frame.hpp>

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
     * Get current projection version.
     */
    std::string get_version();

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
     * Create button UI.
     */
    ekg_button* button(std::string text);

    /*
     * Create frame UI.
     */
    ekg_frame* frame();

    /**
     * Visibility of elements in screen.
     **/
    enum visibility {
        VISIBLE, LOW_PRIORITY, NONE
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
     * The core instance of EKG.
     **/
    struct core {
    public:
        static ekg_core instance;
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