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
#include <ekg/impl/ekg_ui_element_checkbox.hpp>
#include <ekg/impl/ekg_ui_element_slider.hpp>
#include <ekg/impl/ekg_ui_element_popup.hpp>
#include <ekg/impl/ekg_ui_element_combobox.hpp>
#include <ekg/impl/ekg_ui_element_textbox.hpp>
#include <ekg/impl/ekg_ui_element_tab.hpp>

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
     * The global variable ptr for all.
     */
    extern ekg_core* const core;

    /*
     * Set offset for axis x docks.
     */
    extern float text_dock_offset;

    /*
     * The dt variable of ekg.
     */
    extern float delta_time;

    /*
     * The clock of entry box.
     */

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
     * Create text-box UI.
     */
    ekg_textbox* textbox();

    /*
     * Create combobox UI.
     */
    ekg_combobox* combobox(const std::string &tag, const std::string &value, const std::vector<std::string> &vec);

    /*
     * Create popup UI.
     */
    ekg_popup* popup(const std::string &tag, const std::vector<std::string> &vec);

    /*
     * Create slider UI.
     */
    ekg_slider* slider(float val, float min, float max);

    /*
     * Create check box UI.
     */
    ekg_checkbox* checkbox(const std::string &text);

    /*
     * Create button UI.
     */
    ekg_button* button(const std::string &text);

    /*
     * Create frame UI.
     */
    ekg_frame* frame();

    /*
     * Get current poll UI event from ekg, note: do not need to delete this ptr.
     */
    ekg_event* event();

     /*
     * Get current theme.
     */
    ekg_theme &theme();

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
        NONE     = 0,
        ABSTRACT = 1,
        SLIDER   = 2,
        COMBOBOX = 3,
        POPUP    = 4,
        BUTTON   = 5,
        CHECKBOX = 6,
        FRAME    = 7,
        TEXTBOX  = 8
    };
};

#endif