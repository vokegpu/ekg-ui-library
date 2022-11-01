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
 * @VokeGpu 2022 all rights reserved.
 */

#include "ekg/core/runtime.hpp"
#include "ekg/util/util_event.hpp"
#include "ekg/ui/frame/ui_frame.hpp"
#include "ekg/ui/frame/ui_frame_widget.hpp"
#include "ekg/ui/button/ui_button_widget.hpp"
#include "ekg/ui/label/ui_label_widget.hpp"
#include "ekg/ui/checkbox/ui_checkbox_widget.hpp"

ekg::stack ekg::swap::collect {};
ekg::stack ekg::swap::back {};
ekg::stack ekg::swap::front {};
std::vector<ekg::ui::abstract_widget*> ekg::swap::buffer {};

void ekg::swap::refresh() {
    ekg::swap::collect.clear();
    ekg::swap::back.clear();
    ekg::swap::front.clear();
    ekg::swap::buffer.clear();
}

void ekg::runtime::set_root(SDL_Window *sdl_win_root) {
    this->root = sdl_win_root;
}

SDL_Window* ekg::runtime::get_root() {
    return this->root;
}

void ekg::runtime::init() {
    this->allocator.init();
    this->theme_manager.init();

    if (FT_Init_FreeType(&ekg::draw::font_renderer::ft_library)) {
        ekg::log("could not init FreeType");
    }

    this->prepare_tasks();
    this->prepare_ui_env();
    this->layout_manager.init();
}

void ekg::runtime::quit() {
    this->allocator.quit();
    this->theme_manager.quit();
    this->layout_manager.quit();
}

ekg::draw::font_renderer &ekg::runtime::get_f_renderer_small() {
    return this->f_renderer_small;
}

ekg::draw::font_renderer &ekg::runtime::get_f_renderer_normal() {
    return this->f_renderer_normal;
}

ekg::draw::font_renderer &ekg::runtime::get_f_renderer_big() {
    return this->f_renderer_big;
}

void ekg::runtime::process_event(SDL_Event &sdl_event) {
    this->input_manager.on_event(sdl_event);

    bool pressed {ekg::input::pressed()};
    bool released {ekg::input::released()};

    if (pressed || released || ekg::input::motion()) {
        this->widget_id_focused = 0;
    }

    ekg::ui::abstract_widget* focused_widget {nullptr};
    bool hovered {};
    bool found_absolute_widget {};

    for (ekg::ui::abstract_widget* &widgets : this->loaded_widget_list) {
        if (widgets == nullptr || !widgets->data->is_alive()) {
            continue;
        }

        widgets->on_pre_event(sdl_event);
        hovered = widgets->flag.hovered && widgets->data->get_state() == ekg::state::visible;

        if ((hovered || widgets->flag.absolute) && !found_absolute_widget) {
            this->widget_id_focused = widgets->data->get_id();
            focused_widget = widgets;
            found_absolute_widget = widgets->flag.absolute;
        }

        widgets->on_post_event(sdl_event);

        if (!hovered || found_absolute_widget) {
            widgets->on_event(sdl_event);
        }
    }

    if (focused_widget != nullptr) {
        focused_widget->on_pre_event(sdl_event);
        focused_widget->on_event(sdl_event);
        focused_widget->on_post_event(sdl_event);
    }

    if (pressed) {
        this->widget_id_pressed_focused = this->widget_id_focused;
    } else if (released) {
        this->widget_id_released_focused = this->widget_id_focused;
    }

    if (this->prev_widget_id_focused != this->widget_id_focused && this->widget_id_focused != 0 && (pressed || released)) {
        this->swap_widget_id_focused = this->widget_id_focused;
        this->prev_widget_id_focused = this->widget_id_focused;

        ekg::dispatch(ekg::env::swap);
        ekg::dispatch(ekg::env::redraw);
    }
}

void ekg::runtime::process_update() {
    this->input_manager.on_update();

    if (this->handler.should_poll()) {
        this->handler.on_update();
    }

    glViewport(0, 0, ekg::display::width, ekg::display::height);
}

void ekg::runtime::process_render() {
    this->allocator.draw();
}

ekg::gpu::allocator &ekg::runtime::get_gpu_allocator() {
    return this->allocator;
}

ekg::service::handler &ekg::runtime::get_service_handler() {
    return this->handler;
}

void ekg::runtime::prepare_tasks() {
    ekg::log("creating task events");

    this->handler.dispatch(new ekg::cpu::event {"refresh", this, [](void* data) {
        auto runtime {static_cast<ekg::runtime*>(data)};

        for (ekg::ui::abstract_widget* &widgets : runtime->to_refresh_widgets) {
            if (widgets == nullptr) {
                continue;
            }

            if (!widgets->data->is_alive()) {
                delete widgets;
                widgets = nullptr;
            }

            runtime->loaded_widget_list.push_back(widgets);
        }

        runtime->to_refresh_widgets.clear();
    }, ekg::event::alloc});

    this->handler.dispatch(new ekg::cpu::event {"swap", this, [](void* data) {
        auto runtime {static_cast<ekg::runtime*>(data)};

        if (runtime->swap_widget_id_focused == 0) {
            return;
        }

        for (ekg::ui::abstract_widget* &widgets : runtime->loaded_widget_list) {
            if (widgets == nullptr) {
                continue;
            }

            if (ekg::swap::collect.registry[widgets->data->get_id()] || ekg::swap::front.registry[widgets->data->get_id()]) {
                continue;
            }

            ekg::swap::collect.clear();
            ekg::push_back_stack(widgets, ekg::swap::collect);

            if (ekg::swap::collect.registry[runtime->swap_widget_id_focused]) {
                ekg::swap::front.registry.insert(ekg::swap::collect.registry.begin(), ekg::swap::collect.registry.end());
                ekg::swap::front.ordered_list.insert(ekg::swap::front.ordered_list.end(), ekg::swap::collect.ordered_list.begin(), ekg::swap::collect.ordered_list.end());
            } else {
                ekg::swap::back.registry.insert(ekg::swap::collect.registry.begin(), ekg::swap::collect.registry.end());
                ekg::swap::back.ordered_list.insert(ekg::swap::back.ordered_list.end(), ekg::swap::collect.ordered_list.begin(), ekg::swap::collect.ordered_list.end());
            }
        }

        runtime->swap_widget_id_focused = 0;
        runtime->loaded_widget_list.clear();

        for (ekg::ui::abstract_widget* &widget : ekg::swap::back.ordered_list) {
            if (widget == nullptr) {
                continue;
            }

            runtime->loaded_widget_list.push_back(widget);
        }

        for (ekg::ui::abstract_widget* &widget : ekg::swap::front.ordered_list) {
            if (widget == nullptr) {
                continue;
            }

            runtime->loaded_widget_list.push_back(widget);
        }

        ekg::swap::refresh();
    }, ekg::event::alloc});

    this->handler.dispatch(new ekg::cpu::event {"reset", this, [](void* data) {
        auto runtime {static_cast<ekg::runtime*>(data)};

        for (ekg::ui::abstract_widget* &widgets : runtime->to_reset_widgets) {
            if (widgets == nullptr || runtime->processed_widgets_map[widgets->data->get_id()]) {
                continue;
            }

            switch (widgets->data->get_type()) {
                case ekg::type::frame: {
                    auto ui {(ekg::ui::frame*) widgets->data};
                    auto pos {ui->get_pos_initial()};
                    auto size {ui->get_size_initial()};

                    if (ui->widget() != ekg::vec4 {pos, size}) {
                        widgets->layout.w = size.x;
                        widgets->layout.h = size.y;

                        if (ui->get_parent_id() != 0) {
                            widgets->layout.x = pos.x - widgets->parent->x;
                            widgets->layout.y = pos.y - widgets->parent->y;
                        } else {
                            widgets->parent->x = pos.x;
                            widgets->parent->y = pos.y;
                        }
                    }

                    break;
                }
            }
        }

        runtime->to_reset_widgets.clear();
        runtime->processed_widgets_map.clear();
    }, ekg::event::alloc});

    this->handler.dispatch(new ekg::cpu::event {"reload", this, [](void* data) {
        auto runtime {static_cast<ekg::runtime*>(data)};

        for (ekg::ui::abstract_widget* &widget : runtime->to_reload_widgets) {
            if (widget == nullptr) {
                continue;
            }

            if (widget->data->should_sync_with_ui()) {
                widget->data->set_sync_with_ui(false);
                auto rect = widget->data->ui();

                widget->layout.w = rect.w;
                widget->layout.h = rect.h;

                if (widget->data->get_parent_id() != 0) {
                    widget->layout.x = rect.x - widget->parent->x;
                    widget->layout.y = rect.y - widget->parent->y;
                } else {
                    widget->parent->x = rect.x;
                    widget->parent->y = rect.y;
                }
            }

            widget->on_reload();
            runtime->processed_widgets_map[widget->data->get_id()] = true;
        }

        runtime->to_reload_widgets.clear();
        runtime->processed_widgets_map.clear();
    }, ekg::event::alloc});

    this->handler.dispatch(new ekg::cpu::event {"synclayout", this, [](void* data) {
        auto runtime {static_cast<ekg::runtime*>(data)};
        // todo fix the issue with sync layout offset.

        for (ekg::ui::abstract_widget* &widget : runtime->to_sync_layout_widgets) {
            if (widget == nullptr || runtime->processed_widgets_map[widget->data->get_id()]) {
                continue;
            }

            runtime->layout_manager.process_scaled(widget);
            runtime->processed_widgets_map[widget->data->get_id()] = true;
        }

        runtime->to_sync_layout_widgets.clear();
        runtime->processed_widgets_map.clear();
    }, ekg::event::alloc});

    this->handler.dispatch(new ekg::cpu::event {"redraw", this, [](void* data) {
        auto runtime {static_cast<ekg::runtime*>(data)};

        runtime->allocator.invoke();
        runtime->f_renderer_big.blit("Widgets count: " + std::to_string(runtime->loaded_widget_list.size()), 10, 10, {255, 255, 255, 255});

        for (ekg::ui::abstract_widget* &widgets : runtime->loaded_widget_list) {
            if (widgets == nullptr) {
                continue;
            }

            if (widgets->data->is_alive() && widgets->data->get_state() == ekg::state::visible) {
                widgets->on_draw_refresh();
            }
        }

        runtime->allocator.revoke();
    }, ekg::event::alloc});
}

ekg::ui::abstract_widget *ekg::runtime::get_fast_widget_by_id(uint32_t id) {
    return this->widgets_map[id];
}

void ekg::runtime::reload_widget(ekg::ui::abstract_widget* widget) {
    if (widget != nullptr) {
        this->to_reload_widgets.push_back(widget);
        ekg::dispatch(ekg::env::reload);
    }
}

ekg::service::input &ekg::runtime::get_service_input() {
    return this->input_manager;
}

ekg::service::theme &ekg::runtime::get_service_theme() {
    return this->theme_manager;
}

void ekg::runtime::prepare_ui_env() {
    ekg::log("creating widget fonts");

    this->f_renderer_small.font_size = 16;
    this->f_renderer_small.reload();
    this->f_renderer_small.bind_allocator(&this->allocator);

    this->f_renderer_normal.font_size = 18;
    this->f_renderer_normal.reload();
    this->f_renderer_normal.bind_allocator(&this->allocator);

    this->f_renderer_big.font_size = 24;
    this->f_renderer_big.reload();
    this->f_renderer_big.bind_allocator(&this->allocator);

    ekg::log("creating widget binds");

    this->input_manager.bind("frame-drag-activy", "mouse-left");
    this->input_manager.bind("frame-drag-activy", "finger-click");
    this->input_manager.bind("frame-resize-activy", "mouse-left");
    this->input_manager.bind("frame-resize-activy", "finger-click");
    this->input_manager.bind("button-activy", "mouse-left");
    this->input_manager.bind("button-activy", "finger-click");
    this->input_manager.bind("checkbox-activy", "mouse-left");
    this->input_manager.bind("checkbox-activy", "finger-click");
    this->input_manager.bind("popup-activy", "mouse-right");
    this->input_manager.bind("popup-activy", "finger-hold");
    this->input_manager.bind("popup-component-activy", "mouse-left");
    this->input_manager.bind("popup-component-activy", "finger-click");
    this->input_manager.bind("textbox-action-delete-left", "backspace");
    this->input_manager.bind("textbox-action-select-all", "lctrl+a");
    this->input_manager.bind("textbox-action-select-all", "finger-hold");
    this->input_manager.bind("textbox-action-select-all", "mouse-left-double");
    this->input_manager.bind("textbox-action-delete-right", "delete");
    this->input_manager.bind("textbox-action-break-line", "return");
    this->input_manager.bind("textbox-action-up", "up");
    this->input_manager.bind("textbox-action-down", "down");
    this->input_manager.bind("textbox-action-down", "right");
    this->input_manager.bind("textbox-action-down", "left");
}

void ekg::runtime::create_ui(ekg::ui::abstract* ui) {
    ui->set_id(++this->token_id);
    this->swap_widget_id_focused = ui->get_id();
    ekg::ui::abstract_widget* created_widget {nullptr};
    bool is_group {};

    switch (ui->get_type()) {
        case type::abstract: {
            auto widget = new ekg::ui::abstract_widget();
            widget->data = ui;
            created_widget = widget;
            break;
        }

        case type::frame: {
            auto widget = new ekg::ui::frame_widget();
            widget->data = ui;
            created_widget = widget;
            this->current_bind_group = created_widget;
            is_group = true;
            break;
        }

        case type::button: {
            auto widget = new ekg::ui::button_widget();
            widget->data = ui;
            created_widget = widget;
            break;
        }

        case type::label: {
            auto widget = new ekg::ui::label_widget();
            widget->data = ui;
            created_widget = widget;
            break;
        }

        case type::checkbox: {
            auto widget = new ekg::ui::checkbox_widget();
            widget->data = ui;
            created_widget = widget;
            break;
        }
    }

    this->to_refresh_widgets.push_back(created_widget);
    this->widgets_map[ui->get_id()] = created_widget;
    this->reset_widget(created_widget);
    this->reload_widget(created_widget);

    if (!is_group && this->current_bind_group != nullptr) {
        this->current_bind_group->data->parent(ui->get_id());
    } else if (is_group) {
        this->sync_layout_widget(created_widget);
    }

    ekg::log("created widget " + std::to_string(ui->get_id()));
    ekg::dispatch(ekg::env::refresh);
    ekg::dispatch(ekg::env::swap);
}

void ekg::runtime::reset_widget(ekg::ui::abstract_widget *widget) {
    if (widget != nullptr) {
        this->to_reset_widgets.push_back(widget);
        ekg::dispatch(ekg::env::reset);
    }
}

void ekg::runtime::sync_layout_widget(ekg::ui::abstract_widget *widget) {
    if (widget == nullptr) {
        return;
    }

    bool is_group  {widget->data->get_type() == ekg::type::frame};
    if (!is_group) {
        widget = this->get_fast_widget_by_id(widget->data->get_parent_id());
    }

    if (is_group || (!is_group || widget != nullptr)) {
        this->to_sync_layout_widgets.push_back(widget);
        ekg::dispatch(ekg::env::synclayout);
    }
}

void ekg::runtime::reset_group_instance() {
    this->current_bind_group = nullptr;
}
