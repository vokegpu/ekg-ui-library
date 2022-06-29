#include <ekg/ekg.hpp>
#include <ekg/ekg_api.hpp>

ekg_core ekg::core::instance;

std::string ekg::get_version() {
    return "1.0.0";
}

void ekg::init() {
   api::init();
   api::OpenGL::init();
   ekg::core::init();

    if (EKG_CPU_PLATFORM == api::cpu::ARM) {
        // TODO Init SDL2 log to ARM platform.
    }
}

void ekg::quit() {

}

void ekg::set_font(const char *path) {

}

void ekg::core::init() {
    utility::log("Core initialised.");
}

void ekg::core::add(ekg_abstract_element* &element) {
    instance.add_element(element);
}
