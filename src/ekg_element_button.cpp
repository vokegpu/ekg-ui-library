#include <ekg/ekg_element_button.hpp>
#include <ekg/ekg_gpu.hpp>
#include <ekg/ekg_font.hpp>

ekg_button::ekg_button() {

}

ekg_button::~ekg_button() {

}

void ekg_button::on_killed() {
    ekg_abstract_element::on_killed();
}

void ekg_button::on_sync() {
    ekg_abstract_element::on_sync();
}

void ekg_button::on_pre_event_update(SDL_Event &sdl_event) {
    ekg_abstract_element::on_pre_event_update(sdl_event);

    float x = 0;
    float y = 0;

    if (ekgapi::motion(sdl_event, x, y)) {
        this->flag.flag_over = this->rect.collide_aabb_with_point(x, y);
        this->flag.flag_highlight = this->flag.flag_over;
    }
}

void ekg_button::on_event(SDL_Event &sdl_event) {
    ekg_abstract_element::on_event(sdl_event);
}

void ekg_button::on_post_event_update(SDL_Event &sdl_event) {
    ekg_abstract_element::on_post_event_update(sdl_event);

    this->flag.flag_over = false;
}

void ekg_button::on_draw_refresh() {
    ekg_abstract_element::on_draw_refresh();

    ekgmath::vec4f color(190, 190, 190, 200);
    ekggpu::rectangle(10, 10, 200, 200, color);

    color.color(190, 0, 190, 255);
    ekggpu::rectangle(10, 10, 200, 200, color);

    ekgfont::render("oi sou lidna", 10, 10, color);
}