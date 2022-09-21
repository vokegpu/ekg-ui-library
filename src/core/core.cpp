#include "ekg/core/core.hpp"
#include "ekg/cpu/cpu_input.hpp"

void ekg_core::set_root(SDL_Window *sdl_win_root) {
    this->root = sdl_win_root;
}

SDL_Window* ekg_core::get_root() {
    return this->root;
}

void ekg_core::init() {

}

void ekg_core::quit() {

}

void ekg_core::process_event(SDL_Event &sdl_event) {

}

void ekg_core::process_update() {

}

void ekg_core::process_render() {

}
