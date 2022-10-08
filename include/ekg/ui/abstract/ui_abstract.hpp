#ifndef EKG_UI_ABSTRACT_H
#define EKG_UI_ABSTRACT_H

#include <iostream>
#include "ekg/core/feature.hpp"

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

            std::string tag {};
            std::string extra_tag {};

            bool alive {true};
            ekg::state state {};
            ekg::type type {ekg::type::abstract};
        public:
            abstract();
            ~abstract();

            void set_id(uint32_t token);
            uint32_t get_id();

            void set_parent_id(uint32_t token);
            uint32_t get_parent_id();

            void set_tag(std::string_view str);
            std::string get_tag();

            void set_extra_tag(std::string_view str);
            std::string get_extra_tag();

            void set_alive(bool flag_alive);
            bool is_alive();

            void set_state(const ekg::state &enum_state);
            ekg::state get_state();

            void set_type(const ekg::type &enum_type);
            ekg::type get_type();
        };
    }
}

#endif