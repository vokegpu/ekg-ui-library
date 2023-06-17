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
#include "ekg/core/runtime.hpp"
#include "ekg/util/util_event.hpp"
#include "ekg/ui/frame/ui_frame.hpp"
#include "ekg/ui/frame/ui_frame_widget.hpp"
#include "ekg/ui/button/ui_button_widget.hpp"
#include "ekg/ui/label/ui_label_widget.hpp"
#include "ekg/ui/checkbox/ui_checkbox_widget.hpp"
#include "ekg/ui/slider/ui_slider_widget.hpp"
#include "ekg/ui/popup/ui_popup_widget.hpp"
#include "ekg/ui/textbox/ui_textbox_widget.hpp"
#include "ekg/ui/scroll/ui_scroll_widget.hpp"
#include "ekg/ui/scroll/ui_scroll.hpp"
#include "ekg/draw/draw.hpp"
#include "ekg/ekg.hpp"

#include <chrono>

ekg::stack ekg::swap::collect {};
ekg::stack ekg::swap::back {};
ekg::stack ekg::swap::front {};
std::vector<ekg::ui::abstract_widget*> ekg::swap::buffer {};
std::vector<uint64_t> ekg::swap::tooktimeanalyzingtelemtry {};

void ekg::swap::refresh() {
    ekg::swap::collect.clear();
    ekg::swap::back.clear();
    ekg::swap::front.clear();
    ekg::swap::buffer.clear();
}

void ekg::runtime::set_root(SDL_Window *sdl_win_root) {
    this->root = sdl_win_root;
}

SDL_Window *ekg::runtime::get_root() {
    return this->root;
}

ekg::timing &ekg::runtime::get_ui_timing() {
    return this->ui_timing;
}

void ekg::runtime::set_top_level(int32_t id) {
    this->top_level_id = id;
}

int32_t ekg::runtime::get_top_level() {
    return this->top_level_id;
}

void ekg::runtime::update_size_changed() {
    ekg::dispatch(ekg::env::redraw);

    this->layout_service.update_scale_factor();
    uint32_t font_size {static_cast<uint32_t>(ekg::clamp(static_cast<int32_t>(18.0f * this->layout_service.get_scale_factor()), 0, 256))};

    if (this->f_renderer_normal.font_size != font_size) {
        this->f_renderer_small.font_size = ekg::min(font_size - 4, 4);
        this->f_renderer_small.reload();

        this->f_renderer_normal.font_size = ekg::min(font_size, 8);
        this->f_renderer_normal.reload();

        this->f_renderer_big.font_size = ekg::min(font_size + 6, 12);
        this->f_renderer_big.reload();
    }

    for (ekg::ui::abstract_widget *&widgets : this->widget_list_map["all"]) {
        if (!widgets->data->has_parent()) {
            this->do_task_synclayout(widgets);
        } else {
            widgets->on_reload();
        }
    }
}

void ekg::runtime::init() {
    this->allocator.init();
    this->theme_service.init();

    if (FT_Init_FreeType(&ekg::draw::font_renderer::ft_library)) {
        ekg::log(ekg::log::WARNING) << "Failed to init FreeType library";
    }

    this->prepare_tasks();
    this->prepare_ui_env();
    this->layout_service.init();
}

void ekg::runtime::quit() {
    this->allocator.quit();
    this->theme_service.quit();
    this->layout_service.quit();
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
    this->input_service.on_event(sdl_event);

    bool pressed {ekg::input::pressed()};
    bool released {ekg::input::released()};

    if (pressed || released || ekg::input::motion()) {
        this->widget_id_focused = 0;
    }

    if (this->widget_absolute_activy != nullptr && this->widget_absolute_activy->flag.absolute) {
        this->widget_absolute_activy->on_pre_event(sdl_event);
        this->widget_absolute_activy->on_event(sdl_event);
        this->widget_absolute_activy->on_post_event(sdl_event);
        return;
    }

    this->widget_absolute_activy = nullptr;

    bool hovered {};
    bool found_absolute_widget {};

    ekg::ui::abstract_widget *focused_widget {};
    ekg::ui::abstract_widget *widgets {};
    int32_t prev_parent_master {};

    auto &all {this->widget_list_map["all"]};
    uint64_t size {all.size()};
    for (uint64_t it {}; it < size; it++) {
        widgets = all.at(it);
        if (widgets == nullptr || !widgets->data->is_alive()) {
            continue;
        }

        widgets->on_pre_event(sdl_event);

        /*
         * Text input like textbox and keyboard events should not update stack, instead just mouse events.
         */
        hovered = !(sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_KEYUP || sdl_event.type == SDL_TEXTINPUT) 
                && widgets->flag.hovered && widgets->data->get_state() == ekg::state::visible;

        if ((hovered || widgets->flag.absolute) && (!found_absolute_widget || (widgets->data->get_parent_id() == prev_parent_master && found_absolute_widget))) {
            this->widget_id_focused = widgets->data->get_id();
            focused_widget = widgets;
            found_absolute_widget = widgets->flag.absolute;
        }

        widgets->on_post_event(sdl_event);
        if (!hovered) {
            widgets->on_event(sdl_event);
        }

        if (widgets->data->has_children()) {
            prev_parent_master = widgets->data->get_id();
        }
    }

    ekg::hovered::type = ekg::type::abstract;
    ekg::hovered::id = this->widget_id_focused;

    if (focused_widget != nullptr) {
        focused_widget->on_pre_event(sdl_event);
        focused_widget->on_event(sdl_event);
        focused_widget->on_post_event(sdl_event);

        if (focused_widget->flag.absolute) {
            this->widget_absolute_activy = focused_widget;
        }

        ekg::hovered::type = focused_widget->data->get_type();
    } 

    if (pressed) {
        this->widget_id_pressed_focused = this->widget_id_focused;
        ekg::hovered::down = this->widget_id_focused;
        ekg::hovered::downtype = focused_widget != nullptr ? focused_widget->data->get_type() : ekg::type::abstract;
    } else if (released) {
        this->widget_id_released_focused = this->widget_id_focused;
        ekg::hovered::up = this->widget_id_focused;
        ekg::hovered::uptype = focused_widget != nullptr ? focused_widget->data->get_type() : ekg::type::abstract;
    }

    if (this->prev_widget_id_focused != this->widget_id_focused && this->widget_id_focused != 0 && (pressed || released)) {
        this->swap_widget_id_focused = this->widget_id_focused;
        this->prev_widget_id_focused = this->widget_id_focused;

        ekg::dispatch(ekg::env::swap);
        ekg::dispatch(ekg::env::redraw);
    }
}

void ekg::runtime::process_update() {
    this->input_service.on_update();
    bool {ekg::reach(this->ui_timing, 1000) && ekg::reset(this->ui_timing)};

    if (this->enable_high_priority_frequency) {
        auto &update = this->widget_list_map["update"];
        size_t counter {};

        for (ekg::ui::abstract_widget *&widgets : update) {
            if (widgets == nullptr || !widgets->is_high_frequency) {
                ++counter;
                continue;
            }

            widgets->on_update();
        }

        if (counter == update.size()) {
            this->enable_high_priority_frequency = false;
            update.clear();
        }
    }

    if (this->handler_service.should_poll()) {
        this->handler_service.on_update();
    }

    this->allocator.on_update();
    glViewport(0, 0, ekg::display::width, ekg::display::height);
}

void ekg::runtime::process_render() {
    this->allocator.draw();
}

ekg::gpu::allocator &ekg::runtime::get_gpu_allocator() {
    return this->allocator;
}

ekg::service::handler &ekg::runtime::get_service_handler() {
    return this->handler_service;
}

void ekg::runtime::prepare_tasks() {
    ekg::log() << "Preparing internal EKG core";

    this->handler_service.dispatch(new ekg::cpu::event {"refresh", this, [](void *p_data) {
        auto *runtime {static_cast<ekg::runtime*>(p_data)};
        auto &all = runtime->widget_list_map["all"];
        auto &refresh = runtime->widget_list_map["refresh"];
        bool should_call_gc {};

        for (ekg::ui::abstract_widget *&widgets : refresh) {
            if (widgets == nullptr || runtime->processed_widget_map[widgets->data->get_id()]) {
                continue;
            }

            if (widgets == nullptr || !widgets->data->is_alive()) {
                should_call_gc = true;
                continue;
            }

            all.push_back(widgets);
            runtime->processed_widget_map[widgets->data->get_id()] = true;
        }

        refresh.clear();
        runtime->processed_widget_map.clear();

        if (should_call_gc) {
            ekg::dispatch(ekg::env::gc);
        }
    }, ekg::event::alloc});

    this->handler_service.dispatch(new ekg::cpu::event {"swap", this, [](void *p_data) {
        auto *runtime {static_cast<ekg::runtime*>(p_data)};

        if (runtime->swap_widget_id_focused == 0) {
            return;
        }

        auto &all {runtime->widget_list_map["all"]};
        for (ekg::ui::abstract_widget *&widgets : all) {
            if (widgets == nullptr || ekg::swap::collect.registry[widgets->data->get_id()] || ekg::swap::front.registry[widgets->data->get_id()]) {
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

        all.clear();
        all.insert(all.end(), ekg::swap::back.ordered_list.begin(), ekg::swap::back.ordered_list.end());
        all.insert(all.end(), ekg::swap::front.ordered_list.begin(), ekg::swap::front.ordered_list.end());

        ekg::swap::refresh();
    }, ekg::event::alloc});

    this->handler_service.dispatch(new ekg::cpu::event {"reload", this, [](void *p_data) {
        auto *runtime {static_cast<ekg::runtime*>(p_data)};
        auto &reload = runtime->widget_list_map["reload"];

        for (ekg::ui::abstract_widget *&widgets : reload) {
            if (widgets == nullptr) {
                continue;
            }

            auto &sync_flags {widgets->data->get_sync()};
            if (ekg::bitwise::contains(sync_flags, (uint16_t) ekg::uisync::reset)) {
                ekg::bitwise::remove(sync_flags, (uint16_t) ekg::uisync::reset);

                switch (widgets->data->get_type()) {
                    case ekg::type::frame: {
                        auto ui {(ekg::ui::frame*) widgets->data};
                        auto pos {ui->get_pos_initial()};
                        auto size {ui->get_size_initial()};
                        auto &rect_ui {ui->ui()};

                        if (ui->widget() != ekg::vec4 {pos, size}) {
                            widgets->dimension.w = size.x;
                            widgets->dimension.h = size.y;

                            if (ui->get_parent_id() != 0) {
                                widgets->dimension.x = pos.x - widgets->parent->x;
                                widgets->dimension.y = pos.y - widgets->parent->y;
                            } else {
                                widgets->parent->x = pos.x;
                                widgets->parent->y = pos.y;
                            }
                        }

                        break;
                    }

                    default: {
                        break;
                    }
                }
            }

            if (ekg::bitwise::contains(sync_flags, (uint16_t) ekg::uisync::dimension)) {
                ekg::bitwise::remove(sync_flags, (uint16_t) ekg::uisync::dimension);

                auto &rect {widgets->data->ui()};
                switch (widgets->data->get_type()) {
                    case ekg::type::popup: {
                        widgets->dimension.w = rect.w;
                        widgets->parent->x = rect.x;
                        widgets->parent->y = rect.y;
                        break;
                    }

                    default: {
                        widgets->dimension.w = rect.w;
                        widgets->dimension.h = rect.h;

                        if (widgets->data->has_parent()) {
                            widgets->dimension.x = rect.x - widgets->parent->x;
                            widgets->dimension.y = rect.y - widgets->parent->y;
                        } else {
                            widgets->parent->x = rect.x;
                            widgets->parent->y = rect.y;
                        }

                        break;
                    }
                }
            }

            widgets->on_reload();
        }

        reload.clear();
    }, ekg::event::alloc});

    this->handler_service.dispatch(new ekg::cpu::event {"synclayout", this, [](void *p_data) {
        auto *runtime {static_cast<ekg::runtime*>(p_data)};
        auto &reload {runtime->widget_list_map["synclayout"]};

        for (ekg::ui::abstract_widget *&widgets : reload) {
            if (widgets == nullptr || runtime->processed_widget_map[widgets->data->get_id()]) {
                continue;
            }

            runtime->layout_service.process_scaled(widgets);
            runtime->processed_widget_map[widgets->data->get_id()] = true;
        }

        reload.clear();
        runtime->processed_widget_map.clear();
        ekg::dispatch(ekg::env::redraw);
    }, ekg::event::alloc});

    this->handler_service.dispatch(new ekg::cpu::event {"redraw", this, [](void *p_data) {
        auto *runtime {static_cast<ekg::runtime*>(p_data)};
        auto &all {runtime->widget_list_map["all"]};

        runtime->allocator.invoke();
        if (ekg::debug) {
            runtime->f_renderer_big.blit("Widgets count: " + std::to_string(all.size()), 10, 10, {1.0f, 1.0f, 1.0f, 1.0f});
        } else {
            runtime->f_renderer_big.blit(" ", 10, 10, {1.0f, 1.0f, 1.0f, 1.0f});
        }

        for (ekg::ui::abstract_widget *&widgets : all) {
            if (widgets != nullptr && widgets->data->is_alive() && widgets->data->get_state() == ekg::state::visible) {
                widgets->on_draw_refresh();
            }
        }

        runtime->allocator.revoke();
    }, ekg::event::alloc});

    this->handler_service.dispatch(new ekg::cpu::event {"gc", this, [](void *p_data) {
        auto *runtime {static_cast<ekg::runtime*>(p_data)};
        auto &all {runtime->widget_list_map["all"]};
        auto &high_frequency {runtime->widget_list_map["update"]};
        auto &redraw {runtime->widget_list_map["redraw"]};
        auto &allocator {runtime->get_gpu_allocator()};

        redraw.clear();
        high_frequency.clear();
        std::vector<ekg::ui::abstract_widget*> new_list {};

        for (ekg::ui::abstract_widget *&widgets : all) {
            if (widgets == nullptr || widgets->data == nullptr) {
                continue;
            }

            if (!widgets->data->is_alive()) {
                ekg::hovered::id = ekg::hovered::id == widgets->data->get_id() ? 0 : ekg::hovered::id;
                ekg::hovered::up = ekg::hovered::up == widgets->data->get_id() ? 0 : ekg::hovered::up;
                ekg::hovered::down = ekg::hovered::down == widgets->data->get_id() ? 0 : ekg::hovered::down;
                allocator.erase_scissor_by_id(widgets->data->get_id());
                delete widgets->data;
                delete widgets;
                continue;
            }

            if (widgets->is_high_frequency) {
                high_frequency.push_back(widgets);
            }

            if (widgets->data->get_state() == ekg::state::visible) {
                redraw.push_back(widgets);
            }

            new_list.push_back(widgets);
        }

        all = new_list;
    }, ekg::event::alloc});
}

ekg::ui::abstract_widget *ekg::runtime::get_fast_widget_by_id(int32_t id) {
    /* widget ID 0 is defined as none, or be, ID token accumulation start with 1 and not 0 */
    return id ? this->widget_map[id] : nullptr;
}

void ekg::runtime::do_task_reload(ekg::ui::abstract_widget *widget) {
    if (widget != nullptr) {
        this->widget_list_map["reload"].push_back(widget);
        ekg::dispatch(ekg::env::reload);
    }
}

ekg::service::input &ekg::runtime::get_service_input() {
    return this->input_service;
}

ekg::service::theme &ekg::runtime::get_service_theme() {
    return this->theme_service;
}

ekg::service::layout &ekg::runtime::get_service_layout() {
    return this->layout_service;
}

void ekg::runtime::prepare_ui_env() {
    ekg::log() << "Preparing internal user interface environment";

    this->widget_list_map["all"] = {};
    this->widget_list_map["refresh"] = {};
    this->widget_list_map["reload"] = {};
    this->widget_list_map["synclayout"] = {};
    this->widget_list_map["redraw"] = {};
    this->widget_list_map["scissor"] = {};
    this->widget_list_map["update"] = {};

    this->f_renderer_small.font_size = 16;
    this->f_renderer_small.reload();
    this->f_renderer_small.bind_allocator(&this->allocator);

    this->f_renderer_normal.font_size = 18;
    this->f_renderer_normal.reload();
    this->f_renderer_normal.bind_allocator(&this->allocator);

    this->f_renderer_big.font_size = 24;
    this->f_renderer_big.reload();
    this->f_renderer_big.bind_allocator(&this->allocator);

    ekg::log() << "Registering user interface input bindings";

    this->input_service.bind("frame-drag-activy", "mouse-1");
    this->input_service.bind("frame-drag-activy", "finger-click");
    this->input_service.bind("frame-resize-activy", "mouse-1");
    this->input_service.bind("frame-resize-activy", "finger-click");

    this->input_service.bind("button-activy", "mouse-1");
    this->input_service.bind("button-activy", "finger-click");

    this->input_service.bind("checkbox-activy", "mouse-1");
    this->input_service.bind("checkbox-activy", "finger-click");

    this->input_service.bind("popup-activy", "mouse-1");
    this->input_service.bind("popup-activy", "finger-click");

    this->input_service.bind("textbox-action-select-all", "lctrl+a");
    this->input_service.bind("textbox-action-select-all", "finger-hold");
    this->input_service.bind("textbox-action-select-all", "mouse-1-double");
    this->input_service.bind("textbox-action-delete-left", "backspace");
    this->input_service.bind("textbox-action-delete-right", "delete");
    this->input_service.bind("textbox-action-break-line", "return");
    this->input_service.bind("textbox-action-break-line", "keypad enter");
    this->input_service.bind("textbox-action-up", "up");
    this->input_service.bind("textbox-action-down", "down");
    this->input_service.bind("textbox-action-right", "right");
    this->input_service.bind("textbox-action-left", "left");
    this->input_service.bind("textbox-activy", "mouse-1");
    this->input_service.bind("textbox-activy", "finger-click");

    this->input_service.bind("slider-activy", "mouse-1");
    this->input_service.bind("slider-activy", "finger-click");
    this->input_service.bind("slider-bar-increase", "mouse-wheel-up");
    this->input_service.bind("slider-bar-decrease", "mouse-wheel-down");

    this->input_service.bind("scrollbar-drag", "mouse-1");
    this->input_service.bind("scrollbar-drag", "finger-click");
    this->input_service.bind("scrollbar-scroll", "mouse-wheel");
    this->input_service.bind("scrollbar-scroll", "finger-swipe");
}

void ekg::runtime::gen_widget(ekg::ui::abstract *ui) {
    ui->set_id(++this->token_id);

    this->swap_widget_id_focused = ui->get_id();
    ekg::ui::abstract_widget *created_widget {nullptr};

    bool update_layout {};
    bool append_group {};

    switch (ui->get_type()) {
    case ekg::type::abstract: {
        auto *widget {new ekg::ui::abstract_widget()};
        widget->data = ui;
        created_widget = widget;
        break;
    }
    case ekg::type::frame: {
        auto *widget {new ekg::ui::frame_widget()};
        widget->is_scissor_refresh = true;
        widget->data = ui;
        update_layout = true;
        created_widget = widget;
        this->current_bind_group = ui;
        ui->reset();
        break;
    }
    case ekg::type::button: {
        auto *widget {new ekg::ui::button_widget()};
        widget->data = ui;
        created_widget = widget;
        append_group = true;
        break;
    }
    case ekg::type::label: {
        auto *widget {new ekg::ui::label_widget()};
        widget->data = ui;
        created_widget = widget;
        append_group = true;
        break;
    }
    case ekg::type::checkbox: {
        auto *widget {new ekg::ui::checkbox_widget()};
        widget->data = ui;
        created_widget = widget;
        append_group = true;
        break;
    }
    case ekg::type::slider: {
        auto *widget {new ekg::ui::slider_widget()};
        widget->data = ui;
        created_widget = widget;
        append_group = true;
        break;
    }
    case ekg::type::popup: {
        auto *widget {new ekg::ui::popup_widget()};
        widget->data = ui;
        created_widget = widget;
        update_layout = false;
        break;
    }
    case ekg::type::textbox: {
        auto *widget {new ekg::ui::textbox_widget()};
        widget->data = ui;
        created_widget = widget;
        append_group = true;
        break;
    }
    case ekg::type::scroll: {
        auto *widget {new ekg::ui::scroll_widget()};
        widget->data = ui;
        created_widget = widget;
        append_group = true;
        break;
    }
    default:
        break;
    }

    this->widget_map[ui->get_id()] = created_widget;
    this->do_task_refresh(created_widget);
    this->do_task_reload(created_widget);

    if (append_group && this->current_bind_group != nullptr) {
        this->current_bind_group->add_child(ui->get_id());
    }

    if (update_layout) {
        this->do_task_synclayout(created_widget);
    }

    ekg::dispatch(ekg::env::swap);
}

void ekg::runtime::do_task_synclayout(ekg::ui::abstract_widget *widget) {
    if (widget == nullptr) {
        return;
    }

    bool is_group {widget->data->get_type() == ekg::type::frame};
    bool check_parent {is_group == false && widget->data->has_parent()};

    if (check_parent) {
        widget = this->get_fast_widget_by_id(widget->data->get_parent_id());
    }

    if (widget != nullptr) {
        this->widget_list_map["synclayout"].push_back(widget);
        ekg::dispatch(ekg::env::synclayout);
    }
}

void ekg::runtime::do_task_refresh(ekg::ui::abstract_widget *widget) {
    if (widget == nullptr) {
        return;
    }

    this->widget_list_map["refresh"].push_back(widget);
    ekg::dispatch(ekg::env::refresh);
    ekg::dispatch(ekg::env::redraw);
}

void ekg::runtime::end_group_flag() {
    this->current_bind_group = nullptr;
}

void ekg::runtime::erase(int32_t id) {
    auto &all {this->widget_list_map["all"]};

    for (size_t it {}; it < all.size(); it++) {
        auto &widget {all[it]};

        if (widget != nullptr && widget->data->get_id() == id) {
            this->widget_map.erase(widget->data->get_id());
            all.erase(all.begin() + it);
            break;
        }
    }
}

void ekg::runtime::set_update_high_frequency(ekg::ui::abstract_widget *widget) {
    if (widget != nullptr && !widget->is_high_frequency) {
        this->enable_high_priority_frequency = true;
        auto &update {this->widget_list_map["update"]};
        bool contains {};

        for (ekg::ui::abstract_widget *&widgets : update) {
            contains = widgets == widget;
            if (contains) {
                break;
            }
        }

        if (!contains) {
            update.push_back(widget);
            widget->is_high_frequency = true;
        }
    }
}