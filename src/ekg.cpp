/**
 * EKG-LICENSE - this software/library LICENSE can not be modified in any instance.
 *
 * --
 * ANY NON-OFFICIAL MODIFICATION IS A CRIME.
 * DO NOT SELL THIS CODE SOFTWARE, FOR USE EKG IN A COMMERCIAL PRODUCT ADD EKG-LICENSE TO PROJECT,
 * RESPECT THE COPYRIGHT TERMS OF EKG, NO SELL WITHOUT EKG-LICENSE (IT IS A CRIME).
 * DO NOT FORK THE PROJECT SOURCE WITHOUT EKG-LICENSE.
 *
 * END OF EKG-LICENSE.
 **/
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

ekg_popup *ekg::popup(const std::string &text, const std::vector<std::string> &vec) {
    if (the_ekg_core->get_hovered_element_type() == ekg::ui::POPUP) {
        return nullptr;
    }

    auto popup_worker = new ekg_popup();
    the_ekg_core->add_element(popup_worker);

    popup_worker->set_tag(text);
    popup_worker->set_text_dock(ekg::dock::CENTER);
    popup_worker->add(vec);
    popup_worker->set_width(125);
    popup_worker->set_height(25);
    popup_worker->set_opened(true);
    popup_worker->set_pos(ekgapi::display_interact_x, ekgapi::display_interact_y);

    return popup_worker;
}

ekg_button* ekg::button(const std::string &text) {
    auto button_worker = new ekg_button();
    the_ekg_core->add_element(button_worker);

    button_worker->set_text(text);
    button_worker->set_visibility(ekg::visibility::VISIBLE);
    button_worker->set_text_dock(ekg::dock::CENTER);

    return button_worker;
}

ekg_slider* ekg::slider(float val, float min, float max) {
    auto slider_worker = new ekg_slider();
    the_ekg_core->add_element(slider_worker);

    slider_worker->set_visibility(ekg::visibility::VISIBLE);
    slider_worker->set_min(min);
    slider_worker->set_max(max);
    slider_worker->set_value(val);
    slider_worker->set_width(125);

    return slider_worker;
}

ekg_frame* ekg::frame() {
    auto frame_worker = new ekg_frame();
    the_ekg_core->add_element(frame_worker);

    frame_worker->set_visibility(ekg::visibility::VISIBLE);
    frame_worker->set_drag_dock(ekg::dock::TOP);
    frame_worker->set_size(300, 300);
    frame_worker->set_drag_offset(300);
    frame_worker->set_resize_offset(30.0f);

    return frame_worker;
}

ekg_check_box* ekg::check_box(const std::string &text) {
    auto check_box_worker = new ekg_check_box();
    the_ekg_core->add_element(check_box_worker);

    check_box_worker->set_visibility(ekg::visibility::VISIBLE);
    check_box_worker->set_text(text);
    check_box_worker->set_text_dock(ekg::dock::LEFT);

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

uint32_t ekg::hovered_element_id() {
    return the_ekg_core->get_hovered_element_id();
}

uint16_t ekg::hovered_element_type() {
    return the_ekg_core->get_hovered_element_type();
}

ekg_event &ekg::event() {
    return the_ekg_core->poll_event();
}

void ekg::core::init() {
    ekgutil::log("Core initialised.");
    the_ekg_core->init();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void ekg::core::quit() {
    the_ekg_core->quit();
}