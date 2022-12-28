/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, free to use in anything (utility, hacking, cheat, game, software).
 * 5- Malware, rat and others virus. We do not care.
 * 6- Do not modify this license under any instance.
 *
 * @VokeGpu 2023 all rights reserved.
 */

#include "ekg/ui/popup/ui_popup_widget.hpp"
#include "ekg/ui/popup/ui_popup.hpp"
#include "ekg/ekg.hpp"

void ekg::ui::popup_widget::destroy() {

}

void ekg::ui::popup_widget::on_reload() {
	auto ui {(ekg::ui::popup*) this->data};
	auto &f_renderer {ekg::f_renderer(ui->get_font_size())};
    auto &component_list {ui->get_component_list()};
    auto &layout {ekg::core->get_service_layout()};

    float max_width {};
    float text_width {};
    float text_height {f_renderer.get_text_height()};
    float dimension_offset {text_height / 2};

    /* First phase: get text metrics and prevent multi-iterations. */

    this->element_list.clear();
    for (ekg::component &components : ui->get_component_list()) {
        text_width = f_renderer.get_text_width(components.name);

        ekg::ui::popup_widget::element element {};
        element.rect_text.w = text_width;
        element.rect_text.h = text_height;

        if (text_width > max_width) {
            max_width = text_width;
        }

        this->element_list.push_back(element);
    }

    /* Second phase: compute bounds and layout mask. */

    float offset {ekg::find_min_offset(max_width, dimension_offset)};
    float text_dock_flags {ui->get_text_align()};
    
    ekg::vec3 layout_offset {offset, offset, text_height + dimension_offset};
    auto &layout_mask {layout.get_layout_mask()};

    this->dimension.w = ekg::min(this->dimension.w, max_width + dimension_offset);
    this->dimension.h = offset;

    for (int32_t it {}; it < this->element_list.size(); it++) {
        auto &component {component_list.at(it)};
        auto &element {this->element_list.at(it)};

        element.rect_bound.w = this->dimension.w;
        element.rect_bound.h = element.rect_text.h + dimension_offset;
        element.rect_bound.x = 0;
        element.rect_bound.y = this->dimension.h;

        layout_offset.z = element.rect_bound.h;

        layout.set_preset_mask(layout_offset, ekg::axis::horizontal, element.rect_bound.w);
        layout.insert_into_mask({&element.rect_text, text_dock_flags});
        layout.process_layout_mask();

        // Process the popup height based in layout mask.
        this->dimension.h += layout_mask.h;
    }
}

void ekg::ui::popup_widget::on_pre_event(SDL_Event &sdl_event) {

}

void ekg::ui::popup_widget::on_event(SDL_Event &sdl_event) {

}

void ekg::ui::popup_widget::on_post_event(SDL_Event &sdl_event) {
}

void ekg::ui::popup_widget::on_update() {

}

void ekg::ui::popup_widget::on_draw_refresh() {
    auto ui {(ekg::ui::popup*) this->data};
    auto &rect {this->get_abs_rect()};
    auto &theme {ekg::theme()};
    auto &f_renderer {ekg::f_renderer(ui->get_font_size())};

    ekg::draw::bind_scissor(ui->get_id());

    for (uint32_t it {}; it < this->element_list.size(); it++) {
        auto &element {this->element_list.at(it)};

        ekg::draw::rect(element.rect_bound + rect, theme.popup_background);
        f_renderer.blit(rect.x + element.rect_text.x, rect.y + element_list.rect_text.y, theme.popup_string);
    }

    ekg::draw::bind_off_scissor();
}