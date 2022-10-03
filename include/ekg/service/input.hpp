#ifndef EKG_SERVICE_INPUT_H
#define EKG_SERVICE_INPUT_H

#include "ekg/util/env.hpp"
#include "ekg/util/geometry.hpp"
#include <map>
#include <vector>
#include <SDL2/SDL.h>

namespace ekg {
    struct task {
        std::vector<std::string> call_bounds {};
        bool state {};
    };

    namespace service {
        class input {
        protected:
            std::map<std::string, std::string> map_call_action {};
            std::map<std::string, ekg::task> map_action_register {};

            bool pressed {};
            bool released {};
            bool motion {};
            bool wheel {};

            ekg::vec4 last_finger_interact {};

            bool finger_hold_event {};
            bool finger_wheel_event {};
        public:
            ekg::timing timing_last_interact {};
            ekg::vec4 interact {};

            void init();

            bool was_pressed();
            bool was_released();
            bool was_motion();
            bool was_wheel();

            void registry(const std::string &action_tag);
            void add_bind(const std::string &action_tag, const std::string &action_key);
            void remove_bind(const std::string &action_tag, const std::string &action_key);

            void set_state(const std::string &action_tag, bool action_state);
            bool get_state(const std::string &action_tag);

            void on_event(SDL_Event &sdl_event);
            void on_update();
        };
    }
}

#endif