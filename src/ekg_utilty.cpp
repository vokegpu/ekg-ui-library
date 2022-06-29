#include <ekg/ekg_utility.hpp>
#include <ekg/ekg_api.hpp>

void utility::log(const char *log) {
    api::send_output(log);
}

bool utility::contains(uint8_t &flags, uint8_t target) {
    return flags & target;
}

bool utility::remove(uint8_t &flags, uint8_t target) {
    bool flag_contains = flags & target;
    flags = flags & ~(target);
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
