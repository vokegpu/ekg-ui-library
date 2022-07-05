#include <ekg/ekg_element_button.hpp>
#include <ekg/ekg_gpu.hpp>

ekg_button::ekg_button() {
    this->rect.x = 20;
    this->rect.y = 20;
    this->rect.w = 200;
    this->rect.h = 200;
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
    ekggpu::inject(this->master_id);

    ekgmath::vec4 color(255, 0, 255, 100);
    ekggpu::rectangle(this->rect, color);

    if (this->flag.flag_over) {
        color.color(255, 255, 255, 50);
        ekggpu::rectangle(this->rect, color);
    }

    color.color(0, 0, 255, 100);
    ekggpu::rectangle(this->rect, color);
}