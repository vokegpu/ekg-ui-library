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
#include "ekg/api/ekg_theme.hpp"

void ekg_theme_service::init() {
    this->reset_theme();
}

void ekg_theme_service::load(const char *path_to_theme_file_configuration) {

}

void ekg_theme_service::reset_theme() {
    this->loaded_theme.validation = true;

    this->loaded_theme.string_enabled_color = ekgmath::vec4f(0, 0, 0, 255);
    this->loaded_theme.string_disabled_color = ekgmath::vec4f(0, 0, 0, 150);
    this->loaded_theme.string_value_color = ekgmath::vec4f(190, 190, 190, 255);

    this->loaded_theme.button_background = ekgmath::vec4f(255, 255, 255, 255);
    this->loaded_theme.button_highlight = ekgmath::vec4f(190, 190, 190, 150);
    this->loaded_theme.button_activy = ekgmath::vec4f(0, 0, 190, 100);

    this->loaded_theme.check_box_background = ekgmath::vec4f(255, 255, 255, 255);
    this->loaded_theme.check_box_highlight = ekgmath::vec4f(190, 190, 190, 150);
    this->loaded_theme.check_box_activy = ekgmath::vec4f(0, 0, 190, 100);

    this->loaded_theme.slider_background = ekgmath::vec4f(255, 255, 255, 255);
    this->loaded_theme.slider_highlight = ekgmath::vec4f(190, 190, 190, 150);
    this->loaded_theme.slider_activy = ekgmath::vec4f(0, 0, 190, 100);

    this->loaded_theme.combobox_background = ekgmath::vec4f(255, 255, 255, 255);
    this->loaded_theme.combobox_highlight = ekgmath::vec4f(190, 190, 190, 150);
    this->loaded_theme.combobox_activy = ekgmath::vec4f(0, 0, 190, 100);
    this->loaded_theme.combobox_border = ekgmath::vec4f(190, 190, 190, 255);

    this->loaded_theme.popup_background = ekgmath::vec4f(255, 255, 255, 255);
    this->loaded_theme.popup_highlight = ekgmath::vec4f(0, 0, 190, 100);
    this->loaded_theme.popup_border = ekgmath::vec4f(190, 190, 190, 255);

    this->loaded_theme.immediate_popup_background = ekgmath::vec4f(255, 255, 255, 255);
    this->loaded_theme.immediate_popup_border = ekgmath::vec4f(190, 190, 190, 255);

    this->loaded_theme.frame_background = ekgmath::vec4f(255, 255, 255, 255);
    this->loaded_theme.frame_border = ekgmath::vec4f(190, 190, 190, 255);
}

ekg_theme &ekg_theme_service::get_loaded_theme() {
    return this->loaded_theme;
}
