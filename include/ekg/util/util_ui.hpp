/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, use in any thing (hacking, cheating, games, softwares).
 * 5- All malwares, rat and others virus. We do not care.
 * 6- Do not modify this license under any circunstancie.
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
        std::map<uint32_t, bool> registry {};
        std::vector<ekg::ui::abstract_widget*> ordered_list {};
        void clear();
    };

    void reload(uint32_t id);
    void reload(ekg::ui::abstract_widget* widget);

    void reset(uint32_t id);
    void reset(ekg::ui::abstract_widget* widget);

    void sync_layout(uint32_t id);
    void sync_layout(ekg::ui::abstract_widget* widget);

    void push_back_stack(ekg::ui::abstract_widget* widget, ekg::stack &stack);
}

#endif