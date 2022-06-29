#include <ekg/ekg_api.hpp>
#include <ekg/ekg_utility.hpp>

uint64_t ekg_cpu_timing::last_ticks = 0;
bool ekg_cpu_timing::clock_going_on = false;

bool ekg_cpu_timing::start() {
    last_ticks = SDL_GetTicks64();
    clock_going_on = true;

    return clock_going_on;
}

bool ekg_cpu_timing::endif(uint64_t ms) {
    clock_going_on = false;
    return SDL_GetTicks64() - last_ticks > ms;
}

bool api::input1(SDL_Event &sdl_event) {
    switch (sdl_event.type) {
        case SDL_FINGERUP: {
            ekg_cpu_timing::clock_going_on = false;
            return true;
        }

        case SDL_FINGERDOWN: {
            return true;
        }

        case SDL_MOUSEBUTTONDOWN: {
            return sdl_event.motion.type == SDL_BUTTON_LEFT;
        }

        case SDL_MOUSEBUTTONUP: {
            ekg_cpu_timing::clock_going_on = false;
            return sdl_event.motion.type == SDL_BUTTON_LEFT;
        }
    }

    return false;
}

bool api::input2(SDL_Event &sdl_event) {
    switch (sdl_event.type) {
        case SDL_FINGERUP: {
            return ekg_cpu_timing::endif(EKG_ACTIVE_CALLBACK_MS_DELAY);
        }

        case SDL_FINGERDOWN: {
            ekg_display_touch_input.last_down_x = sdl_event.tfinger.x;
            ekg_display_touch_input.last_down_y = sdl_event.tfinger.y;

            return true;
        }

        case SDL_MOUSEBUTTONDOWN: {
            return sdl_event.motion.type == SDL_BUTTON_RIGHT;
        }

        case SDL_MOUSEBUTTONUP: {
            ekg_cpu_timing::clock_going_on = false;
            return sdl_event.motion.type == SDL_BUTTON_RIGHT;
        }
    }

    return false;
}

bool api::input3(SDL_Event &sdl_event) {
    switch (sdl_event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            return sdl_event.motion.type == SDL_BUTTON_MIDDLE;
        }

        case SDL_MOUSEBUTTONUP: {
            ekg_cpu_timing::clock_going_on = false;
            return sdl_event.motion.type == SDL_BUTTON_MIDDLE;
        }
    }

    return false;
}

void api::init() {
    EKG_ACTIVE_CALLBACK_MS_DELAY = 500;
    EKG_CPU_PLATFORM = api::cpu::X86;
}

float api::motion(SDL_Event &sdl_event, float x, float y) {
    if (!ekg_cpu_timing::clock_going_on) {
        return 0.0f;
    }

    switch (sdl_event.type) {
        case SDL_FINGERMOTION: {
            return y - ekg_display_touch_input.last_down_y;
        }

        case SDL_MOUSEWHEEL: {
            return sdl_event.wheel.preciseY;
        }
    }

    return 0.0f;
}

void api::send_output(const char *output) {
    switch (EKG_CPU_PLATFORM) {
        case api::cpu::X86: {
            std::string str = "[EKG] " + std::string(output);
            std::cout << str.c_str() << "\n";
            break;
        }

        case api::cpu::ARM: {
            //SDL_Log("[EKG] %s", output);
            break;
        }
    }
}

void api::OpenGL::init() {
    utility::log("API OpenGL initialised.");
}
