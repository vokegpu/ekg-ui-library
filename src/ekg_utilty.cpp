#include <ekg/ekg_utility.hpp>
#include <ekg/ekg_api.hpp>

void ekgutil::log(const std::string &log) {
    ekgapi::send_output(log.c_str());
}

bool ekgutil::contains(uint8_t &flags, uint8_t target) {
    return (flags & target) != 0;
}

bool ekgutil::remove(uint8_t &flags, uint8_t target) {
    bool flag_contains = flags & target;
    flags &= ~(target);
    return flag_contains;
}

bool ekgutil::add(uint8_t &flags, uint8_t val_flag) {
    flags |= val_flag;
    return true;
}

bool ekgutil::stack::contains(uint32_t id) {
    for (uint32_t _ids : this->ids) {
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
