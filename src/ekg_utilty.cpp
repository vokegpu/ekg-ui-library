#include <ekg/ekg_utility.hpp>
#include <ekg/ekg_api.hpp>

void utility::log(const std::string &log) {
    api::send_output(log.c_str());
}

bool utility::contains(uint8_t &flags, uint8_t target) {
    return (flags & target) != 0;
}

bool utility::remove(uint8_t &flags, uint8_t target) {
    bool flag_contains = flags & target;
    flags &= ~(target);
    return flag_contains;
}

bool utility::add(uint8_t &flags, uint8_t val_flag) {
    flags |= val_flag;
    return true;
}

bool utility::stack::contains(uint32_t id) {
    for (uint32_t _ids : this->ids) {
        if (_ids == id) {
            return true;
        }
    }

    return false;
}

bool utility::stack::remove(uint32_t id) {
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

void utility::stack::add(uint32_t id) {
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

ekgmath::vec4::vec4() {

}

ekgmath::vec4::vec4(uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha) {
    this->x = (float) red / 255.0f;
    this->y = (float) green / 255.0f;
    this->z = (float) blue / 255.0f;
    this->w = (float) alpha / 255.0f;
}
