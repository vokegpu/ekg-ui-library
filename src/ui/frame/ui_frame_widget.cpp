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

#include "ekg/ui/frame/ui_frame_widget.hpp"
#include "ekg/ui/frame/ui_frame.hpp"
#include "ekg/ekg.hpp"
#include "ekg/draw/draw.hpp"
#include "ekg/util/util_ui.hpp"
#include "ekg/os/system_cursor.hpp"

void ekg::ui::frame_widget::on_destroy() {
    abstract_widget::on_destroy();
}

void ekg::ui::frame_widget::on_reload() {
    abstract_widget::on_reload();
    auto &rect {this->get_abs_rect()};
    this->ui_theme_activy_offset = static_cast<float>(ekg::theme().frame_activy_offset);
}

void ekg::ui::frame_widget::on_pre_event(SDL_Event &sdl_event) {
    abstract_widget::on_pre_event(sdl_event);
}

void ekg::ui::frame_widget::on_event(SDL_Event &sdl_event) {
    abstract_widget::on_event(sdl_event);

    auto &interact {ekg::interact()};
    auto ui {(ekg::ui::frame*) this->data};
    auto &rect {this->get_abs_rect()};

    uint16_t drag_dock_flags {ui->get_drag_dock()};
    uint16_t resize_dock_flags {ui->get_resize_dock()};
    uint16_t shown_cursor_dock_flags {};

    if ((drag_dock_flags != ekg::dock::none || resize_dock_flags != ekg::dock::none) && ekg::input::pressed() && this->flag.hovered && !this->flag.activy && (ekg::input::action("frame-drag-activy") || ekg::input::action("frame-resize-activy"))) {
        ekg::rect rect_visible_scissor {rect};
        ekg::draw::get_visible(this->data->get_id(), rect_visible_scissor);

        const ekg::vec2 vec_limit_offset {this->ui_theme_activy_offset, this->ui_theme_activy_offset};
        ekg::set_dock_scaled(rect_visible_scissor, vec_limit_offset, this->docker_activy_drag);
        ekg::set_dock_scaled(rect_visible_scissor, vec_limit_offset / 4.0f, this->docker_activy_resize);

        this->target_dock_drag = ekg::find_collide_dock(this->docker_activy_drag, drag_dock_flags, interact);
        this->target_dock_resize = ekg::find_collide_dock(this->docker_activy_resize, resize_dock_flags, interact);

        shown_cursor_dock_flags = this->target_dock_resize;

        this->rect_delta.x = interact.x - rect.x;
        this->rect_delta.y = interact.y - rect.y;
        this->rect_delta.w = rect.w;
        this->rect_delta.h = rect.h;

        this->flag.activy = this->target_dock_drag != ekg::dock::none || this->target_dock_resize != ekg::dock::none;
        this->flag.absolute = this->flag.activy;
        this->is_scissor_refresh = this->flag.activy;
    } else if (ekg::input::motion() && this->flag.activy) {
        ekg::rect new_rect {rect};
        shown_cursor_dock_flags = this->target_dock_resize;

        if (this->target_dock_drag != ekg::dock::none && this->target_dock_resize == ekg::dock::none) {
            new_rect.x = interact.x - this->rect_delta.x;
            new_rect.y = interact.y - this->rect_delta.y;
        }

        if (this->target_dock_resize != ekg::dock::none) {
            if (ekg::bitwise::contains(this->target_dock_resize, ekg::dock::left)) {
                float diff {((interact.x - new_rect.x) - this->rect_delta.x)};
                new_rect.x = (interact.x - this->rect_delta.x);
                new_rect.w -= diff;
            }

            if (ekg::bitwise::contains(this->target_dock_resize, ekg::dock::right)) {
                new_rect.w = this->rect_delta.w + ((interact.x - new_rect.x) - this->rect_delta.x);
            }

            if (ekg::bitwise::contains(this->target_dock_resize, ekg::dock::top)) {
                float diff {((interact.y - new_rect.y) - this->rect_delta.y)};
                new_rect.y = (interact.y - this->rect_delta.y);
                new_rect.h -= diff;
            }

            if (ekg::bitwise::contains(this->target_dock_resize, ekg::dock::bottom)) {
                new_rect.h = this->rect_delta.h + ((interact.y - new_rect.y) - this->rect_delta.y);
            }
        }

        ekg::set_rect_clamped(new_rect, 30.0f);

        if (rect != new_rect) {
            if (ui->has_parent()) {
                this->dimension.x = new_rect.x - this->parent->x;
                this->dimension.y = new_rect.y - this->parent->y;
            } else {
                this->parent->x = new_rect.x;
                this->parent->y = new_rect.y;
            }

            this->dimension.w = new_rect.w;
            this->dimension.h = new_rect.h;

            ui->ui() = new_rect;

            /*
             * Is very important to use synclaout before scissor.
             */
            if (this->target_dock_resize != ekg::dock::none) {
                this->is_targeting_absolute_parent = true;
                ekg::synclayout(this);
            }

            ekg::reload(this);
        }
    } else if (resize_dock_flags != ekg::dock::none && this->flag.hovered && !this->flag.activy) {
        ekg::rect rect_visible_scissor {};
        ekg::draw::get_visible(this->data->get_id(), rect_visible_scissor);

        const ekg::vec2 vec_limit_offset {this->ui_theme_activy_offset, this->ui_theme_activy_offset};
        ekg::set_dock_scaled(rect_visible_scissor, vec_limit_offset / 4.0f, this->docker_activy_resize);
        shown_cursor_dock_flags = ekg::find_collide_dock(this->docker_activy_resize, resize_dock_flags, interact);
    }

    if (shown_cursor_dock_flags) {
        bool top {ekg::bitwise::contains(shown_cursor_dock_flags, ekg::dock::top)};
        bool bottom {ekg::bitwise::contains(shown_cursor_dock_flags, ekg::dock::bottom)};
        bool left {ekg::bitwise::contains(shown_cursor_dock_flags, ekg::dock::left)};
        bool right {ekg::bitwise::contains(shown_cursor_dock_flags, ekg::dock::right)};

        if ((top && left) || (bottom && right)) {
            ekg::cursor = ekg::systemcursor::sizenwse;
        } else if ((top && right) || (bottom && left)) {
            ekg::cursor = ekg::systemcursor::sizenesw;
        } else if (top || bottom) {
            ekg::cursor = ekg::systemcursor::sizens;
        } else if (left || right) {
            ekg::cursor = ekg::systemcursor::sizewe;
        }
    }

    if (ekg::input::released()) {
        if (this->flag.activy) {
            this->flag.absolute = false;
        }

        this->target_dock_resize = ekg::dock::none;
        this->target_dock_drag = ekg::dock::none;
        this->flag.activy = false;
    }
}

void ekg::ui::frame_widget::on_post_event(SDL_Event &sdl_event) {
    abstract_widget::on_post_event(sdl_event);
}

void ekg::ui::frame_widget::on_update() {
    abstract_widget::on_update();

    if (this->p_scroll_embedded != nullptr) {
        this->p_scroll_embedded->clamp_scroll();
        this->p_scroll_embedded->reset_scroll();
    }
}

void ekg::ui::frame_widget::on_draw_refresh() {
    abstract_widget::on_draw_refresh();
    auto &rect {this->get_abs_rect()};
    auto &theme {ekg::theme()};

    if (this->p_scroll_embedded != nullptr) {
        this->p_scroll_embedded->clamp_scroll();
    }

    ekg::draw::bind_scissor(this->data->get_id());
    ekg::draw::sync_scissor(rect, this->data->get_parent_id());

    ekg::draw::rect(rect, theme.frame_background);
    ekg::draw::rect(this->docker_activy_drag.top, theme.frame_border);
    ekg::draw::rect(rect, theme.frame_outline, ekg::drawmode::outline);

    ekg::draw::bind_off_scissor();
}