/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, free to use in anything (utility, hacking, cheat, game, software).
 * 5- Malware, rat and others virus. We do not care.
 * 6- Do not modify this license under any instance.
 *
 * @VokeGpu 2022 all rights reserved.
 */

#ifndef EKG_H
#define EKG_H

#include "ekg/core/runtime.hpp"
#include "ekg/util/util_event.hpp"

#include "ekg/ui/frame/ui_frame.hpp"
#include "ekg/ui/label/ui_label.hpp"
#include "ekg/ui/button/ui_button.hpp"
#include "ekg/ui/checkbox/ui_checkbox.hpp"

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
    ekg::ui::checkbox *checkbox(std::string_view text, uint16_t = ekg::dock::left | ekg::dock::top);
    void popgroup();
}

#endif