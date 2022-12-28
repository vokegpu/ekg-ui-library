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
            static std::map<std::string, const char*> special_keys_name_map;

            std::map<std::string, std::vector<std::string>> input_bind_map {};
            std::map<std::string, bool> input_register_map {};
            std::map<int32_t, std::string> special_keys_sdl_map {};

            std::vector<std::string> special_keys_unit_pressed {};
            std::vector<std::string> special_keys_released {};
            std::vector<std::string> double_click_mouse_buttons_pressed {};

            bool pressed_event {}, released_event {}, motion_event {}, wheel_event {}, finger_hold_event {}, finger_wheel_event {};

            ekg::vec4 last_finger_interact {};
            ekg::timing double_interact {};

            void complete_with_units(std::string &string_builder, const std::string &key_name);
            bool contains_unit(const std::string &label);
            bool is_special_key(int32_t sdl_key_code);
        public:
            ekg::timing timing_last_interact {};
            ekg::vec4 interact {};

            bool was_pressed();
            bool was_released();
            bool was_motion();
            bool was_wheel();

            void bind(std::string_view input_tag, std::string_view key);
            void unbind(std::string_view input_tag, std::string_view key);
            void callback(std::string_view key, bool callback);
            bool pressed(std::string_view key);

            void on_event(SDL_Event &sdl_event);
            void on_update();
        };
    }

#endif