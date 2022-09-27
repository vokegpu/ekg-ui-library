#include "ekg/draw/draw.hpp"
#include "ekg/ekg.hpp"

void ekg::draw::rect(const ekg::rect &rect, const ekg::vec4 &color, int32_t line_thickness) {
    ekg::draw::rect(static_cast<int32_t>(rect.x), static_cast<int32_t>(rect.y), static_cast<int32_t>(rect.w), static_cast<int32_t>(rect.h), color, line_thickness);
}

void ekg::draw::rect(int32_t x, int32_t y, int32_t w, int32_t h, const ekg::vec4 &color, int32_t line_thickness) {
    ekg::gpu::allocator &allocator = ekg::core->get_gpu_allocator();
    ekg::gpu::data &data = allocator.bind_current_data();

    allocator.vertex2f(0, 0);
    allocator.vertex2f(0, 1);
    allocator.vertex2f(1, 1);
    allocator.vertex2f(1, 1);
    allocator.vertex2f(1, 0);
    allocator.vertex2f(0, 0);

    allocator.coord2f(0, 0);
    allocator.coord2f(0, 1);
    allocator.coord2f(1, 1);
    allocator.coord2f(1, 1);
    allocator.coord2f(1, 0);
    allocator.coord2f(0, 0);

    data.rect_area[0] = static_cast<float>(x);
    data.rect_area[1] = static_cast<float>(y);
    data.rect_area[2] = static_cast<float>(w);
    data.rect_area[3] = static_cast<float>(h);

    data.colored_area[0] = color.x / 255;
    data.colored_area[1] = color.y / 255;
    data.colored_area[2] = color.z / 255;
    data.colored_area[3] = color.w / 255;
    data.outline = line_thickness;

    allocator.dispatch();
}

void ekg::draw::scissor(int32_t x, int32_t y, int32_t w, int32_t h) {
    ekg::core->get_gpu_allocator().bind_scissor(x, y, w, h);
}

void ekg::draw::pop_scissor() {
    ekg::core->get_gpu_allocator().bind_scissor(-1, -1, -1, -1);
}
