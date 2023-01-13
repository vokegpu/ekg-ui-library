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

std::map<std::string, const char*> ekg::service::input::special_keys_name_map = {
        {"Left Shift", "lshift"}, {"Right Shift", "rshift"},
        {"Left Ctrl", "lctrl"},   {"Right Ctrl", "rctrl"},
        {"Left Alt", "lalt"},     {"Right Alt", "ralt"},
        {"Tab", "tab"}
};

void ekg::service::input::on_event(SDL_Event &sdl_event) {
    this->pressed_event = false;
    this->released_event = false;
    this->motion_event = false;

    switch (sdl_event.type) {
        case SDL_KEYDOWN: {
            this->pressed_event = true;

            for (std::string key_name = SDL_GetKeyName(sdl_event.key.keysym.sym); !key_name.empty(); key_name) {
                if (this->is_special_key(sdl_event.key.keysym.sym)) {
                    this->special_keys_sdl_map[sdl_event.key.keysym.sym] = ekg::service::input::special_keys_name_map[key_name];
                    this->special_keys_sdl_map[sdl_event.key.keysym.sym] += "+";
                } else {
                    std::string string_builder {};
                    std::transform(key_name.begin(), key_name.end(), key_name.begin(), ::tolower);

                    this->complete_with_units(string_builder, key_name);
                    this->callback(string_builder, true);

                    if (string_builder != key_name && !this->contains_unit(string_builder)) {
                        this->special_keys_unit_pressed.push_back(string_builder);
                    }
                }

                break;
            }

            break;
        }

        case SDL_KEYUP: {
            this->pressed_event = true;

            for (std::string key_name = SDL_GetKeyName(sdl_event.key.keysym.sym); !key_name.empty(); key_name) {
                if (this->is_special_key(sdl_event.key.keysym.sym)) {
                    this->special_keys_sdl_map[sdl_event.key.keysym.sym] = "";
                    this->special_keys_released.emplace_back(ekg::service::input::special_keys_name_map[key_name]);
                } else {
                    std::string string_builder{};
                    std::transform(key_name.begin(), key_name.end(), key_name.begin(), ::tolower);

                    this->complete_with_units(string_builder, key_name);
                    this->callback(string_builder, false);
                }

                break;
            }

            break;
        }

        case SDL_MOUSEBUTTONDOWN: {
            this->pressed_event = true;
            bool double_click_factor {ekg::reach(this->double_interact, 500)};

            switch (sdl_event.button.button) {
                case SDL_BUTTON_LEFT: {
                    this->callback("mouse-left", true);

                    if (!double_click_factor) {
                        const std::string input_tag = "mouse-left-double";
                        this->callback(input_tag, true);
                        this->double_click_mouse_buttons_pressed.push_back(input_tag);
                    }
                    break;
                }

                case SDL_BUTTON_RIGHT: {
                    this->callback("mouse-right", true);

                    if (!double_click_factor) {
                        const std::string input_tag = "mouse-right-double";
                        this->callback(input_tag, true);
                        this->double_click_mouse_buttons_pressed.push_back(input_tag);
                    }

                    break;
                }

                case SDL_BUTTON_MIDDLE: {
                    this->callback("mouse-middle", true);

                    if (!double_click_factor) {
                        const std::string input_tag = "mouse-middle-double";
                        this->callback(input_tag, true);
                        this->double_click_mouse_buttons_pressed.push_back(input_tag);
                    }

                    break;
                }

                default: {
                    this->callback("mouse-" + std::to_string(sdl_event.button.button), true);

                    if (!double_click_factor) {
                        const std::string input_tag = "mouse-" + std::to_string(sdl_event.button.button) + "-double";
                        this->callback(input_tag, true);
                        this->double_click_mouse_buttons_pressed.push_back(input_tag);
                    }

                    break;
                }
            }

            if (double_click_factor) {
                ekg::reset(this->double_interact);
            }
            break;
        }

        case SDL_MOUSEBUTTONUP: {
            this->released_event = true;
            
            switch (sdl_event.button.button) {
                case SDL_BUTTON_LEFT: {
                    this->callback("mouse-left", false);
                    this->callback("mouse-left-double", false);
                    break;
                }

                case SDL_BUTTON_RIGHT: {
                    this->callback("mouse-right", false);
                    this->callback("mouse-right-double", false);
                    break;
                }

                case SDL_BUTTON_MIDDLE: {
                    this->callback("mouse-middle", false);
                    this->callback("mouse-middle-double", false);
                    break;
                }

                default: {
                    this->callback("mouse-" + std::to_string(sdl_event.button.button), false);
                    this->callback("mouse-" + std::to_string(sdl_event.button.button) + "-double", false);
                    break;
                }
            }
        }

        case SDL_MOUSEMOTION: {
            this->motion_event = true;
            this->interact.x = static_cast<float>(sdl_event.motion.x);
            this->interact.y = static_cast<float>(sdl_event.motion.y);
            break;
        }

        case SDL_MOUSEWHEEL: {
            this->wheel_event = true;
            this->interact.z = sdl_event.wheel.preciseX;
            this->interact.w = sdl_event.wheel.preciseY;

            this->callback("mouse-wheel-up", this->interact.w > 0);
            this->callback("mouse-wheel-down", this->interact.w < 0);
            break;
        }

        case SDL_FINGERDOWN: {
            this->pressed_event = true;
            ekg::reset(this->timing_last_interact);
            bool reach_double_interact {ekg::reach(this->double_interact, 500)};

            this->last_finger_interact.x = sdl_event.tfinger.x;
            this->last_finger_interact.y = sdl_event.tfinger.y;

            this->callback("finger-click", true);
            this->callback("finger-click-double", !reach_double_interact);

            if (reach_double_interact) {
                ekg::reset(this->double_interact);
            }

            break;
        }

        case SDL_FINGERUP: {
            this->released_event = true;
            this->callback("finger-hold", (this->finger_hold_event = ekg::reach(this->timing_last_interact, 750)));
            this->callback("finger-click", false);
            this->callback("finger-click-double", false);
            break;
        }

        case SDL_FINGERMOTION: {
            this->motion_event = true;
            this->interact.x = sdl_event.tfinger.x;
            this->interact.y = sdl_event.tfinger.y;

            this->interact.z = this->interact.x - this->interact.z;
            this->interact.w = this->interact.y - this->interact.w;

            this->finger_wheel_event = true;
            ekg::reset(this->timing_last_interact);
            break;
        }
    }

    if (this->motion_event && !this->double_click_mouse_buttons_pressed.empty()) {
        for (const std::string &button : this->double_click_mouse_buttons_pressed) {
            this->callback(button, false);
        }

        this->double_click_mouse_buttons_pressed.clear();
    }
}

bool ekg::service::input::was_pressed() {
    return this->pressed_event;
}

bool ekg::service::input::was_released() {
    return this->released_event;
}

bool ekg::service::input::was_motion() {
    return this->motion_event;
}

bool ekg::service::input::was_wheel() {
    return this->wheel_event;
}

void ekg::service::input::on_update() {
    if (this->wheel_event) {
        this->callback("mouse-wheel-up", false);
        this->callback("mouse-wheel-down", false);
        this->wheel_event = false;
    }

    if (this->finger_wheel_event) {
        this->interact.z = 0.0f;
        this->interact.w = 0.0f;
        this->finger_wheel_event = false;
    }

    if (this->finger_hold_event) {
        this->finger_hold_event = false;
    }

    if (!this->special_keys_released.empty()) {
        for (std::string &units : this->special_keys_unit_pressed) {
            this->callback(units, false);
        }

        this->special_keys_unit_pressed.clear();
        this->special_keys_released.clear();
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
        bind_list.push_back(input_tag.data());
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
        bind_list.erase(bind_list.cbegin() + (uint32_t) it);
    }
}

void ekg::service::input::callback(std::string_view key, bool callback) {
    for (std::string &binds : this->input_bind_map[key.data()]) {
        this->input_register_map[binds] = callback;
    }
}

void ekg::service::input::complete_with_units(std::string &string_builder, const std::string &key_name) {
    string_builder += this->special_keys_sdl_map[SDLK_LCTRL];
    string_builder += this->special_keys_sdl_map[SDLK_RCTRL];
    string_builder += this->special_keys_sdl_map[SDLK_LSHIFT];
    string_builder += this->special_keys_sdl_map[SDLK_RSHIFT];
    string_builder += this->special_keys_sdl_map[SDLK_LALT];
    string_builder += this->special_keys_sdl_map[SDLK_RALT];
    string_builder += this->special_keys_sdl_map[SDLK_TAB];
    string_builder += key_name;
}

bool ekg::service::input::contains_unit(const std::string &label) {
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
