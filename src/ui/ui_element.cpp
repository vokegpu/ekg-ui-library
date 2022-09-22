#include "ekg/ui/ui_element.hpp"

void ekg::element::set_id(uint32_t token) {
    this->id = token;
}

uint32_t ekg::element::get_id() {
    return this->id;
}

void ekg::element::set_parent_id(uint32_t token) {
    this->parent_id = token;
}

uint32_t ekg::element::get_parent_id() {
    return this->parent_id;
}

void ekg::element::set_tag(const std::string &str) {
    this->tag = str;
}

std::string ekg::element::get_tag() {
    return this->tag;
}

void ekg::element::set_extra_tag(const std::string &str) {
    this->extra_tag = str;
}

std::string ekg::element::get_extra_tag() {
    return this->extra_tag;
}
