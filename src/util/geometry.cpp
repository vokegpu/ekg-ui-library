#include "ekg/util/geometry.hpp"
#include "ekg/ekg.hpp"

float ekg::display::dt {};
int32_t ekg::display::width {};
int32_t ekg::display::height {};
char* const ekg::empty {};

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

void ekg::set_dock_scaled(const ekg::rect &rect, float offset, ekg::docker &docker) {
    docker.rect = rect;

    docker.left.x = rect.x;
    docker.left.y = rect.y;
    docker.left.w = offset;
    docker.left.h = rect.h;

    docker.right.w = offset;
    docker.right.h = rect.h;
    docker.right.x = rect.x + rect.w - docker.right.w;
    docker.right.y = rect.y;

    docker.top.x = rect.x;
    docker.top.y = rect.y;
    docker.top.w = rect.w;
    docker.top.h = offset;

    docker.bottom.w = rect.w;
    docker.bottom.h = offset;
    docker.bottom.x = rect.x;
    docker.bottom.y = rect.y + rect.h - docker.bottom.h;
}

uint16_t ekg::docker_collide_vec_docks(ekg::docker &docker, const ekg::vec4 &vec) {
    uint16_t collided {ekg::dock::none};

    if (ekg::rect_collide_vec(docker.left, vec)) {
        ekg::bitwise::add(collided, ekg::dock::left);
    }

    if (ekg::rect_collide_vec(docker.right, vec)) {
        ekg::bitwise::add(collided, ekg::dock::right);
    }

    if (ekg::rect_collide_vec(docker.top, vec)) {
        ekg::bitwise::add(collided, ekg::dock::top);
    }

    if (ekg::rect_collide_vec(docker.bottom, vec)) {
        ekg::bitwise::add(collided, ekg::dock::bottom);
    }


    return collided;
}