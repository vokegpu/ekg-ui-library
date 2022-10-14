#include "ekg/service/layout.hpp"
#include "ekg/ui/frame/ui_frame.hpp"
#include "ekg/ekg.hpp"

void ekg::service::layout::process(ekg::ui::abstract_widget *widget) {

}

void ekg::service::layout::process_scaled(ekg::ui::abstract_widget* widget) {
    auto type {widget->data->get_type()};
    auto is_group {type == ekg::type::frame};

    if (!is_group) {
        return;
    }

    ekg::ui::abstract_widget* widgets {};
    for (uint32_t &ids : widget->data->get_parent_id_list()) {
        widgets = ekg::core->get_fast_widget_by_id(ids);

        if (widgets == nullptr) {
            continue;
        }

        auto &rect {widgets->data->rect()};
    }
}

void ekg::service::layout::init() {
    this->min_factor_height = 1;
    this->min_height =ekg::core->get_f_renderer_normal().get_text_height();
    this->min_offset = this->min_height / 3;
    this->min_height += this->min_offset * 2;
}

void ekg::service::layout::quit() {

}
