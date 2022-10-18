#include "ekg/ui/abstract/ui_abstract.hpp"
#include "ekg/ekg.hpp"

ekg::ui::abstract::abstract() {

}

ekg::ui::abstract::~abstract() {
}

void ekg::ui::abstract::parent(uint32_t token) {
    bool not_contains {true};

    for (uint32_t &tokens : this->parent_id_list) {
        if (!(not_contains = tokens != token)) {
            break;
        }
    }

    if (not_contains) {
        auto widget = ekg::core->get_fast_widget_by_id(token);

        if (widget != nullptr) {
            auto group = ekg::core->get_fast_widget_by_id(this->id);
            this->parent_id_list.push_back(token);
            widget->data->set_parent_id(this->id);
            widget->parent = &group->data->widget();
        }
    }
}

std::vector<uint32_t> &ekg::ui::abstract::get_parent_id_list() {
    return this->parent_id_list;
}

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

void ekg::ui::abstract::set_alive(bool flag_alive) {
    this->alive = flag_alive;
}

bool ekg::ui::abstract::is_alive() {
    return this->alive;
}

void ekg::ui::abstract::set_state(const ekg::state &enum_state) {
    this->state = enum_state;
}

ekg::state ekg::ui::abstract::get_state() {
    return this->state;
}

void ekg::ui::abstract::set_type(const ekg::type &enum_type) {
    this->type = enum_type;
}

ekg::type ekg::ui::abstract::get_type() {
    return this->type;
}

ekg::rect &ekg::ui::abstract::widget() {
    return this->rect_widget;
}

uint16_t ekg::ui::abstract::get_dock() {
    return this->dock;
}

ekg::rect &ekg::ui::abstract::ui() {
    return this->sync_ui;
}

void ekg::ui::abstract::set_sync_with_ui(bool sync_state) {
    if (this->sync_with_ui != sync_state) {
        this->sync_with_ui = sync_state;
        ekg::reload(this->id);
    }
}

bool ekg::ui::abstract::should_sync_with_ui() {
    return this->sync_with_ui;
}

void ekg::ui::abstract::set_tag(std::string_view string) {
    this->tag = string;
}

std::string_view ekg::ui::abstract::get_tag() {
    return this->tag;
}
