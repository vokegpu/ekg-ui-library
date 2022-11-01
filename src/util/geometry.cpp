/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, use in any thing (hacking, cheating, games, softwares).
 * 5- All malwares, rat and others virus. We do not care.
 * 6- Do not modify this license under any circunstancie.
 *
 * @VokeGpu 2022 all rights reserved.
 */

#include "ekg/util/geometry.hpp"
#include "ekg/ekg.hpp"

float ekg::display::dt {};
int32_t ekg::display::width {};
int32_t ekg::display::height {};

bool ekg::rect_collide_rect(const ekg::rect &rect_a, const ekg::rect &rect_b) {
    return rect_a.x < rect_b.x + rect_b.w && rect_a.x + rect_a.w > rect_b.x &&
           rect_a.y < rect_b.y + rect_b.h && rect_a.x + rect_a.w > rect_b.y;
}

void ekg::orthographic2d(float *matrix, float left, float right, float bottom, float top) {
    const float depth_near = -1.0f;
    const float depth_far = 1.0f;
    const float depth_inv = 1.0f / (depth_far - depth_near);
    const float y_inv = 1.0f / (top - bottom);
    const float x_inv = 1.0f / (right - left);

    matrix[0] = 2.0f * x_inv;
    matrix[1] = 0.0f;
    matrix[2] = 0.0f;
    matrix[3] = 0.0f;

    matrix[4] = 0.0f;
    matrix[5] = 2.0f * y_inv;
    matrix[6] = 0.0f;
    matrix[7] = 0.0f;

    matrix[8] = 0.0f;
    matrix[9] = 0.0f;
    matrix[10] = -2.0f * depth_inv;
    matrix[11] = 0.0f;

    matrix[12] = -(right + left) * x_inv;
    matrix[13] = -(top + bottom) * y_inv;
    matrix[14] = -(depth_far + depth_near) * depth_inv;
    matrix[15] = 1.0f;
}

bool ekg::rect_collide_vec(const ekg::rect &rect, const ekg::vec4 &vec) {
    return vec.x > rect.x && vec.x < rect.x + rect.w && vec.y > rect.y && vec.y < rect.y + rect.h;
}

ekg::vec4 &ekg::interact() {
    return ekg::core->get_service_input().interact;
}

void ekg::set_dock_scaled(const ekg::rect &rect, const ekg::vec2 &offset, ekg::docker &docker) {
    docker.rect = rect;

    docker.left.x = rect.x;
    docker.left.y = rect.y;
    docker.left.w = offset.x;
    docker.left.h = rect.h;

    docker.right.w = offset.x;
    docker.right.h = rect.h;
    docker.right.x = rect.x + rect.w - offset.x;
    docker.right.y = rect.y;

    docker.top.x = rect.x;
    docker.top.y = rect.y;
    docker.top.w = rect.w;
    docker.top.h = offset.y;

    docker.bottom.w = rect.w;
    docker.bottom.h = offset.y;
    docker.bottom.x = rect.x;
    docker.bottom.y = rect.y + rect.h - offset.y;

    auto dx = offset.x / 2;
    auto dy = offset.y / 2;

    docker.center.x = rect.x + (rect.w / 2) - dx;
    docker.center.y = rect.y + (rect.h / 2) - dy;
    docker.center.w = dx;
    docker.center.h = dy;
}

int32_t ekg::find_collide_dock(ekg::docker &docker, uint16_t flags, const ekg::vec4 &vec) {
    if (ekg::bitwise::contains(flags, ekg::dock::full) && ekg::rect_collide_vec(docker.rect, vec)) {
        return ekg::dock::full;
    }

    uint16_t collided {ekg::dock::none};

    collided = ekg::bitwise::contains(flags, ekg::dock::left) && ekg::rect_collide_vec(docker.left, vec) ? ekg::bitwise::add(collided, ekg::dock::left) : collided;
    collided = ekg::bitwise::contains(flags, ekg::dock::right) && ekg::rect_collide_vec(docker.right, vec) ? ekg::bitwise::add(collided, ekg::dock::right) : collided;
    collided = ekg::bitwise::contains(flags, ekg::dock::top) && ekg::rect_collide_vec(docker.top, vec) ? ekg::bitwise::add(collided, ekg::dock::top) : collided;
    collided = ekg::bitwise::contains(flags, ekg::dock::bottom) && ekg::rect_collide_vec(docker.bottom, vec) ? ekg::bitwise::add(collided, ekg::dock::bottom) : collided;
    collided = ekg::bitwise::contains(flags, ekg::dock::center) && ekg::rect_collide_vec(docker.center, vec) ? ekg::bitwise::add(collided, ekg::dock::center) : collided;

    return collided;
}

void ekg::set_rect_clamped(ekg::rect &rect, float min_size) {
    rect.x = ekg::min(rect.x, min_size);
    rect.y = ekg::min(rect.y, min_size);
    rect.w = ekg::min(rect.w, min_size);
    rect.h = ekg::min(rect.h, min_size);
}

float ekg::min(float a, float b) {
    return a < b ? b : a;
}

float ekg::max(float a, float b) {
    return a > b ? b : a;
}

bool ekg::rect::operator == (const ekg::rect &rect) const {
    return this->x == rect.x && this->y == rect.y && this->w == rect.w && this->h == rect.h;
}

bool ekg::rect::operator!=(const ekg::rect &rect) const {
    return this->x != rect.x || this->y != rect.y || this->w != rect.w || this->h != rect.h;
}

bool ekg::rect::operator==(const ekg::vec4 &vec) const {
    return this->x == vec.x && this->y == vec.y && this->w == vec.z && this->h == vec.w;
}

bool ekg::rect::operator!=(const ekg::vec4 &vec) const {
    return this->x != vec.x || this->y != vec.y || this->w != vec.z || this->h != vec.w;
}

ekg::rect::rect(float posx, float posy, const ekg::vec2 &vec) {
    this->x = posx;
    this->y = posy;
    this->w = vec.x;
    this->h = vec.y;
}

ekg::rect::rect(const ekg::vec2 &vec, float sizew, float sizeh) {
    this->x = vec.x;
    this->y = vec.y;
    this->w = sizew;
    this->h = sizeh;
}

ekg::rect::rect(const ekg::vec2 &pos, const ekg::vec2 &size) {
    this->x = pos.x;
    this->y = pos.y;
    this->w = size.x;
    this->h = size.y;
}

ekg::rect::rect(float posx, float posy, float sizew, float sizeh) {
    this->x = posx;
    this->y = posy;
    this->w = sizew;
    this->h = sizeh;
}

ekg::rect ekg::rect::operator-(const ekg::rect &rect) const {
    return {this->x - rect.x, this->y - rect.y, this->w, this->h};
}

ekg::rect ekg::rect::operator+(const ekg::rect &rect) const {
    return {this->x + rect.x, this->y + rect.y, this->w, this->h};
}

ekg::vec4::vec4(float posx, float posy, const ekg::vec2 &vec) {
    this->x = posx;
    this->y = posy;
    this->z = vec.x;
    this->w = vec.y;
}

ekg::vec4::vec4(const ekg::vec2 &vec, float posz, float posw) {
    this->x = vec.x;
    this->y = vec.y;
    this->z = posz;
    this->w = posw;
}

ekg::vec4::vec4(const ekg::vec2 &pos, const ekg::vec2 &size) {
    this->x = pos.x;
    this->y = pos.y;
    this->z = size.x;
    this->w = size.y;
}

ekg::vec4::vec4(float posx, float posy, float posz, float posw) {
    this->x = posx;
    this->y = posy;
    this->z = posz;
    this->w = posw;
}

ekg::vec4::vec4(const ekg::vec4 &pos, float depth) {
    this->x = pos.x;
    this->y = pos.y;
    this->z = pos.z;
    this->w = depth;
}

ekg::vec2 ekg::vec2::operator/(float f) {
    return {this->x / f, this->y / f};
}
