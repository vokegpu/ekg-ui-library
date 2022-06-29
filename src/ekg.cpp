#include <ekg/ekg.hpp>
#include <ekg/ekg_api.hpp>

ekg_core ekg::core::instance;
float ekg::core::delta_time = 0.0f;

std::string ekg::get_version() {
    return "1.0.0";
}

void ekg::init(SDL_Window* &sdl_window) {
   api::init();
   api::OpenGL::init();
   ekg::core::init();

    if (EKG_CPU_PLATFORM == api::cpu::ARM) {
        // TODO Init SDL2 log to ARM platform.
    }

    ekg::core::instance.set_instances(sdl_window);
}

void ekg::quit() {

}

void ekg::set_font(const char *path) {

}

void ekg::events(SDL_Event &sdl_event) {
    ekg::core::instance.process_event_section(sdl_event);
}

void ekg::update(float dt) {
    ekg::core::delta_time = dt;
    ekg::core::instance.process_update_section();
}

void ekg::render() {
    ekg::core::instance.process_render_section();
}

void ekg::core::init() {
    utility::log("Core initialised.");
}