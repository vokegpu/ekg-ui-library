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

#ifndef EKG_UTIL_UI_H
#define EKG_UTIL_UI_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include <map>

namespace ekg {
    extern ekg::rect empty;

    struct stack {
        std::map<int32_t, bool> registry {};
        std::vector<ekg::ui::abstract_widget*> ordered_list {};
        void clear();
    };

    struct component {
        std::string name {};
        ekg::flag flag {};
        ekg::rect rect {};
        bool boolean {};

        void *data1 {nullptr};
        int32_t id {}, linked_id {};
    };

    void reload(int32_t);
    void reload(ekg::ui::abstract_widget*);

    void scissor(int32_t);
    void scissor(ekg::ui::abstract_widget*);

    void synclayout(int32_t id);
    void synclayout(ekg::ui::abstract_widget *pwidget);

    void push_back_stack(ekg::ui::abstract_widget*, ekg::stack&);
    ekg::ui::abstract_widget *find_absolute_parent_master(ekg::ui::abstract_widget*);
}

#endif