#include <ekg/ekg.hpp>

ekg_core* const the_ekg_core = new ekg_core();
float ekg::core::delta_time = 0.0f;

std::string ekg::get_version() {
    return "1.0.0";
}

void ekg::init(SDL_Window* &sdl_window) {
   ekgapi::init();
   ekgapi::OpenGL::init();

    if (EKG_CPU_PLATFORM == ekgapi::cpu::ARM) {
        // TODO Init SDL2 log to ARM platform.
    }

    the_ekg_core->set_instances(sdl_window);
    ekg::core::init();
}

void ekg::quit() {
    the_ekg_core->quit();
}

void ekg::set_font(const char *path) {
    const std::string string = std::string(path);
    the_ekg_core->get_font_manager().load(string);

    if (the_ekg_core->get_font_manager().reload()) {}
}

void ekg::poll_event(SDL_Event &sdl_event) {
    the_ekg_core->process_event_section(sdl_event);
}

void ekg::update(float dt) {
    ekg::core::delta_time = dt;
    the_ekg_core->process_update_section();
}

void ekg::render() {
    the_ekg_core->process_render_section();
}

ekg_button* ekg::button(std::string text) {
    auto button_worker = new ekg_button();
    button_worker->set_text(text);

    // Add into context handler.
    the_ekg_core->add_element(button_worker);

    return button_worker;
}

ekg_slider* ekg::slider(float val, float min, float max) {
    auto slider_worker = new ekg_slider();
    slider_worker->set_min(min);
    slider_worker->set_max(max);
    slider_worker->set_value(val);
    slider_worker->set_width(125);

    // Add into context handler.
    the_ekg_core->add_element(slider_worker);

    return slider_worker;
}

ekg_frame* ekg::frame() {
    auto frame_worker = new ekg_frame();
    frame_worker->set_size(300, 300);

    // Add into context handler.
    the_ekg_core->add_element(frame_worker);

    return frame_worker;
}

ekg_check_box* ekg::check_box(std::string text) {
    auto check_box_worker = new ekg_check_box();
    check_box_worker->set_text(text);

    // Add into context handler.
    the_ekg_core->add_element(check_box_worker);

    return check_box_worker;
}

ekg_theme &ekg::theme() {
    return the_ekg_core->get_theme_service().get_loaded_theme();
}

float ekg::depth() {
    return the_ekg_core->get_gpu_handler().get_depth_level();
}

void ekg::depth(float depth_level) {
    return the_ekg_core->get_gpu_handler().set_depth_level(depth_level);
}

void ekg::set_font_size(uint32_t size) {
    the_ekg_core->get_font_manager().set_size(size);
}

void ekg::core::init() {
    ekgutil::log("Core initialised.");
    the_ekg_core->init();

    glEnable(GL_DEPTH_TEST);
}

void ekg::core::quit() {
    the_ekg_core->quit();
}