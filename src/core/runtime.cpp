/*
 * MIT License
 * 
 * Copyright (c) 2022-2023 Rina Wilk / vokegpu@gmail.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <chrono>

#include "ekg/core/runtime.hpp"
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
#include "ekg/os/ekg_opengl.hpp"

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

void ekg::runtime::update_size_changed() {
    ekg::dispatch(ekg::env::redraw);

    this->service_layout.update_scale_factor();
    uint32_t font_size {static_cast<uint32_t>(ekg::clamp(static_cast<int32_t>(18.0f * this->service_layout.get_scale_factor()), 0, 256))};

    if (this->f_renderer_normal.font_size != font_size) {
        this->f_renderer_small.font_size = ekg::min(font_size - 4, 4);
        this->f_renderer_small.reload();

        this->f_renderer_normal.font_size = ekg::min(font_size, 8);
        this->f_renderer_normal.reload();

        this->f_renderer_big.font_size = ekg::min(font_size + 6, 12);
        this->f_renderer_big.reload();
    }

    for (ekg::ui::abstract_widget *&p_widgets : this->widget_list_map["all"]) {
        p_widgets->on_reload();
        if (!p_widgets->p_data->has_parent() && p_widgets->p_data->has_children()) {
            this->do_task_synclayout(p_widgets);
        }
    }
}

void ekg::runtime::init() {
    if (FT_Init_FreeType(&ekg::draw::font_renderer::ft_library)) {
        ekg::log() << "Error: Failed to init FreeType library";
    }

    this->gpu_allocator.init();
    this->prepare_tasks();
    this->prepare_ui_env();
    this->service_layout.init();
    this->service_theme.init();
}

void ekg::runtime::quit() {
    this->gpu_allocator.quit();
    this->service_theme.quit();
    this->service_layout.quit();
}

void ekg::runtime::process_event(SDL_Event &sdl_event) {
    this->service_input.on_event(sdl_event);

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
    bool first_absolute {};

    ekg::ui::abstract_widget *p_widget_focused {};
    int32_t widgets_id {};

    for (ekg::ui::abstract_widget *&p_widgets : this->widget_list_map["all"]) {
        if (p_widgets == nullptr || !p_widgets->p_data->is_alive()) {
            continue;
        }

        p_widgets->on_pre_event(sdl_event);

        /*
         * Text input like textbox and keyboard events should not update stack, instead just mouse events.
         */
        hovered = !(sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_KEYUP || sdl_event.type == SDL_TEXTINPUT) 
                && p_widgets->flag.hovered && p_widgets->p_data->get_state() == ekg::state::visible;
        if (hovered) {
            this->widget_id_focused = p_widgets->p_data->get_id();

            p_widget_focused = p_widgets;
            first_absolute = false;
        }

        /*
         * The absolute/top-level system check for the first absolute fired widget,
         * everytime a widget is hovered then reset again the checking state.
         *
         * The order of scrollable widgets like scroll widget is not sequentially,
         * e.g, the mouse is hovering some children of frame 2 and absolute widget scroll from frame 2 is fired:
         * frame 1           // hovered, check for the first absolute
         *
         * frame 2 (frame 1) // hovered, then reset and find for the first absolute again
         * widgets...        // hovering some of children widgets, then reset over again
         * scroll (frame 2)  // found the first absolute, target it
         * 
         * frame 3 (frame 1) // not hovering, then does not reset any absolute checking
         * ...
         *
         * scroll (frame 1)  // do not target this fired absolute widget.
         * end of e.g.
         */
        if (p_widgets->flag.absolute && !first_absolute) {
            p_widget_focused = p_widgets;
            first_absolute = true;
        }

        p_widgets->on_post_event(sdl_event);
        if (!hovered && !p_widgets->flag.absolute) {
            p_widgets->on_event(sdl_event);
        }
    }

    ekg::hovered::type = ekg::type::abstract;
    ekg::hovered::id = this->widget_id_focused;

    if (p_widget_focused != nullptr) {
        p_widget_focused->on_pre_event(sdl_event);
        p_widget_focused->on_event(sdl_event);
        p_widget_focused->on_post_event(sdl_event);

        if (p_widget_focused->flag.absolute) {
            this->widget_absolute_activy = p_widget_focused;
        }

        ekg::hovered::type = p_widget_focused->p_data->get_type();
    } 

    if (pressed) {
        this->widget_id_pressed_focused = this->widget_id_focused;
        ekg::hovered::down = this->widget_id_focused;
        ekg::hovered::downtype = p_widget_focused != nullptr ? p_widget_focused->p_data->get_type() : ekg::type::abstract;
    } else if (released) {
        this->widget_id_released_focused = this->widget_id_focused;
        ekg::hovered::up = this->widget_id_focused;
        ekg::hovered::uptype = p_widget_focused != nullptr ? p_widget_focused->p_data->get_type() : ekg::type::abstract;
    }

    if (this->prev_widget_id_focused != this->widget_id_focused && this->widget_id_focused != 0 && (pressed || released)) {
        this->swap_widget_id_focused = this->widget_id_focused;
        this->prev_widget_id_focused = this->widget_id_focused;

        ekg::dispatch(ekg::env::swap);
        ekg::dispatch(ekg::env::redraw);
    }
}

void ekg::runtime::process_update() {
    this->service_input.on_update();
    bool {ekg::reach(this->ui_timing, 1000) && ekg::reset(this->ui_timing)};

    if (this->enable_high_priority_frequency) {
        auto &update = this->widget_list_map["update"];
        size_t counter {};

        for (ekg::ui::abstract_widget *&p_widgets : update) {
            if (p_widgets == nullptr || !p_widgets->is_high_frequency) {
                ++counter;
                continue;
            }

            p_widgets->on_update();
        }

        if (counter == update.size()) {
            this->enable_high_priority_frequency = false;
            update.clear();
        }
    }

    this->service_handler.on_update();
    this->gpu_allocator.on_update();

    ekg::log::flush();
}

void ekg::runtime::request_redraw_gui() {
    this->should_re_batch_gui = true;
}

void ekg::runtime::process_render() {
    if (this->should_re_batch_gui) {
        this->should_re_batch_gui = false;

        auto &all {this->widget_list_map["all"]};
        this->gpu_allocator.invoke();

        for (ekg::ui::abstract_widget *&p_widgets : all) {
            if (p_widgets != nullptr && p_widgets->p_data->is_alive() && p_widgets->p_data->get_state() == ekg::state::visible) {
                p_widgets->on_draw_refresh();
            }
        }

        this->gpu_allocator.revoke();
    }

    glViewport(0, 0, ekg::display::width, ekg::display::height);
    this->gpu_allocator.draw();
}

void ekg::runtime::prepare_tasks() {
    ekg::log() << "Preparing internal EKG core";

    this->service_handler.init_multi_thread_task_thread();

    this->service_handler.allocate() = {
        .p_tag      = "refresh",
        .p_callback = this,
        .function   = [](void *p_callback) {
            auto *runtime {static_cast<ekg::runtime*>(p_callback)};
            auto &all = runtime->widget_list_map["all"];
            auto &refresh = runtime->widget_list_map["refresh"];
            bool should_call_gc {};

            for (ekg::ui::abstract_widget *&p_widgets : refresh) {
                if (p_widgets == nullptr || runtime->processed_widget_map[p_widgets->p_data->get_id()]) {
                    continue;
                }

                if (p_widgets == nullptr || !p_widgets->p_data->is_alive()) {
                    should_call_gc = true;
                    continue;
                }

                all.push_back(p_widgets);
                runtime->processed_widget_map[p_widgets->p_data->get_id()] = true;
            }

            refresh.clear();
            runtime->processed_widget_map.clear();

            if (should_call_gc) {
                ekg::dispatch(ekg::env::gc);
            }
        }
    };

    this->service_handler.allocate() = {
        .p_tag      = "swap",
        .p_callback = this,
        .function   = [](void *p_callback) {
            auto *runtime {static_cast<ekg::runtime*>(p_callback)};

            if (runtime->swap_widget_id_focused == 0) {
                return;
            }

            auto &all {runtime->widget_list_map["all"]};
            for (ekg::ui::abstract_widget *&p_widgets : all) {
                if (p_widgets == nullptr || ekg::swap::collect.registry[p_widgets->p_data->get_id()] || ekg::swap::front.registry[p_widgets->p_data->get_id()]) {
                    continue;
                }

                ekg::swap::collect.clear();
                ekg::push_back_stack(p_widgets, ekg::swap::collect);

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
        }
    };

    this->service_handler.allocate() = {
        .p_tag      = "reload",
        .p_callback = this,
        .function   = [](void *p_callback) {
            auto *runtime {static_cast<ekg::runtime*>(p_callback)};
            auto &reload = runtime->widget_list_map["reload"];

            ekg::vec4 rect {};

            for (ekg::ui::abstract_widget *&p_widgets : reload) {
                if (p_widgets == nullptr) {
                    continue;
                }

                auto &sync_flags {p_widgets->p_data->get_sync()};
                if (ekg::bitwise::contains(sync_flags, (uint16_t) ekg::ui_sync::reset)) {
                    ekg::bitwise::remove(sync_flags, (uint16_t) ekg::ui_sync::reset);

                    switch (p_widgets->p_data->get_type()) {
                        case ekg::type::frame: {
                            auto p_ui {(ekg::ui::frame*) p_widgets->p_data};
                            auto pos {p_ui->get_pos_initial()};
                            auto size {p_ui->get_size_initial()};
                            auto &rect_ui {p_ui->ui()};

                            rect.x = pos.x;
                            rect.y = pos.y;
                            rect.z = size.x;
                            rect.w = size.y;

                            if (p_ui->widget() != rect) {
                                p_widgets->dimension.w = size.x;
                                p_widgets->dimension.h = size.y;

                                if (p_ui->get_parent_id() != 0) {
                                    p_widgets->dimension.x = pos.x - p_widgets->p_parent->x;
                                    p_widgets->dimension.y = pos.y - p_widgets->p_parent->y;
                                } else {
                                    p_widgets->p_parent->x = pos.x;
                                    p_widgets->p_parent->y = pos.y;
                                }
                            }

                            break;
                        }

                        default: {
                            break;
                        }
                    }
                }

                if (ekg::bitwise::contains(sync_flags, (uint16_t) ekg::ui_sync::dimension)) {
                    ekg::bitwise::remove(sync_flags, (uint16_t) ekg::ui_sync::dimension);

                    auto &rect {p_widgets->p_data->ui()};
                    switch (p_widgets->p_data->get_category()) {
                        case ekg::category::top_level: {
                            p_widgets->dimension.w = rect.w;
                            p_widgets->p_parent->x = rect.x;
                            p_widgets->p_parent->y = rect.y;
                            break;
                        }

                        default: {
                            p_widgets->dimension.w = rect.w;
                            p_widgets->dimension.h = rect.h;

                            if (p_widgets->p_data->has_parent()) {
                                p_widgets->dimension.x = rect.x - p_widgets->p_parent->x;
                                p_widgets->dimension.y = rect.y - p_widgets->p_parent->y;
                            } else {
                                p_widgets->p_parent->x = rect.x;
                                p_widgets->p_parent->y = rect.y;
                            }

                            break;
                        }
                    }
                }

                p_widgets->on_reload();
            }

            reload.clear();
        }
    };

    this->service_handler.allocate() = {
        .p_tag      = "synclayout",
        .p_callback = this,
        .function   = [](void *p_callback) {
            auto *runtime {static_cast<ekg::runtime*>(p_callback)};
            auto &synclayout {runtime->widget_list_map["synclayout"]};

            for (ekg::ui::abstract_widget *&p_widgets : synclayout) {
                if (p_widgets == nullptr || runtime->processed_widget_map[p_widgets->p_data->get_id()]) {
                    continue;
                }

                runtime->service_layout.process_scaled(p_widgets);
                runtime->processed_widget_map[p_widgets->p_data->get_id()] = true;
            }

            synclayout.clear();
            runtime->processed_widget_map.clear();
            ekg::dispatch(ekg::env::redraw);
        }
    };

    this->service_handler.allocate() = {
        .p_tag      = "gc",
        .p_callback = this,
        .function   = [](void *p_callback) {
            auto *runtime {static_cast<ekg::runtime*>(p_callback)};
            auto &all {runtime->widget_list_map["all"]};
            auto &high_frequency {runtime->widget_list_map["update"]};
            auto &redraw {runtime->widget_list_map["redraw"]};
            auto &allocator {runtime->gpu_allocator};

            redraw.clear();
            high_frequency.clear();
            std::vector<ekg::ui::abstract_widget*> new_list {};

            for (ekg::ui::abstract_widget *&p_widgets : all) {
                if (p_widgets == nullptr || p_widgets->p_data == nullptr) {
                    continue;
                }

                if (!p_widgets->p_data->is_alive()) {
                    ekg::hovered::id = ekg::hovered::id == p_widgets->p_data->get_id() ? 0 : ekg::hovered::id;
                    ekg::hovered::up = ekg::hovered::up == p_widgets->p_data->get_id() ? 0 : ekg::hovered::up;
                    ekg::hovered::down = ekg::hovered::down == p_widgets->p_data->get_id() ? 0 : ekg::hovered::down;
                    allocator.erase_scissor_by_id(p_widgets->p_data->get_id());

                    delete p_widgets->p_data;
                    delete p_widgets;

                    continue;
                }

                if (p_widgets->is_high_frequency) {
                    high_frequency.push_back(p_widgets);
                }

                if (p_widgets->p_data->get_state() == ekg::state::visible) {
                    redraw.push_back(p_widgets);
                }

                new_list.push_back(p_widgets);
            }

            all = new_list;
        }
    };
}

ekg::ui::abstract_widget *ekg::runtime::get_fast_widget_by_id(int32_t id) {
    /* widget ID 0 is defined as none, or be, ID token accumulation start with 1 and not 0 */
    return id ? this->widget_map[id] : nullptr;
}

void ekg::runtime::do_task_reload(ekg::ui::abstract_widget *p_widget) {
    if (p_widget != nullptr) {
        this->widget_list_map["reload"].emplace_back(p_widget);
        ekg::dispatch(ekg::env::reload);
    }
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
    this->f_renderer_small.bind_allocator(&this->gpu_allocator);

    this->f_renderer_normal.font_size = 0;
    this->f_renderer_normal.bind_allocator(&this->gpu_allocator);

    this->f_renderer_big.font_size = 24;
    this->f_renderer_big.bind_allocator(&this->gpu_allocator);
    this->update_size_changed();

    ekg::log() << "Registering user interface input bindings";

    this->service_input.bind("frame-drag-activy", "mouse-1");
    this->service_input.bind("frame-drag-activy", "finger-click");
    this->service_input.bind("frame-resize-activy", "mouse-1");
    this->service_input.bind("frame-resize-activy", "finger-click");

    this->service_input.bind("button-activy", "mouse-1");
    this->service_input.bind("button-activy", "finger-click");

    this->service_input.bind("checkbox-activy", "mouse-1");
    this->service_input.bind("checkbox-activy", "finger-click");

    this->service_input.bind("popup-activy", "mouse-1");
    this->service_input.bind("popup-activy", "finger-click");

    this->service_input.bind("textbox-action-select-all", "lctrl+a");
    this->service_input.bind("textbox-action-select-all", "rctrl+a");

    this->service_input.bind("textbox-action-select-all-inline", "mouse-1");
    this->service_input.bind("textbox-action-select", "lshift");
    this->service_input.bind("textbox-action-select", "rshift");

    this->service_input.bind("textbox-action-select-word", "mouse-1-double");
    this->service_input.bind("textbox-action-select-word", "finger-hold");

    this->service_input.bind("textbox-action-delete-left", "abs-backspace");
    this->service_input.bind("textbox-action-delete-right", "abs-delete");
    this->service_input.bind("textbox-action-break-line", "abs-return");
    this->service_input.bind("textbox-action-break-line", "abs-keypad enter");
    this->service_input.bind("textbox-action-tab", "tab");
    this->service_input.bind("textbox-action-modifier", "lctrl");
    this->service_input.bind("textbox-action-modifier", "rctrl");

    this->service_input.bind("clipboard-copy", "lctrl+c");
    this->service_input.bind("clipboard-copy", "rctrl+c");
    this->service_input.bind("clipboard-paste", "lctrl+v");
    this->service_input.bind("clipboard-paste", "rctrl+v");
    this->service_input.bind("clipboard-cut", "lctrl+x");
    this->service_input.bind("clipboard-cut", "rctrl+x");

    this->service_input.bind("textbox-action-up", "abs-up");
    this->service_input.bind("textbox-action-down", "abs-down");
    this->service_input.bind("textbox-action-right", "abs-right");
    this->service_input.bind("textbox-action-left", "abs-left");

    this->service_input.bind("textbox-activy", "mouse-1");
    this->service_input.bind("textbox-activy", "finger-click");

    this->service_input.bind("slider-activy", "mouse-1");
    this->service_input.bind("slider-activy", "finger-click");
    this->service_input.bind("slider-bar-increase", "mouse-wheel-up");
    this->service_input.bind("slider-bar-decrease", "mouse-wheel-down");
    this->service_input.bind("slider-bar-modifier", "lctrl");
    this->service_input.bind("slider-bar-modifier", "rctrl");

    this->service_input.bind("scrollbar-drag", "mouse-1");
    this->service_input.bind("scrollbar-drag", "finger-click");
    this->service_input.bind("scrollbar-scroll", "mouse-wheel");
    this->service_input.bind("scrollbar-scroll", "finger-swipe");
}

void ekg::runtime::gen_widget(ekg::ui::abstract *p_ui) {
    p_ui->set_id(++this->token_id);
    
    this->swap_widget_id_focused = p_ui->get_id();
    ekg::ui::abstract_widget *p_widget_created {};

    bool update_layout {};
    bool append_group {};

    switch (p_ui->get_type()) {
    case ekg::type::abstract: {
        ekg::ui::abstract_widget *p_widget {new ekg::ui::abstract_widget()};
        p_widget->p_data = p_ui;
        p_widget_created = p_widget;
        break;
    }
    case ekg::type::frame: {
        ekg::ui::frame_widget *p_widget {new ekg::ui::frame_widget()};
        p_widget->is_scissor_refresh = true;
        p_widget->p_data = p_ui;
        update_layout = true;
        p_widget_created = p_widget;
        this->current_bind_group = p_ui;
        p_ui->reset();
        break;
    }
    case ekg::type::button: {
        ekg::ui::button_widget *p_widget {new ekg::ui::button_widget()};
        p_widget->p_data = p_ui;
        p_widget_created = p_widget;
        append_group = true;
        break;
    }
    case ekg::type::label: {
        ekg::ui::label_widget *p_widget {new ekg::ui::label_widget()};
        p_widget->p_data = p_ui;
        p_widget_created = p_widget;
        append_group = true;
        break;
    }
    case ekg::type::checkbox: {
        ekg::ui::checkbox_widget *p_widget {new ekg::ui::checkbox_widget()};
        p_widget->p_data = p_ui;
        p_widget_created = p_widget;
        append_group = true;
        break;
    }
    case ekg::type::slider: {
        ekg::ui::slider_widget *p_widget {new ekg::ui::slider_widget()};
        p_widget->p_data = p_ui;
        p_widget_created = p_widget;
        append_group = true;
        break;
    }
    case ekg::type::popup: {
        ekg::ui::popup_widget *p_widget {new ekg::ui::popup_widget()};
        p_widget->p_data = p_ui;
        p_widget_created = p_widget;
        update_layout = false;
        break;
    }
    case ekg::type::textbox: {
        ekg::ui::textbox_widget *p_widget {new ekg::ui::textbox_widget()};
        p_widget->p_data = p_ui;
        p_widget_created = p_widget;
        append_group = true;
        break;
    }
    case ekg::type::scroll: {
        ekg::ui::scroll_widget *p_widget {new ekg::ui::scroll_widget()};
        p_widget->p_data = p_ui;
        p_widget_created = p_widget;
        append_group = true;
        break;
    }

    default:
        break;
    }

    this->widget_map[p_ui->get_id()] = p_widget_created;
    this->do_task_refresh(p_widget_created);
    this->do_task_reload(p_widget_created);

    if (append_group && this->current_bind_group != nullptr) {
        this->current_bind_group->add_child(p_ui->get_id());
    }

    if (update_layout) {
        this->do_task_synclayout(p_widget_created);
    }

    ekg::dispatch(ekg::env::swap);
}

void ekg::runtime::do_task_synclayout(ekg::ui::abstract_widget *p_widget) {
    if (p_widget == nullptr) {
        return;
    }

    bool is_group {p_widget->p_data->get_type() == ekg::type::frame};
    bool check_parent {is_group == false && p_widget->p_data->has_parent()};

    if (check_parent) {
        p_widget = this->get_fast_widget_by_id(p_widget->p_data->get_parent_id());
    }

    if (p_widget != nullptr) {
        this->widget_list_map["synclayout"].emplace_back(p_widget);
        ekg::dispatch(ekg::env::synclayout);
    }
}

void ekg::runtime::do_task_refresh(ekg::ui::abstract_widget *p_widget) {
    if (p_widget == nullptr) {
        return;
    }

    this->widget_list_map["refresh"].emplace_back(p_widget);
    ekg::dispatch(ekg::env::refresh);
    ekg::dispatch(ekg::env::redraw);
}

void ekg::runtime::end_group_flag() {
    this->current_bind_group = nullptr;
}

void ekg::runtime::erase(int32_t id) {
    auto &all {this->widget_list_map["all"]};

    for (size_t it {}; it < all.size(); it++) {
        ekg::ui::abstract_widget *&p_widget {all[it]};

        if (p_widget != nullptr && p_widget->p_data->get_id() == id) {
            this->widget_map.erase(p_widget->p_data->get_id());
            all.erase(all.begin() + it);
            break;
        }
    }
}

void ekg::runtime::set_update_high_frequency(ekg::ui::abstract_widget *p_widget) {
    if (p_widget != nullptr && !p_widget->is_high_frequency) {
        this->enable_high_priority_frequency = true;
        
        auto &update {this->widget_list_map["update"]};
        bool contains {};

        for (ekg::ui::abstract_widget *&p_widgets : update) {
            contains = p_widgets == p_widget;
            if (contains) {
                break;
            }
        }

        if (!contains) {
            update.push_back(p_widget);
            p_widget->is_high_frequency = true;
        }
    }
}