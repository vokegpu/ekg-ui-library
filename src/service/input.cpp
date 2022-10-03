#include "ekg/service/input.hpp"

void ekg::service::input::on_event(SDL_Event &sdl_event) {
    this->pressed = sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_MOUSEBUTTONDOWN || sdl_event.type == SDL_FINGERDOWN;
    this->released = sdl_event.type == SDL_KEYUP || sdl_event.type == SDL_MOUSEBUTTONUP || sdl_event.type == SDL_FINGERUP;
    this->motion = sdl_event.type == SDL_MOUSEMOTION || sdl_event.type == SDL_FINGERMOTION;

    switch (sdl_event.type) {
        case SDL_KEYDOWN: {
            for (const char* key_name = SDL_GetKeyName(sdl_event.key.keysym.sym); strcmp(key_name, "") != 0; key_name = nullptr) {
                this->set_state(this->map_call_action[key_name], true);
            }

            break;
        }

        case SDL_KEYUP: {
            for (const char* key_name = SDL_GetKeyName(sdl_event.key.keysym.sym); strcmp(key_name, "") != 0; key_name = nullptr) {
                this->set_state(this->map_call_action[key_name], false);
            }

            break;
        }

        case SDL_MOUSEBUTTONDOWN: {
            switch (sdl_event.button.button) {
                case SDL_BUTTON_LEFT: {
                    this->set_state(this->map_call_action["mouse-left"], true);
                    break;
                }

                case SDL_BUTTON_RIGHT: {
                    this->set_state(this->map_call_action["mouse-right"], true);
                    break;
                }

                case SDL_BUTTON_MIDDLE: {
                    this->set_state(this->map_call_action["mouse-middle"], true);
                    break;
                }

                default: {
                    this->set_state(this->map_call_action["mouse-" + std::to_string(sdl_event.button.button)], true);
                    break;
                }
            }

            break;
        }

        case SDL_MOUSEBUTTONUP: {
            switch (sdl_event.button.button) {
                case SDL_BUTTON_LEFT: {
                    this->set_state(this->map_call_action["mouse-left"], false);
                    break;
                }

                case SDL_BUTTON_RIGHT: {
                    this->set_state(this->map_call_action["mouse-right"], false);
                    break;
                }

                case SDL_BUTTON_MIDDLE: {
                    this->set_state(this->map_call_action["mouse-middle"], false);
                    break;
                }

                default: {
                    this->set_state(this->map_call_action["mouse-" + std::to_string(sdl_event.button.button)], false);
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

            this->set_state(this->map_call_action["finger-click"], true);
            this->last_finger_interact.x = sdl_event.tfinger.x;
            this->last_finger_interact.y = sdl_event.tfinger.y;

            break;
        }

        case SDL_FINGERUP: {
            this->set_state(this->map_call_action["finger-hold"], (this->finger_hold_event = ekg::reach(this->timing_last_interact, 750)));
            this->set_state(this->map_call_action["finger-click"], false);

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

void ekg::service::input::registry(const std::string &action_tag) {
    this->map_action_register[action_tag].state = false;
}

void ekg::service::input::add_bind(const std::string &action_tag, const std::string &action_key) {
    this->map_call_action[action_key] = action_tag;

    auto &task {this->map_action_register[action_tag]};
    bool bind {true};

    for (std::string &binds : task.call_bounds) {
        if (binds == action_key) {
            bind = false;
            break;
        }
    }

    if (bind) {
        task.call_bounds.push_back(action_key);
    }
}

void ekg::service::input::remove_bind(const std::string &action_tag, const std::string &action_key) {
    this->map_call_action[action_key].clear();

    auto &task {this->map_action_register[action_tag]};
    bool unbind {};

    std::size_t index {};

    for (index = 0; index < task.call_bounds.size(); index++) {
        if ((unbind = task.call_bounds.at(index) == action_key)) {
            break;
        }
    }

    if (unbind) {
        task.call_bounds.erase(task.call_bounds.begin() + (uint32_t) index);
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
        this->set_state(this->map_call_action["finger-hold"], false);
        this->finger_hold_event = false;
    }
}

void ekg::service::input::set_state(const std::string &action_tag, bool action_state) {
    this->map_action_register[action_tag].state = action_state;
}

bool ekg::service::input::get_state(const std::string &action_tag) {
    return this->map_action_register[action_tag].state;
}
