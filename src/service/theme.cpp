#include "ekg/service/theme.hpp"
#include "ekg/util/env.hpp"
#include <filesystem>

void ekg::service::theme::init() {
    ekg::log("generating default theme...");

    this->gen_default_theme();
    this->refresh_theme_list();
}

void ekg::service::theme::quit() {

}

void ekg::service::theme::gen_default_theme() {
    this->frame_background = {255, 255, 255, 255};
    this->frame_border = {0, 0, 190, 100};
    this->frame_outline = {0, 0, 0, 0};
    this->frame_activy_offset = 18;

    this->button_string = {0, 0, 0, 255};
    this->button_background = {255, 255, 255, 255};
    this->button_activy = {0, 0, 190, 100};
    this->button_outline = {0, 0, 0, 0};
}

void ekg::service::theme::refresh_theme_list() {
    ekg::log("analysing for theme files...");

    this->loaded_theme_list.clear();
    std::string file_extension {};

    for (auto const &files : std::filesystem::directory_iterator {{"./"}}) {
        file_extension = files.path().filename().string();

        if (file_extension.length() < 4) {
            continue;
        }

        file_extension = file_extension.substr(file_extension.size() - 4, file_extension.size());

        if (file_extension != ".ekg") {
            continue;
        }

        file_extension = files.path().filename().string();
        this->loaded_theme_list.push_back(file_extension);

        ekg::log("found theme: " + file_extension);
    }
}

void ekg::service::theme::load_theme(const std::string &theme) {
    bool keep_load_theme {};

    for (std::string &themes : this->loaded_theme_list) {
        keep_load_theme = themes == theme;

        if (keep_load_theme) {
            break;
        }
    }

    if (keep_load_theme) {
        // todo add theme load.
    }
}
