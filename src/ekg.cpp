#include "ekg/ekg.hpp"

ekg::runtime* ekg::core {nullptr};
std::string ekg::gl_version {"#version 450 core"};

ekg::service::theme &ekg::theme() {
    return ekg::core->get_service_theme();
}

ekg::service::input &ekg::input() {
    return ekg::core->get_service_input();
}

void ekg::init(SDL_Window* root, const std::string &font_path) {
    ekg::log("initialising ekg...");
    ekg::gpu::init_opengl_context();

#if defined(_WIN)
    ekg::os = {ekg::platform::os_win};
#elif defined(__linux__)
    ekg::os = {ekg::platform::os_linux};
#elif defined(__ANDROID__)
    ekg::os = {ekg::platform::os_android};
#endif

    ekg::core = new ekg::runtime();
    ekg::core->get_f_renderer_small().font_path = font_path;
    ekg::core->get_f_renderer_normal().font_path = font_path;
    ekg::core->get_f_renderer_big().font_path = font_path;

    ekg::core->init();
    ekg::core->set_root(root);

    SDL_GetWindowSize(root, &ekg::display::width, &ekg::display::height);
    ekg::log("root display size (" + std::to_string(ekg::display::width) + ", " + std::to_string(ekg::display::height) + ")");
}

void ekg::quit() {
    ekg::log("quiting from ekg...");

    ekg::core->quit();
    delete ekg::core;
}

void ekg::event(SDL_Event &sdl_event) {
    bool phase_keep_process {
            sdl_event.type == SDL_MOUSEBUTTONDOWN || sdl_event.type == SDL_MOUSEBUTTONUP ||
            sdl_event.type == SDL_FINGERUP        || sdl_event.type == SDL_FINGERDOWN ||
            sdl_event.type == SDL_FINGERMOTION    || sdl_event.type == SDL_MOUSEMOTION ||
            sdl_event.type == SDL_KEYDOWN         || sdl_event.type == SDL_KEYUP ||
            sdl_event.type == SDL_WINDOWEVENT
    };

    if (!phase_keep_process) {
        return;
    }

    switch (sdl_event.type) {
        case SDL_WINDOWEVENT: {
            switch (sdl_event.window.type) {
                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    ekg::display::width = sdl_event.window.data1;
                    ekg::display::height = sdl_event.window.data2;
                    ekg::log("window resize event");
                    break;
                }
            }

            break;
        }
    }

    ekg::core->process_event(sdl_event);
}

void ekg::update() {
    ekg::core->process_update();
}

void ekg::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ekg::core->process_render();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void ekg::depth(float depth_preset) {

}

ekg::ui::frame *ekg::frame(std::string_view tag, const ekg::vec2 &initial_position, ekg::vec2 size) {
    auto element = new ekg::ui::frame();

    element->set_type(ekg::type::frame);
    ekg::core->create_ui(element);

    element->set_pos_initial(initial_position.x, initial_position.y);
    element->set_size_initial(size.x, size.y);
    element->set_tag(tag);

    return element;
}
