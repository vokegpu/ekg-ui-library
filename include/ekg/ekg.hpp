#ifndef EKG_H
#define EKG_H

#include "ekg/core/runtime.hpp"
#include "ekg/util/thread.hpp"

#include "ekg/ui/frame/ui_frame.hpp"
#include "ekg/ui/button/ui_button.hpp"

namespace ekg {
    extern ekg::runtime* core;
    extern std::string gl_version;

    ekg::service::theme &theme();
    ekg::service::input &input();

    ekg::ui::frame* frame(std::string_view, const ekg::vec2&, ekg::vec2 = {75, 75});
    ekg::ui::button* button(std::string_view);

    void depth(float);
    void init(SDL_Window*, const std::string&);
    void quit();

    void event(SDL_Event&);
    void update();
    void render();
}

#endif