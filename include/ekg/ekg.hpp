#ifndef EKG_H
#define EKG_H

#include "ekg/core/runtime.hpp"
#include "ekg/util/thread.hpp"

#include "ekg/ui/frame/ui_frame.hpp"

namespace ekg {
    extern ekg::runtime* core;
    extern std::string gl_version;

    ekg::service::theme &theme();
    ekg::service::input &input();

    ekg::ui::frame* frame(std::string_view, const ekg::vec2 &initial_position, ekg::vec2 size = {75, 75});

    void depth(float depth_preset);
    void init(SDL_Window* root, const std::string &font_path);
    void quit();

    void event(SDL_Event &sdl_event);
    void update();
    void render();
}

#endif