#include <ekg/api/ekg_theme.hpp>

void ekg_theme_service::init() {
    this->reset_theme();
}

void ekg_theme_service::load(const char *path_to_theme_file_configuration) {

}

void ekg_theme_service::reset_theme() {
    this->loaded_theme.validation = true;

    this->loaded_theme.string_color = ekgmath::vec4f(0, 0, 0, 255);
    this->loaded_theme.string_value_color = ekgmath::vec4f(190, 190, 190, 255);

    this->loaded_theme.button_background = ekgmath::vec4f(255, 255, 255, 255);
    this->loaded_theme.button_highlight = ekgmath::vec4f(190, 190, 190, 150);
    this->loaded_theme.button_activy = ekgmath::vec4f(0, 0, 190, 100);

    this->loaded_theme.check_box_background = ekgmath::vec4f(255, 255, 255, 255);
    this->loaded_theme.check_box_highlight = ekgmath::vec4f(190, 190, 190, 150);
    this->loaded_theme.check_box_activy = ekgmath::vec4f(0, 0, 190, 100);

    this->loaded_theme.frame_background = ekgmath::vec4f(255, 255, 255, 255);
}

ekg_theme &ekg_theme_service::get_loaded_theme() {
    return this->loaded_theme;
}
