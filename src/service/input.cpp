#include "ekg/service/input.hpp"
#include <string>
#include <algorithm>

void ekg::service::input::on_event(SDL_Event &sdl_event) {
    this->pressed = sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_MOUSEBUTTONDOWN || sdl_event.type == SDL_FINGERDOWN;
    this->released = sdl_event.type == SDL_KEYUP || sdl_event.type == SDL_MOUSEBUTTONUP || sdl_event.type == SDL_FINGERUP;
    this->motion = sdl_event.type == SDL_MOUSEMOTION || sdl_event.type == SDL_FINGERMOTION;

    switch (sdl_event.type) {
        case SDL_KEYDOWN: {
            switch (sdl_event.key.keysym.sym) {
                case SDLK_RCTRL: {
                    this->special_key[sdl_event.key.keysym.sym] = "rctrl+";
                    break;
                }

                case SDLK_LCTRL: {
                    this->special_key[sdl_event.key.keysym.sym] = "lctrl+";
                    break;
                }

                case SDLK_RSHIFT: {
                    this->special_key[sdl_event.key.keysym.sym] = "rshift+";
                    break;
                }

                case SDLK_LSHIFT: {
                    this->special_key[sdl_event.key.keysym.sym] = "lshift+";
                    break;
                }

                case SDLK_LALT: {
                    this->special_key[sdl_event.key.keysym.sym] = "lalt+";
                    break;
                }

                case SDLK_RALT: {
                    this->special_key[sdl_event.key.keysym.sym] = "ralt+";
                    break;
                }

                case SDLK_TAB: {
                    this->special_key[sdl_event.key.keysym.sym] = "tab+";
                    break;
                }

                default: {
                    for (std::string key_name = SDL_GetKeyName(sdl_event.key.keysym.sym); !key_name.empty(); key_name) {
                        std::string string_builder {};
                        std::transform(key_name.begin(), key_name.end(), key_name.begin(), ::tolower);

                        this->complete_with_units(string_builder, key_name);
                        this->callback(string_builder, true);

                        if (string_builder != key_name && !this->contains_unit(string_builder)) {
                            this->special_key_unit_pressed.push_back(string_builder);
                        }

                        break;
                    }

                    break;
                }
            }

            break;
        }

        case SDL_KEYUP: {
            switch (sdl_event.key.keysym.sym) {
                case SDLK_RCTRL: {
                    this->special_key[sdl_event.key.keysym.sym] = "";
                    this->special_key_released.emplace_back("rctrl");
                    break;
                }

                case SDLK_LCTRL: {
                    this->special_key[sdl_event.key.keysym.sym] = "";
                    this->special_key_released.emplace_back("lctrl");
                    break;
                }

                case SDLK_RSHIFT: {
                    this->special_key[sdl_event.key.keysym.sym] = "";
                    this->special_key_released.emplace_back("rshift");
                    break;
                }

                case SDLK_LSHIFT: {
                    this->special_key[sdl_event.key.keysym.sym] = "";
                    this->special_key_released.emplace_back("lshift");
                    break;
                }

                case SDLK_LALT: {
                    this->special_key[sdl_event.key.keysym.sym] = "";
                    this->special_key_released.emplace_back("lalt");
                    break;
                }

                case SDLK_RALT: {
                    this->special_key[sdl_event.key.keysym.sym] = "";
                    this->special_key_released.emplace_back("ralt");
                    break;
                }

                case SDLK_TAB: {
                    this->special_key[sdl_event.key.keysym.sym] = "";
                    this->special_key_released.emplace_back("tab");
                    break;
                }

                default: {
                    for (std::string key_name = SDL_GetKeyName(sdl_event.key.keysym.sym); !key_name.empty(); key_name) {
                        std::string string_builder {};
                        std::transform(key_name.begin(), key_name.end(), key_name.begin(), ::tolower);

                        this->complete_with_units(string_builder, key_name);
                        this->callback(string_builder, false);
                        break;
                    }

                    break;
                }
            }

            break;
        }

        case SDL_MOUSEBUTTONDOWN: {
            switch (sdl_event.button.button) {
                case SDL_BUTTON_LEFT: {
                    this->callback("mouse-left", true);
                    break;
                }

                case SDL_BUTTON_RIGHT: {
                    this->callback("mouse-right", true);
                    break;
                }

                case SDL_BUTTON_MIDDLE: {
                    this->callback("mouse-middle", true);
                    break;
                }

                default: {
                    this->callback("mouse-" + std::to_string(sdl_event.button.button), true);
                    break;
                }
            }

            break;
        }

        case SDL_MOUSEBUTTONUP: {
            switch (sdl_event.button.button) {
                case SDL_BUTTON_LEFT: {
                    this->callback("mouse-left", false);
                    break;
                }

                case SDL_BUTTON_RIGHT: {
                    this->callback("mouse-right", false);
                    break;
                }

                case SDL_BUTTON_MIDDLE: {
                    this->callback("mouse-middle", false);
                    break;
                }

                default: {
                    this->callback("mouse-" + std::to_string(sdl_event.button.button), false);
                    break;
                }
            }
        }

        case SDL_MOUSEWHEEL: {
            this->interact.z = static_cast<float>(sdl_event.wheel.preciseX);
            this->interact.w = static_cast<float>(sdl_event.wheel.preciseY);
            break;
        }

        case SDL_FINGERDOWN: {
            ekg::reset(this->timing_last_interact);

            this->callback("finger-click", true);
            this->last_finger_interact.x = sdl_event.tfinger.x;
            this->last_finger_interact.y = sdl_event.tfinger.y;

            break;
        }

        case SDL_FINGERUP: {
            this->callback("finger-hold", (this->finger_hold_event = ekg::reach(this->timing_last_interact, 750)));
            this->callback("finger-click", false);

            break;
        }

        case SDL_FINGERMOTION: {
            this->interact.x = sdl_event.tfinger.x;
            this->interact.y = sdl_event.tfinger.y;

            this->interact.z = this->interact.x - this->interact.z;
            this->interact.w = this->interact.y - this->interact.w;

            this->finger_wheel_event = true;
            ekg::reset(this->timing_last_interact);

            break;
        }
    }
}

bool ekg::service::input::was_pressed() {
    return this->pressed;
}

bool ekg::service::input::was_released() {
    return this->released;
}

bool ekg::service::input::was_motion() {
    return this->motion;
}

bool ekg::service::input::was_wheel() {
    return this->wheel;
}

void ekg::service::input::on_update() {
    if (this->finger_wheel_event) {
        this->interact.z = 0.0f;
        this->interact.w = 0.0f;
        this->finger_wheel_event = false;
    }

    if (this->finger_hold_event) {
        this->finger_hold_event = false;
    }

    if (!this->special_key_released.empty()) {
        for (std::string &units : this->special_key_unit_pressed) {
            this->callback(units, false);
        }

        this->special_key_unit_pressed.clear();
        this->special_key_released.clear();
    }
}

void ekg::service::input::registry(const std::string &input_tag) {
    this->map_register[input_tag] = false;
}

void ekg::service::input::bind(const std::string &input_tag, const std::string &key) {
    bool should_bind {true};
    auto &bind_list = this->map_bind[key];

    for (std::string &inputs : bind_list) {
        if (inputs == input_tag) {
            should_bind = false;
            break;
        }
    }

    if (should_bind) {
        bind_list.push_back(input_tag);
    }
}

void ekg::service::input::unbind(const std::string &input_tag, const std::string &key) {
    std::size_t it {};
    bool should_unbind {};
    auto &bind_list = this->map_bind[key];

    for (it = 0; it < bind_list.size(); it++) {
        if ((should_unbind = bind_list[it] == input_tag)) {
            break;
        }
    }

    if (should_unbind) {
        bind_list.erase(bind_list.cbegin() + (uint32_t) it);
    }
}

void ekg::service::input::set(const std::string &input_tag, bool callback) {
    this->map_register[input_tag] = callback;
}

bool ekg::service::input::get(const std::string &input_tag) {
    return this->map_register[input_tag];
}

void ekg::service::input::callback(const std::string &key, bool callback) {
    for (std::string &binds : this->map_bind[key]) {
        this->map_register[binds] = callback;
    }
}

void ekg::service::input::complete_with_units(std::string &string_builder, const std::string &key_name) {
    string_builder += this->special_key[SDLK_LCTRL];
    string_builder += this->special_key[SDLK_RCTRL];
    string_builder += this->special_key[SDLK_LSHIFT];
    string_builder += this->special_key[SDLK_RSHIFT];
    string_builder += this->special_key[SDLK_LALT];
    string_builder += this->special_key[SDLK_RALT];
    string_builder += this->special_key[SDLK_TAB];
    string_builder += key_name;
}

bool ekg::service::input::contains_unit(const std::string &label) {
    for (std::string &units : this->special_key_unit_pressed) {
        if (units == label) {
            return true;
        }
    }

    return false;
}
