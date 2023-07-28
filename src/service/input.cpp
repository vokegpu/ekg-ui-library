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

#include "ekg/service/input.hpp"
#include <string>
#include <algorithm>

std::unordered_map<std::string, const char*> ekg::service::input::special_keys_name_map = {
        {"Left Shift", "lshift"}, {"Right Shift", "rshift"},
        {"Left Ctrl", "lctrl"},   {"Right Ctrl", "rctrl"},
        {"Left Alt", "alt"},     {"Right Alt", "altgr"},
        {"Tab", "tab"}
};

void ekg::service::input::on_event(SDL_Event &sdl_event) {
    this->was_pressed = false;
    this->was_released = false;
    this->has_motion = false;
    this->was_typed = false;

    switch (sdl_event.type) {
        case SDL_TEXTINPUT: {
            this->was_pressed = true;
            this->was_typed = true;
            break;
        }

        case SDL_KEYDOWN: {
            this->was_pressed = true;
            std::string key_name {SDL_GetKeyName(sdl_event.key.keysym.sym)};
            std::string string_builder {};

            if (this->is_special_key(sdl_event.key.keysym.sym)) {
                string_builder += ekg::service::input::special_keys_name_map[key_name];
                this->special_keys_sdl_map[sdl_event.key.keysym.sym] = string_builder;
                this->special_keys_sdl_map[sdl_event.key.keysym.sym] += "+";

                this->callback(string_builder, true);
                this->is_special_keys_released = true;
            } else {
                std::transform(key_name.begin(), key_name.end(), key_name.begin(), ::tolower);
                string_builder += "abs-";
                string_builder += key_name;

                this->callback(string_builder, true);
                this->input_released_list.push_back(string_builder);

                string_builder.clear();
                this->complete_with_units(string_builder, key_name);
                this->callback(string_builder, true);
                this->input_released_list.push_back(string_builder);

                if (string_builder != key_name && !this->contains_unit(string_builder)) {
                    this->special_keys_unit_pressed.push_back(string_builder);
                }
            }

            break;
        }

        case SDL_KEYUP: {
            this->was_released = true;
            std::string key_name {SDL_GetKeyName(sdl_event.key.keysym.sym)};
            std::string string_builder {};

            if (this->is_special_key(sdl_event.key.keysym.sym)) {
                this->special_keys_sdl_map[sdl_event.key.keysym.sym] = "";

                string_builder += ekg::service::input::special_keys_name_map[key_name];
                this->callback(string_builder, false);
                this->is_special_keys_released = true;

                string_builder += "-up";
                this->callback(string_builder, true);
            } else {
                std::transform(key_name.begin(), key_name.end(), key_name.begin(), ::tolower);
                string_builder += "abs-";
                string_builder += key_name;
                string_builder += "-up";

                this->callback(string_builder, true);
                this->input_released_list.push_back(string_builder);

                string_builder.clear();
                this->complete_with_units(string_builder, key_name);
                string_builder += "-up";

                this->callback(string_builder, true);
                this->input_released_list.push_back(string_builder);
            }

            break;
        }

        case SDL_MOUSEBUTTONDOWN: {
            std::string string_builder {"mouse-"};
            string_builder += std::to_string(sdl_event.button.button);

            this->was_pressed = true;
            this->callback(string_builder, true);
            this->input_released_list.push_back(string_builder);

            bool double_click_factor {ekg::reach(this->double_interact, 500)};
            if (!double_click_factor) {
                string_builder += "-double";
                this->callback(string_builder, true);

                this->double_click_mouse_buttons_pressed.push_back(string_builder);
                this->input_released_list.push_back(string_builder);
            }

            if (double_click_factor) {
                ekg::reset(this->double_interact);
            }

            break;
        }

        case SDL_MOUSEBUTTONUP: {
            this->was_released = true;
            std::string string_builder {"mouse-"};

            string_builder += std::to_string(sdl_event.button.button);
            string_builder += "-up";

            this->callback(string_builder, true);
            this->input_released_list.push_back(string_builder);
            break;
        }

        case SDL_MOUSEMOTION: {
            this->has_motion = true;
            this->interact.x = static_cast<float>(sdl_event.motion.x);
            this->interact.y = static_cast<float>(sdl_event.motion.y);
            break;
        }

        case SDL_MOUSEWHEEL: {
            this->was_wheel = true;
            this->interact.z = sdl_event.wheel.preciseX;
            this->interact.w = sdl_event.wheel.preciseY;

            this->callback("mouse-wheel", true);
            this->callback("mouse-wheel-up", this->interact.w > 0);
            this->callback("mouse-wheel-down", this->interact.w < 0);
            break;
        }

        case SDL_FINGERDOWN: {
            this->was_pressed = true;
            ekg::reset(this->timing_last_interact);
            bool reach_double_interact {ekg::reach(this->double_interact, 500)};

            this->interact.x = sdl_event.tfinger.x * static_cast<float>(ekg::display::width);
            this->interact.y = sdl_event.tfinger.y * static_cast<float>(ekg::display::height);

            this->last_finger_interact.x = this->interact.x;
            this->last_finger_interact.y = this->interact.y;

            this->callback("finger-click", true);
            this->callback("finger-click-double", !reach_double_interact);

            if (reach_double_interact) {
                ekg::reset(this->double_interact);
            }

            break;
        }

        case SDL_FINGERUP: {
            this->was_released = true;
            this->callback("finger-hold", (this->finger_hold_event = ekg::reach(this->timing_last_interact, 750)));
            this->callback("finger-click", false);
            this->callback("finger-click-double", false);

            /*
             * Should stop the swipe event when there is no finger touching on screen.
             */
            this->callback("finger-swipe", false);
            this->callback("finger-swipe-up", false);
            this->callback("finger-swipe-down", false);

            this->interact.x = sdl_event.tfinger.x * static_cast<float>(ekg::display::width);
            this->interact.y = sdl_event.tfinger.y * static_cast<float>(ekg::display::height);

            this->last_finger_interact.x = this->interact.x;
            this->last_finger_interact.y = this->interact.y;

            this->interact.z = 0.0f;
            this->interact.w = 0.0f;

            break;
        }

        case SDL_FINGERMOTION: {
            this->has_motion = true;
            this->interact.x = sdl_event.tfinger.x * static_cast<float>(ekg::display::width);
            this->interact.y = sdl_event.tfinger.y * static_cast<float>(ekg::display::height);

            this->interact.z = (sdl_event.tfinger.dx * (static_cast<float>(ekg::display::width) / 9.0f));
            this->interact.w = (sdl_event.tfinger.dy * static_cast<float>(ekg::display::height) / 9.0f);

            float swipe_factor = 0.01f;

            this->callback("finger-swipe", (this->interact.w > swipe_factor || this->interact.w < -swipe_factor) || (this->interact.z > swipe_factor || this->interact.z < -swipe_factor));
            this->callback("finger-swipe-up", this->interact.w > swipe_factor);
            this->callback("finger-swipe-down", this->interact.w < -swipe_factor);

            this->finger_swipe_event = true;
            ekg::reset(this->timing_last_interact);
            break;
        }
    }

    if (this->has_motion && !this->double_click_mouse_buttons_pressed.empty()) {
        for (const std::string &button : this->double_click_mouse_buttons_pressed) {
            this->callback(button, false);
        }

        this->double_click_mouse_buttons_pressed.clear();
    }
}

void ekg::service::input::on_update() {
    if (this->was_wheel) {
        this->callback("mouse-wheel", false);
        this->callback("mouse-wheel-up", false);
        this->callback("mouse-wheel-down", false);
        this->was_wheel = false;
    }

    if (this->finger_swipe_event) {
        this->callback("finger-swipe", false);
        this->callback("finger-swipe-up", false);
        this->callback("finger-swipe-down", false);
        this->finger_swipe_event = false;
    }

    this->finger_hold_event = false;

    if (this->is_special_keys_released) {
        for (std::string &units : this->special_keys_unit_pressed) {
            this->callback(units, false);
        }

        this->special_keys_unit_pressed.clear();
        this->is_special_keys_released = false;
    }

    if (!this->input_released_list.empty()) {
        for (std::string &inputs : this->input_released_list) {
            this->callback(inputs, false);
        }

        this->input_released_list.clear();
    }

    if (!this->immediate_register_list.empty()) {
        for (std::string &inputs : this->immediate_register_list) {
            this->input_register_map[inputs] = false;
        }

        this->immediate_register_list.clear();
    }
}

void ekg::service::input::bind(std::string_view input_tag, std::string_view key) {
    bool should_bind {true};
    auto &bind_list = this->input_bind_map[key.data()];

    for (std::string &inputs : bind_list) {
        if (inputs == input_tag) {
            should_bind = false;
            break;
        }
    }

    if (should_bind) {
        bind_list.emplace_back(input_tag.data());
    }
}

void ekg::service::input::unbind(std::string_view input_tag, std::string_view key) {
    std::size_t it {};
    bool should_unbind {};
    auto &bind_list = this->input_bind_map[key.data()];

    for (it = 0; it < bind_list.size(); it++) {
        if ((should_unbind = bind_list[it] == input_tag)) {
            break;
        }
    }

    if (should_unbind) {
        bind_list.erase(bind_list.cbegin() + (uint64_t) it);
    }
}

void ekg::service::input::callback(std::string_view key, bool callback) {
    auto &bind_map {this->input_bind_map[key.data()]};
    this->input_map[key.data()] = callback;

    if (!this->input_register_map.empty()) {
        this->input_register_callback.clear();
    }

    for (std::string &binds : bind_map) {
        this->input_register_map[binds] = callback;
        if (callback) {
            this->input_register_callback.push_back(binds);
        }
    }
}

void ekg::service::input::complete_with_units(std::string &string_builder, std::string_view key_name) {
    string_builder += this->special_keys_sdl_map[SDLK_LCTRL];
    string_builder += this->special_keys_sdl_map[SDLK_RCTRL];
    string_builder += this->special_keys_sdl_map[SDLK_LSHIFT];
    string_builder += this->special_keys_sdl_map[SDLK_RSHIFT];
    string_builder += this->special_keys_sdl_map[SDLK_LALT];
    string_builder += this->special_keys_sdl_map[SDLK_RALT];
    string_builder += this->special_keys_sdl_map[SDLK_TAB];
    string_builder += key_name;
}

void ekg::service::input::fire(std::string_view key) {
    this->input_register_map[key.data()] = key;
    this->immediate_register_list.emplace_back(key);
}

bool ekg::service::input::contains_unit(std::string_view label) {
    for (std::string &units : this->special_keys_unit_pressed) {
        if (units == label) {
            return true;
        }
    }

    return false;
}

bool ekg::service::input::pressed(std::string_view key) {
    return this->input_register_map[key.data()];
}

bool ekg::service::input::is_special_key(int32_t sdl_key_code) {
    return sdl_key_code == SDLK_LCTRL || sdl_key_code == SDLK_RCTRL || sdl_key_code == SDLK_LSHIFT || sdl_key_code == SDLK_RSHIFT || sdl_key_code == SDLK_LALT || sdl_key_code == SDLK_RALT || sdl_key_code == SDLK_TAB;
}

bool ekg::service::input::receive(std::string_view key) {
    return this->input_map[key.data()];
}