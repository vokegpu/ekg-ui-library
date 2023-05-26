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
#include "ekg/util/env.hpp"

#include "ekg/ui/frame/ui_frame.hpp"
#include "ekg/ui/label/ui_label.hpp"
#include "ekg/ui/button/ui_button.hpp"
#include "ekg/ui/checkbox/ui_checkbox.hpp"
#include "ekg/ui/slider/ui_slider.hpp"
#include "ekg/ui/popup/ui_popup.hpp"
#include "ekg/ui/textbox/ui_textbox.hpp"
#include "ekg/ui/scroll/ui_scroll.hpp"

namespace ekg {
    /*
     * The runtime core of ekg.
     */
    extern ekg::runtime *core;

    /*
     * Shaders version should be higher than 300.
     */
    extern std::string gl_version;

    /* The setup and handling functions of ekg. */

    /*
     * Init the runtime core of ekg, set window SDL instance and font renderer path (default but changeable).
     */
    void init(SDL_Window*, std::string_view);

    /*
     * Quit from all services and main runtime core.
     */
    void quit();

    /*
     * Poll events and sync UI events.
     */
    void event(SDL_Event&);

    /*
     * Process events, tasks, services & widgets.
     */
    void update();

    /*
     * Draw all data from gpu allocator.
     */
    void render();

    /* The environment getters of ekg. */

    /*
     * Get the theme service, you can load custom theme files.
     */
    ekg::service::theme &theme();

    /*
     * Get font renderer based on scaled sizes.
     */
    ekg::draw::font_renderer &f_renderer(ekg::font);

    /*
     * Create frame UI.
     */
    ekg::ui::frame *frame(std::string_view, const ekg::vec2&, ekg::vec2 = {75, 75});

    /*
     * Create button UI.
     */
    ekg::ui::button *button(std::string_view, uint16_t = ekg::dock::none);

    /*
     * Create label UI.
     */
    ekg::ui::label *label(std::string_view, uint16_t = ekg::dock::none);

    /*
     * Create checkbox UI.
     */
    ekg::ui::checkbox *checkbox(std::string_view, bool, uint16_t = ekg::dock::none);

    /*
     * Create slider UI.
     */
    ekg::ui::slider *slider(std::string_view, float, float, float, uint16_t = ekg::dock::none);

    /*
     * Create popup UI.
     */
    ekg::ui::popup *popup(std::string_view, const std::vector<std::string>&, bool = true);

    /*
     * Create textbox UI.
     */
    ekg::ui::textbox *textbox(std::string_view, std::string_view, uint16_t = ekg::dock::none);

    /*
     * Create scroll UI.
     */
    ekg::ui::scroll *scroll(std::string_view);

    /*
     * Stop collecting UIs to group.
     */
    void popgroup();
}

#endif