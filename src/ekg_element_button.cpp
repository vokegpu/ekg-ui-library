#include "ekg/impl/ekg_ui_element_button.hpp"
#include "ekg/api/ekg_gpu.hpp"
#include "ekg/api/ekg_font.hpp"

ekg_button::ekg_button() {

}

ekg_button::~ekg_button() {

}

void ekg_button::on_killed() {
    ekg_element::on_killed();
}

void ekg_button::on_sync() {
    ekg_element::on_sync();

    this->min_text_width = ekgfont::get_text_width(this->text);
    this->min_text_height = ekgfont::get_text_height(this->text);

    this->rect.w = this->rect.w < this->min_text_width ? this->min_text_width : this->rect.w;
    this->rect.h = this->rect.h < this->min_text_height ? this->min_text_height : this->rect.h;
}

void ekg_button::on_pre_event_update(SDL_Event &sdl_event) {
    ekg_element::on_pre_event_update(sdl_event);

    float x = 0;
    float y = 0;

    if (ekgapi::motion(sdl_event, x, y)) {
        this->flag.flag_over = this->rect.collide_aabb_with_point(x, y);
        this->flag.flag_highlight = this->flag.flag_over;
    }
}

void ekg_button::on_event(SDL_Event &sdl_event) {
    ekg_element::on_event(sdl_event);
}

void ekg_button::on_post_event_update(SDL_Event &sdl_event) {
    ekg_element::on_post_event_update(sdl_event);

    this->flag.flag_over = false;
}

void ekg_button::on_draw_refresh() {
    ekg_element::on_draw_refresh();

    ekgmath::vec4f color(255, 255, 255, 50);
    ekggpu::rectangle(this->rect, color);

    if (this->flag.flag_highlight) {
        color.color(255, 255, 255, 50);
        ekggpu::rectangle(this->rect, color);
    }

    color.color(255, 255, 255, 255);
    ekgfont::render(this->text, this->rect.x, this->rect.y, color);
}