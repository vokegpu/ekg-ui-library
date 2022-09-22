#include "ekg/core/runtime.hpp"
#include "ekg/cpu/cpu_input.hpp"

void ekg::runtime::set_root(SDL_Window *sdl_win_root) {
    this->root = sdl_win_root;
}

SDL_Window* ekg::runtime::get_root() {
    return this->root;
}

void ekg::runtime::init() {

}

void ekg::runtime::quit() {

}

void ekg::runtime::process_event(SDL_Event &sdl_event) {

}

void ekg::runtime::process_update() {

}

void ekg::runtime::process_render() {

}
