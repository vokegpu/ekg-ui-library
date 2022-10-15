#ifndef EKG_UI_ABSTRACT_H
#define EKG_UI_ABSTRACT_H

#include "ekg/core/feature.hpp"
#include "ekg/util/geometry.hpp"
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
            uint32_t id {};
            uint32_t parent_id {};
            std::vector<uint32_t> parent_id_list {};

            bool alive {true};
            uint16_t dock {};

            ekg::state state {};
            ekg::type type {ekg::type::abstract};
            ekg::rect rect_widget {};
            ekg::rect sync_ui {};
            bool sync_with_ui {};
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

            void set_sync_with_ui(bool state);
            bool should_sync_with_ui();

            uint16_t get_dock();
            ekg::rect &widget();
            ekg::rect &ui();
        };
    }
}

#endif