/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, free to use in anything (utility, hacking, cheat, game, software).
 * 5- Malware, rat and others virus. We do not care.
 * 6- Do not modify this license under any instance.
 *
 * @VokeGpu 2022 all rights reserved.
 */

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

    data.rect_area[0] = x;
    data.rect_area[1] = y;
    data.rect_area[2] = w;
    data.rect_area[3] = h;

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

    data.colored_area[0] = static_cast<int32_t>(color.x);
    data.colored_area[1] = static_cast<int32_t>(color.y);
    data.colored_area[2] = static_cast<int32_t>(color.z);
    data.colored_area[3] = static_cast<int32_t>(color.w);
    data.mode = line_thickness;
    data.factor = 1;

    allocator.dispatch();
}

void ekg::draw::bind_scissor(int32_t id) {
    ekg::core->get_gpu_allocator().bind_scissor(id);
}

int32_t ekg::draw::bind_scissor() {
    auto &allocator {ekg::core->get_gpu_allocator()};
    allocator.invoke_scissor();
    return allocator.get_instance_scissor_id();
}

void ekg::draw::scissor(int32_t x, int32_t y, int32_t w, int32_t h) {
    ekg::core->get_gpu_allocator().scissor(x, y, w, h);
}

void ekg::draw::bind_off_scissor() {
    ekg::core->get_gpu_allocator().revoke_scissor();
}

void ekg::draw::bind_animation(uint32_t id_tag) {
    ekg::core->get_gpu_allocator().bind_animation(id_tag);
}

void ekg::draw::bind_off_animation() {
    ekg::core->get_gpu_allocator().bind_off_animation();
}