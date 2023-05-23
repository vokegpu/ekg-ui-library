#include "ekg/cpu/event.hpp"

uint32_t ekg::listener {};

void ekg::dispatch_ui_event(std::string_view tag, std::string_view value, uint16_t type) {
    SDL_Event sdl_event {};
    sdl_event.type = ekg::listener;
    sdl_event.user.type = ekg::listener;
    sdl_event.user.data1 = new ekg::cpu::uievent {tag.data(), value.data(), type};
    SDL_PushEvent(&sdl_event);
}