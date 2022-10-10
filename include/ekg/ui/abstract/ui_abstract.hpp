#ifndef EKG_UI_ABSTRACT_H
#define EKG_UI_ABSTRACT_H

#include <iostream>
#include "ekg/core/feature.hpp"
#include <vector>

namespace ekg {
    enum class state {
        visible, invisible
    };

    enum class type {
        abstract, frame, button, slider, slider2d, checkbox, textbox, entrybox, combobox, tab, popup
    };

    namespace ui {
        class abstract : public ekg::feature {
        protected:
            uint32_t id {};
            uint32_t parent_id {};
            std::vector<uint32_t> parent_id_list {};

            bool alive {true};
            ekg::state state {};
            ekg::type type {ekg::type::abstract};
        public:
            abstract();
            ~abstract();

            void parent(uint32_t);
            std::vector<uint32_t> &get_parent_id_list();

            void set_id(uint32_t);
            uint32_t get_id();

            void set_parent_id(uint32_t);
            uint32_t get_parent_id();

            void set_alive(bool);
            bool is_alive();

            void set_state(const ekg::state&);
            ekg::state get_state();

            void set_type(const ekg::type&);
            ekg::type get_type();
        };
    }
}

#endif