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

#include "ekg/util/io.hpp"
#include "ekg/util/geometry.hpp"
#include <unordered_map>
#include <vector>
#include "ekg/os/ekg_sdl.hpp"

namespace ekg::service {
        class input {
        protected:
            static std::unordered_map<std::string, const char*> special_keys_name_map;

            std::unordered_map<std::string, std::vector<std::string>> input_bind_map {};
            std::unordered_map<std::string, bool> input_register_map {};
            std::unordered_map<std::string, bool> input_map {};
            std::unordered_map<int32_t, std::string> special_keys_sdl_map {};

            std::vector<std::string> special_keys_unit_pressed {};
            std::vector<std::string> double_click_mouse_buttons_pressed {};
            std::vector<std::string> input_register_callback {};
            std::vector<std::string> input_released_list {};
            std::vector<std::string> immediate_register_list {};

            bool finger_hold_event {};
            bool finger_swipe_event {};
            bool is_special_keys_released {};

            ekg::vec4 last_finger_interact {};
            ekg::timing double_interact {};
        protected:
            void complete_with_units(std::string &string_builder, std::string_view key_name);
            bool contains_unit(std::string_view label);
            bool is_special_key(int32_t sdl_key_code);
        public:
            ekg::timing timing_last_interact {};
            ekg::vec4 interact {};

            bool was_pressed {};
            bool was_released {};
            bool has_motion {};
            bool was_wheel {};
            bool was_typed {};
        public:
            void bind(std::string_view input_tag, std::string_view key);
            void unbind(std::string_view input_tag, std::string_view key);
            void callback(std::string_view key, bool callback);
            void fire(std::string_view key);

            bool pressed(std::string_view key);
            bool receive(std::string_view key);

            void on_event(SDL_Event &sdl_event);
            void on_update();
        };
    }

#endif