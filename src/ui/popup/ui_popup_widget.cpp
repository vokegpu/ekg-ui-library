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
#include "ekg/draw/draw.hpp"

void ekg::ui::popup_widget::on_destroy() {

}

void ekg::ui::popup_widget::on_reload() {
    ekg::ui::abstract_widget::on_reload();

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

    auto text_dock_flags {ui->get_text_align()};
    float offset {ekg::find_min_offset(max_width, dimension_offset)};
    float size {(text_height + dimension_offset) * static_cast<float>(ui->get_scaled_height())};

    ekg::vec3 layout_offset {offset, offset, text_height + dimension_offset};
    auto &layout_mask {layout.get_layout_mask()};

    this->dimension.w = ekg::min(this->dimension.w, max_width + dimension_offset);
    this->dimension.h = offset;

    for (int32_t it {}; it < this->element_list.size(); it++) {
        auto &component {component_list.at(it)};
        auto &element {this->element_list.at(it)};

        element.rect_bound.w = this->dimension.w;
        element.rect_bound.h = size;
        element.rect_bound.x = 0;
        element.rect_bound.y = this->dimension.h;

        layout_offset.z = element.rect_bound.h;

        layout.set_preset_mask(layout_offset, ekg::axis::horizontal, element.rect_bound.w);
        layout.insert_into_mask({&element.rect_text, text_dock_flags});
        layout.process_layout_mask();

        // Process the popup height based in layout mask.
        this->dimension.h += layout_mask.h + (offset * static_cast<float>(element.separator));
    }

    /* Extra offset to final rect. */
    this->dimension.h += offset;

    /* Reset scissor height if current state is invisible. */
    auto *gpu_scissor {ekg::core->get_gpu_allocator().get_scissor_by_id(this->data->get_id())};
    if (ui->get_state() == ekg::state::invisible && gpu_scissor != nullptr) {
        gpu_scissor->rect[3] = this->scissor_opened_height = 0;
    } else if (ui->get_state() == ekg::state::visible && !this->is_high_frequency) {
        ekg::update_high_frequency(this);
        this->elapsed_animation_ticks = SDL_GetTicks64();
    }

    /* Fix screen position at space. */
    if (!ui->has_parent()) {
        float display_w {static_cast<float>(ekg::display::width)};
        float display_h {static_cast<float>(ekg::display::height)};

        if (this->parent->x < 0) {
            this->parent->x = 0;
        }

        if (this->parent->y < 0) {
            this->parent->y = 0;
        }

        if (this->parent->x + this->dimension.w > display_w) {
            this->parent->x = display_w - this->dimension.w;
        }

        if (this->parent->y + this->dimension.h > display_h) {
            this->parent->y = display_h - this->dimension.h;
        }
    }

    this->min_size.x = ekg::min(this->min_size.x, this->dimension.w);
    this->min_size.y = ekg::min(this->min_size.y, this->dimension.h);
}

void ekg::ui::popup_widget::on_pre_event(SDL_Event &sdl_event) {
    if (this->data->get_state() == ekg::state::invisible) {
        return;
    }

    abstract_widget::on_pre_event(sdl_event);
}

void ekg::ui::popup_widget::on_event(SDL_Event &sdl_event) {
    if (this->data->get_state() == ekg::state::invisible) {
        return;
    }

    bool check_hovered {};
    bool pressed {ekg::input::pressed()};
    bool released {ekg::input::released()};

    int32_t hovered {-1};

    if (ekg::input::motion() || pressed || released) {
        check_hovered = this->flag.hovered;
    }

    if (check_hovered) {
        ekg::vec4 &interact {ekg::interact()};
        ekg::rect &rect {this->get_abs_rect()};

        for (int32_t it {}; it < this->element_list.size(); it++) {
            auto &element {this->element_list.at(it)};

            if (ekg::rect_collide_vec(element.rect_bound + rect, interact)) {
                hovered = it;
                break;
            }
        }

        ekg::set(this->hovered_element, hovered);
    }

    if (hovered != -1 && pressed) {
        this->focused_element = hovered;
    } else if (hovered != -1 && released) {
        this->data->destroy();
    } else if (hovered == -1 && pressed) {
        this->data->destroy();
    }
}

void ekg::ui::popup_widget::on_post_event(SDL_Event &sdl_event) {
    abstract_widget::on_post_event(sdl_event);
}

void ekg::ui::popup_widget::on_update() {
    auto &rect {this->get_abs_rect()};
    auto *gpu_scissor {ekg::core->get_gpu_allocator().get_scissor_by_id(this->data->get_id())};

    gpu_scissor->rect[0] = rect.x;
    gpu_scissor->rect[1] = rect.y;
    gpu_scissor->rect[2] = rect.w;

    float animation {ekg::smooth(100, SDL_GetTicks64() - this->elapsed_animation_ticks)};
    this->scissor_opened_height = animation * this->dimension.h;

    if (EQUALS_FLOAT(this->scissor_opened_height, this->dimension.h)) {
        scissor_opened_height = this->dimension.h;
        this->is_high_frequency = false;
    }

    gpu_scissor->rect[3] = this->scissor_opened_height;
}

void ekg::ui::popup_widget::on_draw_refresh() {
    auto ui {(ekg::ui::popup*) this->data};
    auto &rect {this->get_abs_rect()};
    auto &theme {ekg::theme()};
    auto &f_renderer {ekg::f_renderer(ui->get_font_size())};
    auto &component_list {ui->get_component_list()};

    ekg::draw::bind_scissor(ui->get_id());
    ekg::draw::rect(rect, theme.popup_background);
    ekg::draw::rect(rect, theme.popup_outline, ekg::drawmode::outline);

    for (int32_t it {}; it < this->element_list.size(); it++) {
        auto &component {component_list.at(it)};
        auto &element {this->element_list.at(it)};

        if (this->hovered_element == it) ekg::draw::rect(element.rect_bound + rect, theme.popup_highlight);
        f_renderer.blit(component.name, rect.x + element.rect_bound.x + element.rect_text.x, rect.y + element.rect_bound.y + element.rect_text.y, theme.popup_string);
    }

    ekg::draw::bind_off_scissor();
}