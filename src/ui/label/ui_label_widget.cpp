#include "ekg/ui/label/ui_label_widget.hpp"
#include "ekg/ekg.hpp"
#include "ekg/draw/draw.hpp"

void ekg::ui::label_widget::destroy() {
    abstract_widget::destroy();
}

void ekg::ui::label_widget::on_reload() {
    abstract_widget::on_reload();

    auto ui {(ekg::ui::label*) this->data};
    auto &rect = (this->data->widget() = this->layout + *this->parent);
    auto dock {ui->get_text_dock()};
    auto scaled_height {ui->get_scaled_height()};
    auto f_renderer_normal {ekg::core->get_f_renderer_normal()};

    float text_width {f_renderer_normal.get_text_width(ui->get_text())};
    float text_height {f_renderer_normal.get_text_height()};
    float offset {text_height / 3};

    this->layout.w = ekg::min(this->layout.w, text_width + offset);
    this->layout.h = (text_height + offset) * static_cast<float>(scaled_height);

    ekg::set_rect_clamped(this->layout, ekg::theme().min_widget_size);
    ekg::set_dock_scaled({0, 0, this->layout.w, this->layout.h}, {text_width, text_height}, this->docker_text);

    if (ekg::bitwise::contains(dock, ekg::dock::center)) {
        this->extra.x = this->docker_text.center.x;
        this->extra.y = this->docker_text.center.y;
    }

    if (ekg::bitwise::contains(dock, ekg::dock::left)) {
        this->extra.x = this->docker_text.left.x + offset;
    }

    if (ekg::bitwise::contains(dock, ekg::dock::right)) {
        this->extra.x = this->docker_text.right.x - offset;
    }

    if (ekg::bitwise::contains(dock, ekg::dock::top)) {
        this->extra.y = this->docker_text.top.y + offset;
    }

    if (ekg::bitwise::contains(dock, ekg::dock::bottom)) {
        this->extra.y = this->docker_text.bottom.y - offset;
    }
}

void ekg::ui::label_widget::on_pre_event(SDL_Event &sdl_event) {
    abstract_widget::on_pre_event(sdl_event);
}

void ekg::ui::label_widget::on_event(SDL_Event &sdl_event) {
    abstract_widget::on_event(sdl_event);
}

void ekg::ui::label_widget::on_post_event(SDL_Event &sdl_event) {
    abstract_widget::on_post_event(sdl_event);
}

void ekg::ui::label_widget::on_update() {
    abstract_widget::on_update();
}

void ekg::ui::label_widget::on_draw_refresh() {
    abstract_widget::on_draw_refresh();
    auto ui {(ekg::ui::label*) this->data};
    auto &rect = (this->data->widget() = this->layout + *this->parent);
    ekg::core->get_f_renderer_normal().blit(ui->get_text(), rect.x + this->extra.x, rect.y + this->extra.y, ekg::theme().label_string);
}