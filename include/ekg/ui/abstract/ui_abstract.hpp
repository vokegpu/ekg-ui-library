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

#ifndef EKG_UI_ABSTRACT_H
#define EKG_UI_ABSTRACT_H

#include "ekg/core/feature.hpp"
#include "ekg/util/geometry.hpp"
#include "ekg/util/env.hpp"
#include <vector>

namespace ekg {
    enum class state {
        visible, invisible
    };

    enum class type {
        abstract, frame, button, label, slider, slider2d, checkbox, textbox, entrybox, combobox, tab, popup
    };

    namespace ui {
        class abstract : public ekg::feature {
        protected:
            int32_t id {};
            int32_t parent_id {};
            std::vector<int32_t> parent_id_list {};

            bool alive {true};
            uint16_t dock {};
            std::string tag {};

            ekg::state state {};
            ekg::type type {ekg::type::abstract};
            ekg::rect rect_widget {}, sync_ui {};
            bool sync_with_ui {};
        public:
            abstract();
            ~abstract();

            void set_tag(std::string_view);
            std::string_view get_tag();

            void parent(int32_t);
            std::vector<int32_t> &get_parent_id_list();

            void set_id(int32_t);
            int32_t get_id();

            void set_parent_id(int32_t);
            int32_t get_parent_id();

            void set_alive(bool);
            bool is_alive();

            void set_state(const ekg::state&);
            ekg::state get_state();

            void set_type(const ekg::type&);
            ekg::type get_type();

            void set_sync_with_ui(bool sync_state);
            bool should_sync_with_ui();

            uint16_t get_dock();
            ekg::rect &widget();
            ekg::rect &ui();
        };
    }
}

#endif