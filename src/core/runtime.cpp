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

#include <algorithm>
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
#include "ekg/ui/listbox/ui_listbox_widget.hpp"
#include "ekg/ui/scrollbar/ui_scrollbar_widget.hpp"
#include "ekg/ui/scrollbar/ui_scrollbar.hpp"
#include "ekg/draw/draw.hpp"
#include "ekg/ekg.hpp"
#include "ekg/util/gui.hpp"
#include "ekg/layout/docknize.hpp"

ekg::stack ekg::runtime::collect {};
ekg::stack ekg::runtime::back {};
ekg::stack ekg::runtime::front {};

ekg::current_hovered_state ekg::hovered {};

void ekg::runtime::update_size_changed() {
  ekg::dispatch(ekg::env::redraw);

  this->service_layout.update_scale_factor();
  uint32_t font_size {
    static_cast<uint32_t>(ekg_clamp(static_cast<int32_t>(18.0f * this->service_layout.get_scale_factor()), 0, 256))
  };

  if (this->f_renderer_normal.font_size != font_size) {
    this->f_renderer_small.set_size(ekg_min(font_size - 4, 4));
    this->f_renderer_normal.set_size(ekg_min(font_size, 8));
    this->f_renderer_big.set_size(ekg_min(font_size + 6, 12));
  }

  for (ekg::ui::abstract_widget *&p_widgets: this->loaded_widget_list) {
    p_widgets->on_reload();
    if (!p_widgets->p_data->has_parent() && p_widgets->p_data->has_children()) {
      this->do_task_synclayout(p_widgets);
    }
  }
}

void ekg::runtime::init() {
  this->gpu_allocator.init();
  this->prepare_tasks();
  this->prepare_ui_env();
  this->service_layout.init();
  this->service_theme.init();
  this->service_input.init();
}

void ekg::runtime::quit() {
  this->gpu_allocator.quit();
  this->service_theme.quit();
  this->service_layout.quit();
}

void ekg::runtime::process_event() {
  this->service_input.on_event(this->io_event_serial);

  bool pressed {ekg::input::pressed()};
  bool released {ekg::input::released()};

  ekg::hovered.id *= !(pressed || released || ekg::input::motion());

  if (
      this->p_abs_activity_widget != nullptr &&
      this->p_abs_activity_widget->flag.absolute
    ) {

    this->p_abs_activity_widget->on_pre_event(this->io_event_serial);
    this->p_abs_activity_widget->on_event(this->io_event_serial);
    this->p_abs_activity_widget->on_post_event(this->io_event_serial);

    return;
  }

  this->p_abs_activity_widget = nullptr;

  bool hovered {};
  bool first_absolute {};

  ekg::ui::abstract_widget *p_widget_focused {};

  for (ekg::ui::abstract_widget *&p_widgets: this->loaded_widget_list) {
    if (p_widgets == nullptr || !p_widgets->p_data->is_alive()) {
      continue;
    }

    p_widgets->on_pre_event(this->io_event_serial);

    /**
     * Text input like textbox and keyboard events should not update stack, instead just mouse events.
     **/
    hovered = (
      !(
        this->io_event_serial.event_type == ekg::platform_event_type::key_down   ||
        this->io_event_serial.event_type == ekg::platform_event_type::key_up     ||
        this->io_event_serial.event_type == ekg::platform_event_type::text_input
       )
      && p_widgets->flag.hovered
      && p_widgets->p_data->is_visible()
      && p_widgets->p_data->get_state() != ekg::state::disable
    );

    if (hovered) {
      ekg::hovered.id = p_widgets->p_data->get_id();
      p_widget_focused = p_widgets;
      first_absolute = false;
    }

    /**
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
     **/
    if (p_widgets->flag.absolute && !first_absolute) {
      p_widget_focused = p_widgets;
      first_absolute = true;
    }

    p_widgets->on_post_event(this->io_event_serial);
    if (!hovered && !p_widgets->flag.absolute) {
      p_widgets->on_event(this->io_event_serial);
    }
  }

  ekg::hovered.type = ekg::type::abstract;

  if (p_widget_focused != nullptr) {
    p_widget_focused->on_pre_event(this->io_event_serial);
    p_widget_focused->on_event(this->io_event_serial);
    p_widget_focused->on_post_event(this->io_event_serial);

    if (p_widget_focused->flag.absolute) {
      this->p_abs_activity_widget = p_widget_focused;
    }

    ekg::hovered.type = p_widget_focused->p_data->get_type();
  }

  if (pressed) {
    ekg::hovered.down = ekg::hovered.id;
    ekg::hovered.down_type = p_widget_focused != nullptr ? p_widget_focused->p_data->get_type() : ekg::type::abstract;
  } else if (released) {
    ekg::hovered.up = ekg::hovered.id;
    ekg::hovered.down_type = p_widget_focused != nullptr ? p_widget_focused->p_data->get_type() : ekg::type::abstract;
  }

  if (
      ekg::hovered.last != ekg::hovered.id &&
      ekg::hovered.id != 0 &&
      (pressed || released)
    ) {

    ekg::hovered.swap = ekg::hovered.id;
    ekg::hovered.last = ekg::hovered.id;

    ekg::dispatch(ekg::env::swap);
    ekg::dispatch(ekg::env::redraw);
  }
}

void ekg::runtime::process_update() {
  ekg::timing::ticks = this->p_os_platform->get_ticks();
  ekg::reach(this->ui_timing, 1000) && ekg::reset(this->ui_timing);
  ekg::timing::second = this->ui_timing.current_ticks;

  this->service_input.on_update();

  if (this->enable_high_priority_frequency) {
    auto &update = this->update_widget_list;
    size_t counter {};

    for (ekg::ui::abstract_widget *&p_widgets: update) {
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

void ekg::runtime::process_render() {
  if (ekg::ui::redraw) {
    ekg::ui::redraw = false;

    auto &all {this->loaded_widget_list};

    /**
     * The allocator starts here,
     * the GPU data instance and geometry resource are "cleared"
     * and reseted here.
     **/
    this->gpu_allocator.invoke();

    for (ekg::ui::abstract_widget *&p_widgets : all) {
      if (p_widgets != nullptr && p_widgets->p_data->is_alive() && p_widgets->p_data->is_visible()) {
        /**
         * The allocator is used here,
         * each time this statement is called theres one GPU data,
         * being allocated/filled.
         * 
         * The order of rendering depends on which functions are invoke first.
         * 
         * E.g:
         *  gpu-data-group-1 (on_draw_refresh())
         *  gpu-data-group-2
         *  gpu-data-group-3
         * 
         * `gpu-data-group-3` is always hovering all the previous GPU data groups.
         **/
        p_widgets->on_draw_refresh();
      }
    }

    /**
     * The allocator does not need to be called all the time,
     * cause it is require more CPU-side calls and GPU-communication/synchronization.
     * 
     * The use of the word "revoke" means that the `invoked signal` ended with a
     * possible `revoke`, i.e all the generated/populated/filled GPU-data is processed and
     * converted to a geometry GPU-buffer. Removing all the unnecessary GPU-data.
     **/
    this->gpu_allocator.revoke();
  }

  /**
   * The allocator draws each GPU-data now serialized.
   * There is two type concept of draw:
   * 
   * 1- A concave shape does not have a rectangle form, and can render text(s) and
   * batched quad(s) at once.
   * 2- A convex shape have a unique form: rectangle.
   * 
   * The convex form is rendered one per one draw call. It is stupid I know, but with this is possible add cool effects.
   * While concave have the possibility of rendering a lot of shapes at once, is not viable to add many custom effects.
   * 
   * The native GPU API calls are apart from allocator, depends on current GPU API selected.
   * Rendering-hardware interface also known as RHI implements different each GPU API.
   * OpenGL 3/4/ES 3, Vulkan, MoltenVK, DirectX11/12.
   * May differ the features.
   * 
   * But check the code allocator inside code comments.
   **/
  this->gpu_allocator.draw();
}

void ekg::runtime::prepare_tasks() {
  ekg::log() << "Preparing internal EKG core";

  this->service_handler.allocate() = {
    {
      .tag    = "refresh",
      .p_data = this
    },
    .function = [](ekg::info &info) {
      ekg::runtime *p_runtime {static_cast<ekg::runtime *>(info.p_data)};

      auto &all = p_runtime->loaded_widget_list;
      auto &refresh = p_runtime->refresh_widget_list;
      bool should_call_gc {};

      for (ekg::ui::abstract_widget *&p_widgets: refresh) {
        if (p_widgets == nullptr || !p_widgets->was_refreshed) {
          continue;
        }

        p_widgets->was_refreshed = false;

        if (p_widgets == nullptr || !p_widgets->p_data->is_alive()) {
          should_call_gc = true;
          continue;
        }

        all.push_back(p_widgets);
      }

      refresh.clear();

      if (should_call_gc) {
        ekg::dispatch(ekg::env::gc);
      }
    }
  };

  this->service_handler.allocate() = {
    {
      .tag    = "swap",
      .p_data = this
    },
    .function = [](ekg::info &info) {
      if (!ekg::hovered.swap) {
        return;
      }

      ekg::runtime *p_runtime {static_cast<ekg::runtime*>(info.p_data)};
      ekg::runtime::collect.target_id = ekg::hovered.swap;

      auto &all {p_runtime->loaded_widget_list};
      for (ekg::ui::abstract_widget *&p_widgets: all) {
        if (p_widgets == nullptr || p_widgets->p_data->has_parent()) {
          continue;
        }

        ekg::runtime::collect.clear();
        ekg::push_back_stack(p_widgets, ekg::runtime::collect);

        if (ekg::runtime::collect.target_id_found) {
          ekg::runtime::front.ordered_list.insert(
            ekg::runtime::front.ordered_list.end(),
            ekg::runtime::collect.ordered_list.begin(),
            ekg::runtime::collect.ordered_list.end()
          );
        } else {
          ekg::runtime::back.ordered_list.insert(
            ekg::runtime::back.ordered_list.end(),
            ekg::runtime::collect.ordered_list.begin(),
            ekg::runtime::collect.ordered_list.end()
          );
        }
      }

      ekg::hovered.swap = 0;

      std::copy(
        ekg::runtime::back.ordered_list.begin(),
        ekg::runtime::back.ordered_list.end(),
        all.begin()
      );
      
      std::copy(
        ekg::runtime::front.ordered_list.begin(),
        ekg::runtime::front.ordered_list.end(),
        all.begin() + ekg::runtime::back.ordered_list.size()
      );

      ekg::runtime::front.clear();
      ekg::runtime::back.clear();
      ekg::runtime::collect.clear();
    }
  };

  this->service_handler.allocate() = {
    {
      .tag    = "reload",
      .p_data = this
    },
    .function = [](ekg::info &info) {
      ekg::runtime *p_runtime {static_cast<ekg::runtime*>(info.p_data)};
      auto &reload = p_runtime->reload_widget_list;

      ekg::vec4 rect {};

      for (ekg::ui::abstract_widget *&p_widgets: reload) {
        if (p_widgets == nullptr || !p_widgets->was_reloaded) {
          continue;
        }

        p_widgets->was_reloaded = false;

        auto &sync_flags {p_widgets->p_data->get_sync()};
        if (ekg_bitwise_contains(sync_flags, static_cast<uint16_t>(ekg::ui_sync::reset))) {
          ekg_bitwise_remove(sync_flags, static_cast<uint16_t>(ekg::ui_sync::reset));

          switch (p_widgets->p_data->get_type()) {
            case ekg::type::frame: {
              auto p_ui {(ekg::ui::frame *) p_widgets->p_data};
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

        if (ekg_bitwise_contains(sync_flags, static_cast<uint16_t>(ekg::ui_sync::dimension))) {
          ekg_bitwise_remove(sync_flags, static_cast<uint16_t>(ekg::ui_sync::dimension));

          auto &rect {p_widgets->p_data->ui()};
          switch (p_widgets->p_data->get_level()) {
            case ekg::level::top_level: {
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
    {
      .tag    = "synclayout",
      .p_data = this
    },
    .function = [](ekg::info &info) {
      ekg::runtime *p_runtime {static_cast<ekg::runtime *>(info.p_data)};
      auto &synclayout {p_runtime->synclayout_widget_list};

      for (ekg::ui::abstract_widget *&p_widgets: synclayout) {
        if (p_widgets == nullptr || !p_widgets->was_syncedlayout) {
          continue;
        }

        p_widgets->was_syncedlayout = false;
        ekg::layout::docknize(p_widgets);
      }

      synclayout.clear();
      ekg::dispatch(ekg::env::redraw);
    }
  };

  this->service_handler.allocate() = {
    {
      .tag    = "gc",
      .p_data = this,
    },
    .function = [](ekg::info &info) {
      ekg::runtime *p_runtime {static_cast<ekg::runtime *>(info.p_data)};
      auto &all {p_runtime->loaded_widget_list};
      auto &high_frequency {p_runtime->update_widget_list};
      auto &redraw {p_runtime->redraw_widget_list};
      auto &allocator {p_runtime->gpu_allocator};

      redraw.clear();
      high_frequency.clear();
      std::vector<ekg::ui::abstract_widget*> new_list {};
      int32_t element_id {};

      for (ekg::ui::abstract_widget *&p_widgets: all) {
        if (p_widgets == nullptr || p_widgets->p_data == nullptr) {
          continue;
        }

        if (!p_widgets->p_data->is_alive()) {
          element_id = p_widgets->p_data->get_id();

          ekg::hovered.id *= ekg::hovered.id == element_id;
          ekg::hovered.up *= ekg::hovered.up == element_id;
          ekg::hovered.down *= ekg::hovered.down == element_id;

          delete p_widgets->p_data;
          delete p_widgets;

          continue;
        }

        if (p_widgets->is_high_frequency) {
          high_frequency.push_back(p_widgets);
        }

        if (p_widgets->p_data->is_visible()) {
          redraw.push_back(p_widgets);
        }

        new_list.push_back(p_widgets);
      }

      all = new_list;
    }
  };
}

ekg::ui::abstract_widget *ekg::runtime::get_fast_widget_by_id(int32_t id) {
  /** widget ID 0 is defined as none, or be, ID token accumulation start with 1 and not 0 */
  return id ? this->widget_map[id] : nullptr;
}

void ekg::runtime::do_task_reload(ekg::ui::abstract_widget *p_widget) {
  if (p_widget != nullptr && !p_widget->was_reloaded) {
    this->reload_widget_list.emplace_back(p_widget);
    ekg::dispatch(ekg::env::reload);
    p_widget->was_reloaded = true;
  }
}

void ekg::runtime::prepare_ui_env() {
  ekg::log() << "Preparing internal user interface environment";

  this->f_renderer_small.sampler_texture.gl_protected_active_index = true;
  this->f_renderer_small.set_size(16);
  this->f_renderer_small.bind_allocator(&this->gpu_allocator);

  this->f_renderer_normal.sampler_texture.gl_protected_active_index = true;
  this->f_renderer_normal.set_size(0);
  this->f_renderer_normal.bind_allocator(&this->gpu_allocator);

  this->f_renderer_big.sampler_texture.gl_protected_active_index = true;
  this->f_renderer_big.set_size(24);
  this->f_renderer_big.bind_allocator(&this->gpu_allocator);
  this->update_size_changed();

  ekg::log() << "Registering user interface input bindings";

  this->service_input.bind("frame-drag-activity", "mouse-1");
  this->service_input.bind("frame-drag-activity", "finger-click");
  this->service_input.bind("frame-resize-activity", "mouse-1");
  this->service_input.bind("frame-resize-activity", "finger-click");

  this->service_input.bind("button-activity", "mouse-1");
  this->service_input.bind("button-activity", "finger-click");

  this->service_input.bind("checkbox-activity", "mouse-1");
  this->service_input.bind("checkbox-activity", "finger-click");

  this->service_input.bind("popup-activity", "mouse-1");
  this->service_input.bind("popup-activity", "finger-click");

  this->service_input.bind("textbox-activity", "mouse-1");
  this->service_input.bind("textbox-activity", "finger-click");

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

  this->service_input.bind("textbox-action-up", "abs-up");
  this->service_input.bind("textbox-action-down", "abs-down");
  this->service_input.bind("textbox-action-right", "abs-right");
  this->service_input.bind("textbox-action-left", "abs-left");

  this->service_input.bind("clipboard-copy", "lctrl+c");
  this->service_input.bind("clipboard-copy", "rctrl+c");
  this->service_input.bind("clipboard-copy", "copy");
  this->service_input.bind("clipboard-paste", "lctrl+v");
  this->service_input.bind("clipboard-paste", "rctrl+v");
  this->service_input.bind("clipboard-paste", "paste");
  this->service_input.bind("clipboard-cut", "lctrl+x");
  this->service_input.bind("clipboard-cut", "rctrl+x");
  this->service_input.bind("clipboard-cut", "cut");

  this->service_input.bind("listbox-activity-open", "mouse-1-double");
  this->service_input.bind("listbox-activity-open", "finger-hold");

  this->service_input.bind("listbox-activity-select", "mouse-1");
  this->service_input.bind("listbox-activity-select", "finger-click");
  this->service_input.bind("listbox-activity-select-many", "lctrl+mouse-1");
  this->service_input.bind("listbox-activity-select-many", "rctrl+mouse-1");

  this->service_input.bind("slider-activity", "mouse-1");
  this->service_input.bind("slider-activity", "finger-click");
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
  p_ui->unsafe_set_id(++this->token_id);
  
  ekg::hovered.swap = p_ui->get_id();
  ekg::ui::abstract_widget *p_widget_created {};

  bool update_layout {};
  bool append_group {};
  bool switch_groups {};

  switch (p_ui->get_type()) {
    case ekg::type::abstract: {
      ekg::ui::abstract_widget *p_widget {new ekg::ui::abstract_widget()};
      p_widget->p_data = p_ui;
      p_widget_created = p_widget;
      break;
    }

    case ekg::type::frame: {
      ekg::ui::frame_widget *p_widget {new ekg::ui::frame_widget()};
      p_widget->p_data = p_ui;
      update_layout = true;
      p_widget_created = p_widget;
      p_ui->reset();

      if (p_widget->p_data->get_place_dock() == ekg::dock::none) {
        this->p_current_ui_container = p_ui;
      } else {
        append_group = true;
        switch_groups = true;
      }

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

    case ekg::type::listbox: {
      ekg::ui::listbox_widget *p_widget {new ekg::ui::listbox_widget()};
      p_widget->p_data = p_ui;
      p_widget_created = p_widget;
      append_group = true;
      break;
    }

    case ekg::type::scrollbar: {
      ekg::ui::scrollbar_widget *p_widget {new ekg::ui::scrollbar_widget()};
      p_widget->p_data = p_ui;
      p_widget_created = p_widget;
      append_group = true;
      break;
    }

    default: {
      break;
    }
  }

  this->widget_map[p_ui->get_id()] = p_widget_created;
  this->do_task_refresh(p_widget_created);
  this->do_task_reload(p_widget_created);
  p_widget_created->on_create();

  if (append_group && this->p_current_ui_container != nullptr) {
    this->p_current_ui_container->add_child(p_ui->get_id());

    if (switch_groups) {
      this->p_current_ui_container = p_widget_created->p_data;
    }
  }

  if (update_layout) {
    this->do_task_synclayout(p_widget_created);
  }

  ekg::dispatch(ekg::env::swap);
}

void ekg::runtime::do_task_synclayout(ekg::ui::abstract_widget *p_widget) {
  if (p_widget != nullptr && !p_widget->was_syncedlayout) {
    bool is_group {p_widget->p_data->get_type() == ekg::type::frame};
    bool check_parent {is_group == false && p_widget->p_data->has_parent()};

    if (check_parent) {
      p_widget = this->get_fast_widget_by_id(p_widget->p_data->get_parent_id());
    }

    if (p_widget != nullptr) {
      this->synclayout_widget_list.emplace_back(p_widget);
      ekg::dispatch(ekg::env::synclayout);
      p_widget->was_syncedlayout = true;
    }
  }
}

void ekg::runtime::do_task_refresh(ekg::ui::abstract_widget *p_widget) {
  if (p_widget != nullptr && !p_widget->was_refreshed) {
    this->refresh_widget_list.emplace_back(p_widget);
    ekg::dispatch(ekg::env::refresh);
    ekg::dispatch(ekg::env::redraw);
    p_widget->was_refreshed = true;
  }
}

void ekg::runtime::end_group_flag() {
  this->p_current_ui_container = nullptr;
}

void ekg::runtime::end_group_parent_flag() {
  ekg::ui::abstract_widget *p_parent_widget {};
  if (
      this->p_current_ui_container != nullptr &&
      (p_parent_widget = this->get_fast_widget_by_id(this->p_current_ui_container->get_parent_id())) != nullptr
    ) {
    this->p_current_ui_container = p_parent_widget->p_data;
  }
}

void ekg::runtime::erase(int32_t id) {
  auto &all {this->loaded_widget_list};

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

    auto &update {this->update_widget_list};
    bool contains {};

    for (ekg::ui::abstract_widget *&p_widgets: update) {
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