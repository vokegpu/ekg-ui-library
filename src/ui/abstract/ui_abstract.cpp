#include "ekg/ui/abstract/ui_abstract.hpp"

void ekg::ui::abstract::set_id(uint32_t token) {
    this->id = token;
}

uint32_t ekg::ui::abstract::get_id() {
    return this->id;
}

void ekg::ui::abstract::set_parent_id(uint32_t token) {
    this->parent_id = token;
}

uint32_t ekg::ui::abstract::get_parent_id() {
    return this->parent_id;
}

void ekg::ui::abstract::set_tag(const std::string &str) {
    this->tag = str;
}

std::string ekg::ui::abstract::get_tag() {
    return this->tag;
}

void ekg::ui::abstract::set_extra_tag(const std::string &str) {
    this->extra_tag = str;
}

std::string ekg::ui::abstract::get_extra_tag() {
    return this->extra_tag;
}
