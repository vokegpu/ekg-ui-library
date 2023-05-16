#include "ekg/ui/scroll/ui_scroll_emebedded_widget.hpp"

void ekg::ui::scroll_emebedded_widget::on_destroy() {

}
 
void ekg::ui::scroll_emebedded_widget::on_reload() {
    if (this->mother_id == 0) {
        return;
    }

    this->rect_child.x = 0;
    this->rect_child.y = 0;

    ekg::ui::abstract_widget *mother_widget {ekg::core->get_fast_widget_by_id(this->mother_id)};
    ekg::ui::abstract_widget *widgets {};
    float service_layout_min_offset {ekg::core->get_service_layout().get_min_offset()}

    switch (mother_widget->data->get_type()) {
    case ekg::type::frame:
        ekg::ui::frame_widget *frame {(ekg::ui::frame_widget*) mother_widget};
        this->rect_child.h += mother_widget->docker_activy_drag.top.h;
        break;
    default:
        break;
    }

    for (int32_t &ids : this->child_id_list) {
        if ((widgets = ekg::core->get_fast_widget_by_id(ids)) == nullptr) {
            continue;
        }

        if (widgets->rect.w > this->rect_child.w) {
            this->rect_child.w = widgets->rect.w;
        }

        this->rect_child.h += widgets->rect.w;
    }
}

void ekg::ui::scroll_emebedded_widget::on_pre_event(SDL_Event &sdl_event) {

}

void ekg::ui::scroll_emebedded_widget::on_event(SDL_Event &sdl_event) {

}

void ekg::ui::scroll_emebedded_widget::on_post_event(SDL_Event &sdl_event) {

}

void ekg::ui::scroll_emebedded_widget::on_update() {

}

void ekg::ui::scroll_emebedded_widget::on_draw_refresh() {

}