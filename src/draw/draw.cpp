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
 * @VokeGpu 2023 all rights reserved.
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

    data.shape_rect[0] = x;
    data.shape_rect[1] = y;
    data.shape_rect[2] = w;
    data.shape_rect[3] = h;

    allocator.vertex2f(0.0f, 0.0f);
    allocator.vertex2f(0.0f, 1.0f);
    allocator.vertex2f(1.0f, 1.0f);
    allocator.vertex2f(1.0f, 1.0f);
    allocator.vertex2f(1.0f, 0.0f);
    allocator.vertex2f(0.0f, 0.0f);

    allocator.coord2f(0.0f, 0.0f);
    allocator.coord2f(0.0f, 0.0f);
    allocator.coord2f(1.0f, 1.0f);
    allocator.coord2f(1.0f, 1.0f);
    allocator.coord2f(1.0f, 1.0f);
    allocator.coord2f(0.0f, 0.0f);

    data.material_color[0] = color.x;
    data.material_color[1] = color.y;
    data.material_color[2] = color.z;
    data.material_color[3] = color.w;
    data.line_thickness = line_thickness;
    data.factor = 1;

    allocator.dispatch();
}

void ekg::draw::sync_scissor(ekg::rect &child_rect, int32_t mother_parent_id) {
    ekg::core->get_gpu_allocator().sync_scissor(child_rect, mother_parent_id);
}

void ekg::draw::bind_scissor(int32_t id) {
    ekg::core->get_gpu_allocator().bind_scissor(id);
}

void ekg::draw::bind_off_scissor() {
    ekg::core->get_gpu_allocator().bind_off_scissor();
}

bool ekg::draw::is_visible(int32_t scissor_id, ekg::vec4 &interact) {
    auto scissor {ekg::core->get_gpu_allocator().get_scissor_by_id(scissor_id)};
    return scissor != nullptr && ekg::rect_collide_vec({scissor->rect[0], scissor->rect[1], scissor->rect[2], scissor->rect[3]}, interact);
}