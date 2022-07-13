#include <ekg/api/ekg_utility.hpp>
#include <ekg/api/ekg_api.hpp>
#include <ekg/ekg.hpp>

void ekgutil::log(const std::string &log) {
    ekgapi::send_output(log.c_str());
}

bool ekgutil::contains(uint16_t &flags, uint16_t target) {
    return (flags & target) != 0;
}

bool ekgutil::remove(uint16_t &flags, uint16_t target) {
    bool flag_contains = flags & target;
    flags &= ~(target);
    return flag_contains;
}

bool ekgutil::add(uint16_t &flags, uint16_t val_flag) {
    if (ekgutil::contains(flags, val_flag)) {
        return false;
    }

    flags |= val_flag;
    return true;
}

bool ekgutil::find_axis_dock(uint16_t &target, float px, float py, float offset, ekgmath::rect &rect) {
    target = ekg::dock::UNDEFINED;

    if (!rect.collide_aabb_with_point(px, py)) {
        return false;
    }

    float x = rect.x;
    float y = rect.y;
    float w = rect.w;
    float h = rect.h;

    bool phase1 = px > x && px < x + offset;
    bool phase2 = py > y && py < y + offset;
    bool phase3 = px > x + w - offset && px < x + w;
    bool phase4 = py > y + h - offset && py < y + h;
    bool phase5 = px > x + (w / 2) - offset && px < x + (w / 2) + offset &&
                  py > y + (h / 2) - offset && py > y + (h / 2) + offset;

    target = phase1 ? target | ekg::dock::LEFT   : target;
    target = phase2 ? target | ekg::dock::TOP    : target;
    target = phase3 ? target | ekg::dock::RIGHT  : target;
    target = phase4 ? target | ekg::dock::BOTTOM : target;
    target = phase5 ? target | ekg::dock::CENTER : target;

    return target != ekg::dock::UNDEFINED;
}

bool ekgutil::stack::contains(uint32_t id) {
    for (uint32_t &_ids : this->ids) {
        if (_ids == id) {
            return true;
        }
    }

    return false;
}

bool ekgutil::stack::remove(uint32_t id) {
    int32_t index = -1;

    for (uint32_t i = 0; i < this->ids.size(); i++) {
        if (this->ids.at(i) == id) {
            index = (int32_t) i;
            break;
        }
    }

    if (index != -1) {
        this->ids.erase(this->ids.begin() + index);
        return true;
    }

    return false;
}

void ekgutil::stack::add(uint32_t id) {
    if (!this->contains(id)) {
        this->ids.push_back(id);
    }
}

bool ekgmath::rect::collide_aabb_with_point(float px, float py) {
    return px > this->x && px < this->x + this->w && py > this->y && py < this->y + this->h;
}

void ekgmath::rect::copy(ekgmath::rect &r) {
    this->x = r.x;
    this->y = r.y;
    this->w = r.w;
    this->h = r.h;
}

float ekgmath::lerpf(float a, float b, float t) {
    return a + (b - a) * t;
}

float ekgmath::clampf(float val, float min, float max) {
    return val > min ? (val < max ? val : max) : min;
}

void ekgmath::ortho2d(float *mat, float left, float right, float bottom, float top) {
    const float z_near = -1.0f;
    const float z_far = 1.0f;
    const float inv_z = 1.0f / (z_far - z_near);
    const float inv_y = 1.0f / (top - bottom);
    const float inv_x = 1.0f / (right - left);

    uint8_t i = 0;

    mat[i++] = 2.0f * inv_x;
    mat[i++] = 0.0f;
    mat[i++] = 0.0f;
    mat[i++] = 0.0f;

    mat[i++] = 0.0f;
    mat[i++] = 2.0f * inv_y;
    mat[i++] = 0.0f;
    mat[i++] = 0.0f;

    mat[i++] = 0.0f;
    mat[i++] = 0.0f;
    mat[i++] = -2.0f * inv_z;
    mat[i++] = 0.0f;

    mat[i++] = -(right + left) * inv_x;
    mat[i++] = -(top + bottom) * inv_y;
    mat[i++] = -(z_far + z_near) * inv_z;
    mat[i++] = 1.0f;
}

ekgmath::vec4f::vec4f() {

}

ekgmath::vec4f::vec4f(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
    this->color(red, green, blue, alpha);
}

void ekgmath::vec4f::color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
    this->x = (float) red / 255.0f;
    this->y = (float) green / 255.0f;
    this->z = (float) blue / 255.0f;
    this->w = (float) alpha / 255.0f;
}
