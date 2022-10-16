#include "ekg/ui/checkbox/ui_checkbox_widget.hpp"

void ekg::ui::checkbox_widget::destroy() {
    abstract_widget::destroy();
}

void ekg::ui::checkbox_widget::on_reload() {
    abstract_widget::on_reload();
}

void ekg::ui::checkbox_widget::on_pre_event(SDL_Event &sdl_event) {
    abstract_widget::on_pre_event(sdl_event);
}

void ekg::ui::checkbox_widget::on_event(SDL_Event &sdl_event) {
    abstract_widget::on_event(sdl_event);
}

void ekg::ui::checkbox_widget::on_post_event(SDL_Event &sdl_event) {
    abstract_widget::on_post_event(sdl_event);
}

void ekg::ui::checkbox_widget::on_update() {
    abstract_widget::on_update();
}

void ekg::ui::checkbox_widget::on_draw_refresh() {
    abstract_widget::on_draw_refresh();
}
