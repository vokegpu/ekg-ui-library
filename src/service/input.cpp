#include "ekg/service/input.hpp"

void ekg::service::input::init() {
    this->registry("frame-drag-activy");
    this->bind("frame-drag-activy", "mouse-left");
    this->bind("frame-drag-activy", "finger-click");

    this->registry("frame-resize-activy");
    this->bind("frame-resize-activy", "mouse-left");
    this->bind("frame-resize-activy", "finger-click");

    this->registry("button-activy");
    this->bind("button-activy", "mouse-left");
    this->bind("button-activy", "finger-click");

    this->registry("popup-activy");
    this->bind("popup-activy", "mouse-right");
    this->bind("popup-activy", "finger-hold");

    this->registry("popup-component-activy");
    this->bind("popup-component-activy", "mouse-left");
    this->bind("popup-component-activy", "finger-click");
}

void ekg::service::input::on_event(SDL_Event &sdl_event) {
    this->pressed = sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_MOUSEBUTTONDOWN || sdl_event.type == SDL_FINGERDOWN;
    this->released = sdl_event.type == SDL_KEYUP || sdl_event.type == SDL_MOUSEBUTTONUP || sdl_event.type == SDL_FINGERUP;
    this->motion = sdl_event.type == SDL_MOUSEMOTION || sdl_event.type == SDL_FINGERMOTION;

    switch (sdl_event.type) {
        case SDL_KEYDOWN: {
            for (const char* key_name = SDL_GetKeyName(sdl_event.key.keysym.sym); strcmp(key_name, "") != 0; key_name = nullptr) {
                this->callback(key_name, true);
            }

            break;
        }

        case SDL_KEYUP: {
            for (const char* key_name = SDL_GetKeyName(sdl_event.key.keysym.sym); strcmp(key_name, "") != 0; key_name = nullptr) {
                this->callback(key_name, false);
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
