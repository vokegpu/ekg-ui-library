#include "ekg/ui/checkbox/ui_checkbox_widget.hpp"
#include "ekg/util/env.hpp"
#include "ekg/ekg.hpp"
#include "ekg/draw/draw.hpp"
#include "ekg/ui/checkbox/ui_checkbox.hpp"

void ekg::ui::checkbox_widget::destroy() {
    abstract_widget::destroy();
}

void ekg::ui::checkbox_widget::on_reload() {
    abstract_widget::on_reload();

    auto ui {(ekg::ui::checkbox*) this->data};
    auto &rect = (this->data->widget() = this->layout + *this->parent);
    auto dock {ui->get_text_dock()};
    auto scaled_height {ui->get_scaled_height()};
    auto f_renderer {ekg::f_renderer(ui->get_font_size())};

    float text_width {f_renderer.get_text_width(ui->get_text())};
    float text_height {f_renderer.get_text_height()};
    float offset {text_height / 3};
    float box_size {text_height};
    float min_width {box_size + offset + offset + (ui->get_text().empty() ? 0 : text_width + offset)};

    this->layout.w = ekg::min(this->layout.w, min_width);
    this->layout.h = (text_height + offset) * static_cast<float>(scaled_height);
    this->offset.w = box_size;
    this->offset.h = box_size;

    ekg::set_rect_clamped(this->layout, ekg::theme().min_widget_size);
    ekg::set_dock_scaled({0, 0, this->layout.w, this->layout.h}, {min_width, text_height}, this->docker_text);

    if (ekg::bitwise::contains(dock, ekg::dock::center)) {
        this->extra.x = this->docker_text.center.x + offset + text_width;
        this->extra.y = this->docker_text.center.y;
        this->offset.x = this->docker_text.center.x + offset;
        this->offset.y = this->docker_text.center.y;
    }

    if (ekg::bitwise::contains(dock, ekg::dock::left)) {
        this->extra.x = this->docker_text.left.x + box_size + offset + offset;
        this->offset.x = this->docker_text.left.x + offset;
    }

    if (ekg::bitwise::contains(dock, ekg::dock::right)) {
        this->extra.x = this->docker_text.right.x + offset;
        this->offset.x = this->docker_text.right.x + box_size + offset + offset;
    }

    if (ekg::bitwise::contains(dock, ekg::dock::top)) {
        this->extra.y = this->docker_text.top.y;
        this->offset.y = this->docker_text.top.y;
    }

    if (ekg::bitwise::contains(dock, ekg::dock::bottom)) {
        this->extra.y = this->docker_text.bottom.y;
        this->offset.y = this->docker_text.bottom.y;
    }
}

void ekg::ui::checkbox_widget::on_pre_event(SDL_Event &sdl_event) {
    abstract_widget::on_pre_event(sdl_event);
}

void ekg::ui::checkbox_widget::on_event(SDL_Event &sdl_event) {
    abstract_widget::on_event(sdl_event);

    bool pressed {ekg::input::pressed()};
    bool released {ekg::input::released()};

    if (ekg::input::motion() || pressed || released) {
        ekg::set(this->flag.highlight, this->flag.hovered);
        ekg::set(this->flag.focused, this->flag.hovered && ekg::rect_collide_vec(this->offset + (this->layout + *this->parent), ekg::interact()));
    }

    if (pressed && !this->flag.activy && this->flag.hovered && ekg::input::pressed("checkbox-activy")) {
        ekg::set(this->flag.activy, true);
    } else if (released && this->flag.activy) {
        if (this->flag.hovered) {
            auto ui {(ekg::ui::checkbox*) this->data};
            ui->set_value(!ui->get_value());
        }

        ekg::set(this->flag.activy, false);
    }
}

void ekg::ui::checkbox_widget::on_post_event(SDL_Event &sdl_event) {
    abstract_widget::on_post_event(sdl_event);
}

void ekg::ui::checkbox_widget::on_update() {
    abstract_widget::on_update();
}

void ekg::ui::checkbox_widget::on_draw_refresh() {
    abstract_widget::on_draw_refresh();
    auto ui {(ekg::ui::checkbox*) this->data};
    auto &rect = (this->data->widget() = this->layout + *this->parent);
    auto &theme {ekg::theme()};
    auto &f_renderer {ekg::f_renderer(ui->get_font_size())};
    auto box {this->offset + rect};

    ekg::draw::rect(rect, theme.checkbox_background);
    ekg::draw::rect(rect, theme.checkbox_outline);

    if (this->flag.highlight) {
        ekg::draw::rect(rect, theme.checkbox_highlight);
    }

    if (this->flag.focused && !ui->get_value()) {
        ekg::draw::rect(box, theme.checkbox_highlight);
    }

    ekg::draw::rect(box, theme.checkbox_highlight);

    if (this->flag.activy) {
        ekg::draw::rect(this->flag.focused ? box : rect, theme.checkbox_activy);
    }

    if (ui->get_value()) {
        ekg::draw::rect(box, theme.checkbox_activy);
    }

    if (ui->get_value() && this->flag.highlight) {
        ekg::draw::rect(box, {theme.checkbox_activy, theme.checkbox_outline.w}, 1);
    }

    f_renderer.blit(ui->get_text(), rect.x + this->extra.x, rect.y + this->extra.y, theme.checkbox_string);
}
