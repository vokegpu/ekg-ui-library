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
    ekgmath::vec4f string_color;
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

    /* Start of frame colors. */
    ekgmath::vec4f frame_background;
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