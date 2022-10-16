#ifndef EKG_H
#define EKG_H

#include "ekg/core/runtime.hpp"
#include "ekg/util/util_event.hpp"

#include "ekg/ui/frame/ui_frame.hpp"
#include "ekg/ui/label/ui_label.hpp"
#include "ekg/ui/button/ui_button.hpp"

namespace ekg {
    extern ekg::runtime* core;
    extern std::string gl_version;

    /* The setup and handling functions of ekg. */

    void init(SDL_Window*, const std::string&);
    void quit();

    void event(SDL_Event&);
    void update();
    void render();

    /* The environment getters of ekg. */

    ekg::service::theme &theme();
    ekg::draw::font_renderer &f_renderer(ekg::font);

    ekg::ui::frame* frame(std::string_view, const ekg::vec2&, ekg::vec2 = {75, 75});
    ekg::ui::button* button(std::string_view, uint16_t = ekg::dock::left | ekg::dock::top);
    ekg::ui::label* label(std::string_view, uint16_t = ekg::dock::left | ekg::dock::top);
    void pop_group();
}

#endif