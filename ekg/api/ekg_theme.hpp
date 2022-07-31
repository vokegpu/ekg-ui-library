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
#ifndef EKG_THEME_H
#define EKG_THEME_H

#include "ekg_utility.hpp"

/**
 * EKG supports color themes not real UI themes.
 **/
struct ekg_theme {
    /*
     * Make sure the instance is not default.
     */
    bool validation = false;

    /* Start of string colors. */
    ekgmath::vec4f string_enabled_color;
    ekgmath::vec4f string_disabled_color;
    ekgmath::vec4f string_value_color;
    /* End of string colors. */

    /* Start of button colors. */
    ekgmath::vec4f button_background;
    ekgmath::vec4f button_highlight;
    ekgmath::vec4f button_activy;
    /* End of button colors. */

    /* Start of check box colors. */
    ekgmath::vec4f check_box_background;
    ekgmath::vec4f check_box_highlight;
    ekgmath::vec4f check_box_activy;
    /* End of check box colors. */

    /* Start of slider colors. */
    ekgmath::vec4f slider_background;
    ekgmath::vec4f slider_highlight;
    ekgmath::vec4f slider_activy;
    /* End of slider colors. */

    /* Start of popup colors. */
    ekgmath::vec4f popup_background;
    ekgmath::vec4f popup_highlight;
    ekgmath::vec4f popup_border;
    /* End of popup colors. */

    /* Start of immediate popup colors. */
    ekgmath::vec4f immediate_popup_background;
    ekgmath::vec4f immediate_popup_border;
    /* End of immediate popup colors. */

    /* Start of frame colors. */
    ekgmath::vec4f frame_background;
    ekgmath::vec4f frame_border;
    /* End of frame colors. */
};

/**
 * The theme service for manipulation of all themes.
 */
class ekg_theme_service {
protected:
    ekg_theme loaded_theme;

    std::vector<std::string> data;
    bool non_theme_loaded = false;
public:
    /*
     * Get the current theme loaded.
     */
    ekg_theme &get_loaded_theme();

    /*
     * Init with the default theme.
     */
    void init();

    /*
     * Load a .json theme file.
     */
    void load(const char* path_to_theme_file_configuration);

    /*
     * Reset to the default theme colors.
     */
    void reset_theme();
};

#endif