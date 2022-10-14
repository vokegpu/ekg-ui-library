#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include "ekg/util/util_event.hpp"
#include "ekg/util/util_ui.hpp"

ekg::ui::abstract_widget::abstract_widget() {
    this->parent = &ekg::empty;
}

ekg::ui::abstract_widget::~abstract_widget() {

}

void ekg::ui::abstract_widget::destroy() {

}

void ekg::ui::abstract_widget::on_reload() {
    ekg::dispatch(ekg::env::redraw);
}

void ekg::ui::abstract_widget::on_pre_event(SDL_Event &sdl_event) {
    if (ekg::was_pressed() || ekg::was_released() || ekg::was_motion()) {
        auto interact {ekg::interact()};
        auto &rect = (this->data->widget() = *this->parent + this->layout);
        this->flag.hovered = ekg::rect_collide_vec(rect, interact) && (this->data->get_parent_id() == 0 || ekg::rect_collide_vec(*this->parent, interact));
    }
}

void ekg::ui::abstract_widget::on_event(SDL_Event &sdl_event) {

}

void ekg::ui::abstract_widget::on_post_event(SDL_Event &sdl_event) {
    this->flag.hovered = false;
}

void ekg::ui::abstract_widget::on_update() {

}

void ekg::ui::abstract_widget::on_draw_refresh() {

}
