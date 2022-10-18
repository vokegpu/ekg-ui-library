#include "ekg/draw/draw.hpp"
#include "ekg/ekg.hpp"

void ekg::draw::rect(const ekg::rect &rect, const ekg::vec4 &color, int32_t line_thickness) {
    ekg::draw::rect(rect.x, rect.y, rect.w, rect.h, color, line_thickness);
}

void ekg::draw::rect(float x, float y, float w, float h, const ekg::vec4 &color, int32_t line_thickness) {
    if (color.w <= 0) {
        return;
    }

    ekg::gpu::allocator &allocator {ekg::core->get_gpu_allocator()};
    ekg::gpu::data &data {allocator.bind_current_data()};

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

    data.rect_area[0] = x;
    data.rect_area[1] = y;
    data.rect_area[2] = w;
    data.rect_area[3] = h;

    data.colored_area[0] = color.x / 255;
    data.colored_area[1] = color.y / 255;
    data.colored_area[2] = color.z / 255;
    data.colored_area[3] = color.w / 255;
    data.outline = line_thickness;
    data.factor = 1;

    allocator.dispatch();
}

void ekg::draw::scissor(int32_t x, int32_t y, int32_t w, int32_t h) {
    ekg::core->get_gpu_allocator().bind_scissor(x, y, w, h);
}

void ekg::draw::pop_scissor() {
    ekg::core->get_gpu_allocator().bind_scissor(-1, -1, -1, -1);
}
