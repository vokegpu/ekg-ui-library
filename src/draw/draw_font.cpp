#include "ekg/draw/draw_font.hpp"

FT_Library ekg::draw::font_renderer::ft_library {};

float ekg::draw::font_renderer::get_text_width(const std::string &text) {
    return 0;
}

float ekg::draw::font_renderer::get_text_height() {
    return 0;
}

void ekg::draw::font_renderer::set_size(uint8_t size) {

}

void ekg::draw::font_renderer::reload() {
    if
}

void ekg::draw::font_renderer::bind_allocator(ekg::gpu::allocator *gpu_allocator) {

}

void ekg::draw::font_renderer::blit(const std::string &text, float x, float y, const ekg::vec4 &color) {
    if (this->allocator == nullptr) {
        return;
    }


}

void ekg::draw::font_renderer::init() {

}

void ekg::draw::font_renderer::quit() {

}
