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
#include "ekg/ekg.hpp"

ekg_core* const ekg::core = new ekg_core();
float ekg::delta_time = 0.0f;
float ekg::text_dock_offset = 2.0f;

std::string ekg::get_version() {
    return "1.0.0";
}

void ekg::init(SDL_Window* &sdl_window) {
   ekgapi::init();
   ekgapi::OpenGL::init();

    if (EKG_CPU_PLATFORM == ekgapi::cpu::ARM) {
        // TODO Init SDL2 log to ARM platform.
    }

    core->set_instances(sdl_window);
    core->init();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    ekgutil::log("Core initialised.");
}

void ekg::quit() {
    core->quit();
}

void ekg::set_font(const char *path) {
    const std::string string = std::string(path);
    core->get_font_manager().load(string);

    if (core->get_font_manager().reload()) {}
}

void ekg::poll_event(SDL_Event &sdl_event) {
    core->process_event_section(sdl_event);
}

void ekg::update(float dt) {
    ekg::delta_time = dt;
    core->process_update_section();
}

void ekg::render() {
    core->process_render_section();
}

ekg_textbox* ekg::textbox() {
    auto textbox_worker = new ekg_textbox();
    core->add_element(textbox_worker);

    textbox_worker->set_width(100);
    textbox_worker->set_height(100);
    textbox_worker->set_max_columns(666);
    textbox_worker->set_max_rows(666);
    textbox_worker->set_visibility(ekg::visibility::VISIBLE);

    return textbox_worker;
}

ekg_combobox *ekg::combobox(const std::string &tag, const std::string &value, const std::vector<std::string> &vec) {
    auto combobox_worker = new ekg_combobox();
    core->add_element(combobox_worker);

    combobox_worker->set_text_dock(ekg::dock::LEFT);
    combobox_worker->add(vec);
    combobox_worker->set_value(value);
    combobox_worker->set_tag(tag);
    combobox_worker->set_width(125);
    combobox_worker->set_visibility(ekg::visibility::VISIBLE);
    combobox_worker->set_height(25);

    return combobox_worker;
}

ekg_popup *ekg::popup(const std::string &tag, const std::vector<std::string> &vec) {
    if (core->get_hovered_element_type() == ekg::ui::POPUP || core->get_hovered_element_type_input_down() == ekg::ui::POPUP) {
        return nullptr;
    }

    auto popup_worker = new ekg_popup();
    core->add_element(popup_worker);

    popup_worker->set_tag(tag);
    popup_worker->set_text_dock(ekg::dock::LEFT);
    popup_worker->add(vec);
    popup_worker->set_width(125);
    popup_worker->set_height(25);
    popup_worker->set_opened(true);
    popup_worker->set_pos(ekgapi::display_interact_x, ekgapi::display_interact_y);

    return popup_worker;
}

ekg_button* ekg::button(const std::string &text) {
    auto button_worker = new ekg_button();
    core->add_element(button_worker);

    button_worker->set_text(text);
    button_worker->set_visibility(ekg::visibility::VISIBLE);
    button_worker->set_text_dock(ekg::dock::LEFT);
    button_worker->set_width(125);
    button_worker->set_height(25);

    return button_worker;
}

ekg_slider* ekg::slider(float val, float min, float max) {
    auto slider_worker = new ekg_slider();
    core->add_element(slider_worker);

    slider_worker->set_visibility(ekg::visibility::VISIBLE);
    slider_worker->set_min(min);
    slider_worker->set_max(max);
    slider_worker->set_value(val);
    slider_worker->set_width(125);

    return slider_worker;
}

ekg_frame* ekg::frame() {
    auto frame_worker = new ekg_frame();
    core->add_element(frame_worker);

    frame_worker->set_visibility(ekg::visibility::VISIBLE);
    frame_worker->set_drag_dock(ekg::dock::TOP);
    frame_worker->set_size(300, 300);
    frame_worker->set_drag_offset(300);
    frame_worker->set_resize_offset(30.0f);

    return frame_worker;
}

ekg_checkbox* ekg::checkbox(const std::string &text) {
    auto checkbox_worker = new ekg_checkbox();
    core->add_element(checkbox_worker);

    checkbox_worker->set_visibility(ekg::visibility::VISIBLE);
    checkbox_worker->set_text_dock(ekg::dock::LEFT);
    checkbox_worker->set_text(text);

    return checkbox_worker;
}

ekg_theme &ekg::theme() {
    return core->get_theme_service().get_loaded_theme();
}

float ekg::depth() {
    return core->get_gpu_handler().get_depth_level();
}

void ekg::depth(float depth_level) {
    return core->get_gpu_handler().set_depth_level(depth_level);
}

void ekg::set_font_size(uint32_t size) {
    core->get_font_manager().set_size(size);
}

uint32_t ekg::hovered_element_id() {
    return core->get_hovered_element_id();
}

uint16_t ekg::hovered_element_type() {
    return core->get_hovered_element_type();
}

ekg_event* ekg::event() {
    return core->poll_event();
}