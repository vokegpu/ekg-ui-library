/**
 * MIT License
 * 
 * Copyright (c) 2022-2024 Rina Wilk / vokegpu@gmail.com
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

#ifndef EKG_UI_ABSTRACT_WIDGET_H
#define EKG_UI_ABSTRACT_WIDGET_H

#include "ekg/ui/abstract/ui_abstract.hpp"
#include "ekg/util/io.hpp"
#include "ekg/os/platform.hpp"
#include "ekg/ui/display.hpp"

#define ekg_action_dispatch(should, action) \
  if (should && this->p_data->get_task(action) != nullptr) { \
    ekg::task *p_ekg_action_dispatch_task {this->p_data->get_task(action)}; \
    p_ekg_action_dispatch_task->info.p_ui = static_cast<void*>(this->p_data); \
    ekg::core->service_handler.generate() = *p_ekg_action_dispatch_task; \
  } \

/**
 * `self->`p_data->get_layer(layer)
 * the reason for `self->` be removed, due some widgets like:
 * listbox which perform recursive iterations.
 **/
#define ekg_layer(layer) (p_data->get_layer(layer))

namespace ekg::ui {
  class abstract_widget {
  public:
    ekg::ui::abstract *p_data {};
    ekg::ui::abstract_widget *p_abs_parent_widget {};

    ekg::flag flag {};
    ekg::rect dimension {};
    ekg::vec2 min_size {};

    ekg::rect *p_parent {};
    ekg::vec4 *p_scroll {};
    ekg::rect *p_parent_scissor {};

    ekg::rect scissor {};
    ekg::rect empty_parent {};
    ekg::vec4 empty_scroll {};

    bool is_high_frequency {};
    bool is_targeting_absolute_parent {};
  public:
    bool was_reloaded {};
    bool was_refreshed {};
    bool was_syncedlayout {};
  public:
    ekg::rect get_static_rect();

    [[nodiscard]] ekg::rect &get_abs_rect();
  public:
    explicit abstract_widget();

    ~abstract_widget();

    virtual void on_create();

    virtual void on_destroy();

    virtual void on_reload();

    virtual void on_pre_event(ekg::os::io_event_serial &io_event_serial);

    virtual void on_event(ekg::os::io_event_serial &io_event_serial);

    virtual void on_post_event(ekg::os::io_event_serial &io_event_serial);

    virtual void on_update();

    virtual void on_draw_refresh();
  };
}

#endif