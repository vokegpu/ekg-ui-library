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
 * @VokeGpu 2023 all rights reserved.
 */

#ifndef EKG_UTIL_EVENT_H
#define EKG_UTIL_EVENT_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include "ekg/cpu/event.hpp"
#include <map>

namespace ekg {
    enum class env {
        refresh, swap, reload, synclayout, gc, redraw
    };

    void dispatch(ekg::cpu::event *p_ekg_event);
    void dispatch(ekg::env env);
    bool listen(ekg::cpu::uievent &ui_event, SDL_Event &sdl_event);

    extern ekg::rect empty;

    struct hovered {
        static int32_t id;
        static ekg::type type;
        static int32_t up;
        static int32_t down;
        static ekg::type downtype;
        static ekg::type uptype;
    };

    struct stack {
        std::map<int32_t, bool> registry {};
        std::vector<ekg::ui::abstract_widget*> ordered_list {};
        void clear();
    };

    void reload(int32_t id);
    void reload(ekg::ui::abstract_widget *p_widget);

    void synclayout(int32_t id);
    void synclayout(ekg::ui::abstract_widget *p_widgets);

    void refresh(int32_t id);
    void refresh(ekg::ui::abstract_widget *p_widget);

    void update_high_frequency(int32_t id);
    void update_high_frequency(ekg::ui::abstract_widget *p_widget);

    void push_back_stack(ekg::ui::abstract_widget *p_widget, ekg::stack &stack);
    ekg::ui::abstract_widget *find_absolute_parent_master(ekg::ui::abstract_widget *p_widget);

    int32_t &set(int32_t &value, int32_t result);
    bool &set(bool &value, bool result);
    std::string &set(std::string &value, std::string_view result);

    /*
     * Instead of dispatching lot of useless events like layout processing,
     * the assert is able to check and prevent that.
     */
    extern bool assert_task_flag;
    float assert_task(float, float);
}

#endif