#include "ekg/core/runtime.hpp"
#include "ekg/util/util_event.hpp"
#include "ekg/ui/frame/ui_frame.hpp"
#include "ekg/ui/frame/ui_frame_widget.hpp"
#include "ekg/ui/button/ui_button_widget.hpp"
#include "ekg/util/util_ui.hpp"

std::map<uint32_t, ekg::ui::abstract_widget*> ekg::swap::fast {};
std::map<uint32_t, ekg::ui::abstract_widget*> ekg::swap::continuous {};
std::map<uint32_t, ekg::ui::abstract_widget*> ekg::swap::target {};
std::vector<ekg::ui::abstract_widget*> ekg::swap::buffer {};

void ekg::swap::refresh() {
    ekg::swap::fast.clear();
    ekg::swap::continuous.clear();
    ekg::swap::target.clear();
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
}

void ekg::runtime::quit() {
    this->allocator.quit();
    this->theme_manager.quit();
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
    this->widget_id_focused = 0;
    ekg::ui::abstract_widget* focused_widget {nullptr};

    for (ekg::ui::abstract_widget* &widgets : this->list_widget) {
        if (widgets == nullptr || !widgets->data->is_alive()) {
            continue;
        }

        widgets->on_pre_event(sdl_event);

        if (widgets->flag.hovered && widgets->data->get_state() == ekg::state::visible) {
            this->widget_id_focused = widgets->data->get_id();
            focused_widget = widgets;
        }

        widgets->on_post_event(sdl_event);

        if (!widgets->flag.hovered) {
            widgets->on_event(sdl_event);
        }
    }

    if (focused_widget != nullptr) {
        focused_widget->on_pre_event(sdl_event);
        focused_widget->on_event(sdl_event);
        focused_widget->on_post_event(sdl_event);
    }

    if (ekg::was_pressed()) {
        this->widget_id_pressed_focused = this->widget_id_focused;
    } else if (ekg::was_released()) {
        this->widget_id_released_focused = this->widget_id_focused;
    }

    if (this->prev_widget_id_focused != this->widget_id_focused && this->widget_id_focused != 0) {
        this->swap_widget_id_focused = this->widget_id_focused;
        this->prev_widget_id_focused = this->widget_id_focused;

        ekg::dispatch(ekg::env::swap);
    }
}

void ekg::runtime::process_update() {
    this->input_manager.on_update();

    if (this->handler.should_poll()) {
        this->handler.on_update();
    }

    glViewport(0, 0, static_cast<float>(ekg::display::width), static_cast<float>(ekg::display::height));
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
        auto runtime = static_cast<ekg::runtime*>(data);

        for (ekg::ui::abstract_widget* &widgets : runtime->list_refresh_widget) {
            if (widgets == nullptr) {
                continue;
            }

            if (!widgets->data->is_alive()) {
                delete widgets;
                widgets = nullptr;
            }

            runtime->list_widget.push_back(widgets);
        }

        runtime->list_refresh_widget.clear();
    }, true});

    this->handler.dispatch(new ekg::cpu::event {"swap", this, [](void* data) {
        auto runtime = static_cast<ekg::runtime*>(data);

        if (runtime->swap_widget_id_focused == 0) {
            return;
        }

        for (ekg::ui::abstract_widget* &widgets : runtime->list_widget) {
            if (widgets == nullptr) {
                continue;
            }

            if (ekg::swap::fast[widgets->data->get_id()] != nullptr || ekg::swap::target[widgets->data->get_id()] != nullptr) {
                continue;
            }

            ekg::swap::fast.clear();
            ekg::stack(widgets, ekg::swap::fast);

            if (ekg::swap::fast[runtime->swap_widget_id_focused] != nullptr) {
                ekg::swap::target = ekg::swap::fast;
            } else {
                for (std::pair<uint32_t, ekg::ui::abstract_widget*> entry : ekg::swap::fast) {
                    ekg::swap::continuous[entry.first] = entry.second;
                }
            }
        }

        for (std::pair<uint32_t, ekg::ui::abstract_widget*> entry : ekg::swap::continuous) {
            if (entry.second == nullptr) {
                continue;
            }

            ekg::swap::buffer.push_back(entry.second);
        }

        for (std::pair<uint32_t, ekg::ui::abstract_widget*> entry : ekg::swap::target) {
            if (entry.second == nullptr) {
                continue;
            }

            ekg::swap::buffer.push_back(entry.second);
        }

        runtime->list_widget = ekg::swap::buffer;
        runtime->swap_widget_id_focused = 0;

        ekg::swap::refresh();
    }, true});

    this->handler.dispatch(new ekg::cpu::event {"reset", this, [](void* data) {
        auto runtime = static_cast<ekg::runtime*>(data);

        for (ekg::ui::abstract_widget* &widgets : runtime->list_reset_widget) {
            if (widgets == nullptr) {
                continue;
            }

            switch (widgets->data->get_type()) {
                case ekg::type::frame: {
                    auto ui = (ekg::ui::frame*) widgets->data;
                    auto widget = (ekg::ui::frame_widget*) widgets;

                    if (widget->rect != ekg::vec4 {ui->get_pos_initial(), ui->get_size_initial()}) {
                        widget->rect = {ui->get_pos_initial(), ui->get_size_initial()};
                    }

                    break;
                }
            }
        }

        runtime->list_reset_widget.clear();
    }, true});

    this->handler.dispatch(new ekg::cpu::event {"update", this, [](void* data) {
        auto runtime = static_cast<ekg::runtime*>(data);

        for (ekg::ui::abstract_widget* &widgets : runtime->list_update_widget) {
            if (widgets == nullptr) {
                continue;
            }

            widgets->on_reload();
        }

        runtime->list_update_widget.clear();
    }, true});

    this->handler.dispatch(new ekg::cpu::event {"redraw", this, [](void* data) {
        auto runtime = static_cast<ekg::runtime*>(data);

        runtime->allocator.invoke();
        runtime->f_renderer_big.blit("Widgets count: " + std::to_string(runtime->list_widget.size()), 10, 10, {255, 255, 255, 255});

        for (ekg::ui::abstract_widget* &widgets : runtime->list_widget) {
            if (widgets == nullptr) {
                continue;
            }

            if (widgets->data->is_alive() && widgets->data->get_state() == ekg::state::visible) {
                widgets->on_draw_refresh();
            }
        }

        runtime->allocator.revoke();
    }, true});
}

ekg::ui::abstract_widget *ekg::runtime::get_fast_widget_by_id(uint32_t id) {
    return this->map_widget[id];
}

void ekg::runtime::update_widget(ekg::ui::abstract_widget* widget) {
    if (widget != nullptr) {
        this->list_update_widget.push_back(widget);
        ekg::dispatch(ekg::env::update);
    }
}

ekg::service::input &ekg::runtime::get_service_input() {
    return this->input_manager;
}

ekg::service::theme &ekg::runtime::get_service_theme() {
    return this->theme_manager;
}

void ekg::runtime::prepare_ui_env() {
    ekg::log("creating ui fonts");

    this->f_renderer_small.font_size = 16;
    this->f_renderer_small.reload();
    this->f_renderer_small.bind_allocator(&this->allocator);

    this->f_renderer_normal.font_size = 18;
    this->f_renderer_normal.reload();
    this->f_renderer_normal.bind_allocator(&this->allocator);

    this->f_renderer_big.font_size = 24;
    this->f_renderer_big.reload();
    this->f_renderer_big.bind_allocator(&this->allocator);

    ekg::log("creating ui binds");

    this->input_manager.bind("frame-drag-activy", "mouse-left");
    this->input_manager.bind("frame-drag-activy", "finger-click");
    this->input_manager.bind("frame-resize-activy", "mouse-left");
    this->input_manager.bind("frame-resize-activy", "finger-click");
    this->input_manager.bind("button-activy", "lctrl+lshift+a");
    this->input_manager.bind("button-activy", "finger-click");
    this->input_manager.bind("popup-activy", "mouse-right");
    this->input_manager.bind("popup-activy", "finger-hold");
    this->input_manager.bind("popup-component-activy", "mouse-left");
    this->input_manager.bind("popup-component-activy", "finger-click");
    this->input_manager.bind("textbox-action-delete-left", "backspace");
    this->input_manager.bind("textbox-action-select-all", "lctrl+a");
    this->input_manager.bind("textbox-action-select-all", "finger-hold");
    this->input_manager.bind("textbox-action-select-all", "mouse-double-");
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
            break;
        }

        case type::button: {
            auto widget = new ekg::ui::button_widget();
            widget->data = ui;
            created_widget = widget;
            break;
        }
    }

    this->list_refresh_widget.push_back(created_widget);
    this->map_widget[created_widget->data->get_id()] = created_widget;
    ekg::log("created ui " + std::to_string(ui->get_id()));

    ekg::dispatch(ekg::env::refresh);
    ekg::dispatch(ekg::env::swap);
    ekg::dispatch(ekg::env::redraw);
}

void ekg::runtime::reset_widget(ekg::ui::abstract_widget *widget) {
    this->list_reset_widget.push_back(widget);
    ekg::dispatch(ekg::env::reset);
}