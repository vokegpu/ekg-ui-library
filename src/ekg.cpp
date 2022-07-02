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

    if (EKG_CPU_PLATFORM == api::cpu::ARM) {
        // TODO Init SDL2 log to ARM platform.
    }

    ekg::core::instance.set_instances(sdl_window);
    ekg::core::init();
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

ekg::button* ekg::create_button(std::string text) {
    auto button_worker = new ekg_button();
    auto button = new ekg::button(button_worker);

    button->set_text(text);

    // Add into context handler.
    ekg::core::instance.add_element((ekg_abstract_element*) button_worker);

    return button;
}

void ekg::core::init() {
    utility::log("Core initialised.");
    ekg::core::instance.init();
}

ekg::button::button(ekg_button* &element) {
    this->element = element;
}

ekg::button::~button() {

}

void ekg::button::set_width(float val) {
    val = ekgmath::clampf(val, this->element->min_text_width, 12666.0f);

    if (this->element->rect.h != val) {
        this->element->rect.h = val;
        this->element->on_sync();
    }
}

float ekg::button::get_width() {
    return this->element->rect.w;
}

void ekg::button::set_height(float val) {
    val = ekgmath::clampf(val, this->element->min_text_height, 12666.0f);

    if (this->element->rect.h != val) {
        this->element->rect.h = val;
        this->element->on_sync();
    }
}

float ekg::button::get_height() {
    return this->element->rect.h;
}

void ekg::button::set_text(std::string &text) {
    if (this->element->text != text) {
        this->element->text = text;
        this->element->on_sync();
    }
}

std::string ekg::button::get_text() {
    return this->element->text;
}
