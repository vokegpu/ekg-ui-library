#pragma once
#ifndef EKG_H
#define EKG_H

#include <iostream>
#include <string>

#include "ekg_core.hpp"
#include "ekg_element_button.hpp"

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
     * Poll events of GUI.
     */
    void events(SDL_Event &sdl_event);

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
    };

    /**
     * Button element.
     **/
    struct button {
    protected:
        ekg_button* element;
    public:
        button(ekg_button* &element);
        ~button();

        void set_text(std::string &text);
        std::string get_text();

        void set_width(float val);
        float get_width();

        void set_height(float val);
        float get_height();
    };

    ekg::button* create_button(std::string text);
};

#endif