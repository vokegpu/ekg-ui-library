#include "ekg/ui/button/ui_button_widget.hpp"
#include "ekg/ui/button/ui_button.hpp"
#include "ekg/util/env.hpp"
#include "ekg/ekg.hpp"
#include "ekg/draw/draw.hpp"

void ekg::ui::button_widget::destroy() {
    abstract_widget::destroy();
}

void ekg::ui::button_widget::on_reload() {
    abstract_widget::on_reload();

    auto ui {(ekg::ui::button*) this->data};
    auto &rect {this->data->rect()};
    auto dock {ui->get_text_dock()};
    auto scaled_height {ui->get_scaled_height()};
    auto f_renderer_normal {ekg::core->get_f_renderer_normal()};

    float text_width {f_renderer_normal.get_text_width(ui->get_text())};
    float text_height {f_renderer_normal.get_text_height()};
    float offset {text_height / 3};

    rect.h = (text_height + offset * 2) * static_cast<float>(scaled_height);
    ekg::set_rect_clamped(rect, ekg::theme().min_widget_size);
    ekg::set_dock_scaled(rect, text_width, this->docker_text);

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

void ekg::ui::button_widget::on_pre_event(SDL_Event &sdl_event) {
    abstract_widget::on_pre_event(sdl_event);
}

void ekg::ui::button_widget::on_event(SDL_Event &sdl_event) {
    abstract_widget::on_event(sdl_event);
    ekg::set(this->flag.highlight, this->flag.hovered);

    if (ekg::was_pressed() && !this->flag.activy && this->flag.activy && ekg::input("button-activy")) {
        ekg::set(this->flag.activy, true);
    } else if (ekg::was_released() && this->flag.activy) {
        auto ui {(ekg::ui::button*) this->data};
        ui->set_value(this->flag.hovered);

        if (ui->get_value() && ui->get_callback() != nullptr) {
            ekg::dispatch(ui->get_callback());
            ui->set_value(false);
        }

        ekg::set(this->flag.activy, false);
    }
}

void ekg::ui::button_widget::on_post_event(SDL_Event &sdl_event) {
    abstract_widget::on_post_event(sdl_event);
}

void ekg::ui::button_widget::on_update() {
    abstract_widget::on_update();
}

void ekg::ui::button_widget::on_draw_refresh() {
    abstract_widget::on_draw_refresh();
    auto ui {(ekg::ui::button*) this->data};
    auto &rect {ui->rect()};
    auto &theme {ekg::theme()};

    ekg::draw::rect(rect, theme.button_background);
    ekg::draw::rect(rect, theme.button_outline);

    if (this->flag.highlight) {
        ekg::draw::rect(rect, theme.button_highlight);
    }

    if (this->flag.activy) {
        ekg::draw::rect(rect, theme.button_activy);
        ekg::draw::rect(rect, {theme.button_activy, theme.button_outline.w});
    }

    ekg::core->get_f_renderer_normal().blit(ui->get_text(), this->extra.x, this->extra.y, theme.button_string);
}
