#ifndef EKG_SERVICE_INPUT_H
#define EKG_SERVICE_INPUT_H

#include "ekg/util/env.hpp"
#include "ekg/util/geometry.hpp"
#include <map>
#include <vector>
#include <SDL2/SDL.h>

namespace ekg::service {
        class input {
        protected:
            std::map<std::string, std::vector<std::string>> map_bind {};
            std::map<std::string, bool> map_register {};
            std::map<SDL_Keycode, std::string> special_key {};

            std::vector<std::string> special_key_unit_pressed {};
            std::vector<std::string> special_key_released {};

            bool pressed {};
            bool released {};
            bool motion {};
            bool wheel {};

            ekg::vec4 last_finger_interact {};

            bool finger_hold_event {};
            bool finger_wheel_event {};

            void complete_with_units(std::string &string_builder, const std::string &key_name);
            bool contains_unit(const std::string &label);
        public:
            ekg::timing timing_last_interact {};
            ekg::vec4 interact {};

            bool was_pressed();
            bool was_released();
            bool was_motion();
            bool was_wheel();

            void registry(const std::string &input_tag);
            void bind(const std::string &input_tag, const std::string &key);
            void unbind(const std::string &input_tag, const std::string &key);
            void callback(const std::string &key, bool callback);

            void set(const std::string &input_tag, bool callback);
            bool get(const std::string &input_tag);

            void on_event(SDL_Event &sdl_event);
            void on_update();
        };
    }

#endif