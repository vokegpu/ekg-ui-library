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

#include "ekg/ui/frame/ui_frame_widget.hpp"
#include "ekg/ui/frame/ui_frame.hpp"
#include "ekg/ekg.hpp"
#include "ekg/draw/draw.hpp"
#include "ekg/os/platform.hpp"

void ekg::ui::frame_widget::on_reload() {
  abstract_widget::on_reload();
  ekg::rect &rect {this->get_abs_rect()};
  this->ui_theme_activity_offset = static_cast<float>(ekg::current_theme_scheme().frame_activity_offset);
}

void ekg::ui::frame_widget::on_event(ekg::os::io_event_serial &io_event_serial) {
  if (this->p_frame_widget_top_level != nullptr && this->p_frame_widget_top_level->target_dock_drag != ekg::dock::none) {
    this->p_frame_widget_top_level->on_event(io_event_serial);
    this->target_dock_drag = this->p_frame_widget_top_level->target_dock_drag;
    this->flag = this->p_frame_widget_top_level->flag;
    return;
  }

  ekg::ui::frame *p_ui {(ekg::ui::frame *) this->p_data};
  ekg::rect &rect {this->get_abs_rect()};

  ekg::flags drag_dock_flags {p_ui->get_drag_dock()};
  ekg::flags resize_dock_flags {p_ui->get_resize_dock()};
  ekg::flags shown_cursor_dock_flags {};

  bool pressed {ekg::input::pressed()};
  bool released {ekg::input::released()};
  bool motion {ekg::input::motion()};

  ekg_action_dispatch(
    motion && this->flag.hovered && ekg::timing::second > ekg::ui::latency,
    ekg::action::motion
  );

  if ((drag_dock_flags != ekg::dock::none || resize_dock_flags != ekg::dock::none) && pressed &&
      this->flag.hovered && !this->flag.activity &&
      (ekg::input::action("frame-drag-activity") || ekg::input::action("frame-resize-activity"))
     ) {
    ekg::vec4 &interact {ekg::input::interact()};
    const ekg::vec2 vec_limit_offset {this->ui_theme_activity_offset, this->ui_theme_activity_offset};

    ekg::set_dock_scaled(this->scissor, vec_limit_offset, this->docker_activity_drag);
    ekg::set_dock_scaled(this->scissor, vec_limit_offset / 4.0f, this->docker_activity_resize);

    this->target_dock_drag = ekg::find_collide_dock(this->docker_activity_drag, drag_dock_flags, interact);
    this->target_dock_resize = ekg::find_collide_dock(this->docker_activity_resize, resize_dock_flags, interact);

    shown_cursor_dock_flags = this->target_dock_resize;

    this->rect_delta.x = interact.x - rect.x;
    this->rect_delta.y = interact.y - rect.y;
    this->rect_delta.w = rect.w;
    this->rect_delta.h = rect.h;
    this->old_rect = rect;

    this->flag.activity = this->target_dock_drag != ekg::dock::none || this->target_dock_resize != ekg::dock::none;
    this->flag.absolute = this->flag.activity;
  
    if (this->p_frame_widget_top_level != nullptr && this->target_dock_drag != ekg::dock::none) {
      /**
       * We need to calculate the delta based on top-level frame
       * and not the low-level frame. So the drag is perfect.
       **/
      ekg::rect &top_level_frame_rect {this->p_frame_widget_top_level->get_abs_rect()};
      this->rect_delta.x = interact.x - top_level_frame_rect.x;
      this->rect_delta.y = interact.y - top_level_frame_rect.y;

      this->p_frame_widget_top_level->rect_delta = this->rect_delta;
      this->p_frame_widget_top_level->flag = this->flag;
      this->p_frame_widget_top_level->target_dock_drag = this->target_dock_drag;
    }

    ekg_action_dispatch(
      true,
      ekg::action::press
    );
  } else if (motion && this->flag.activity) {
    ekg::rect new_rect {rect};
    ekg::vec4 interact {ekg::input::interact()};
    shown_cursor_dock_flags = this->target_dock_resize;

    if (this->target_dock_drag != ekg::dock::none && this->target_dock_resize == ekg::dock::none) {
      ekg_action_dispatch(
        ekg::timing::second > ekg::ui::latency,
        ekg::action::drag
      );

      new_rect.x = interact.x - this->rect_delta.x;
      new_rect.y = interact.y - this->rect_delta.y;
      ekg::cursor = ekg::system_cursor::size_all;
    }

    if (this->target_dock_resize != ekg::dock::none) {
      ekg_action_dispatch(
        ekg::timing::second > ekg::ui::latency,
        ekg::action::resize
      );

      if (ekg_bitwise_contains(this->target_dock_resize, ekg::dock::left)) {
        interact.x = ekg_min(interact.x, this->rect_delta.x);
        new_rect.x = ekg_max(interact.x - this->rect_delta.x, this->old_rect.x + this->old_rect.w - ekg::ui::min_frame_size);
        new_rect.w = (this->old_rect.x + this->old_rect.w) - new_rect.x;
      }

      if (ekg_bitwise_contains(this->target_dock_resize, ekg::dock::right)) {
        new_rect.w = ekg_min(this->rect_delta.w + ((interact.x - new_rect.x) - this->rect_delta.x), ekg::ui::min_frame_size);
      }

      if (ekg_bitwise_contains(this->target_dock_resize, ekg::dock::top)) {
        interact.y = ekg_min(interact.y, this->rect_delta.y);
        new_rect.y = ekg_max(interact.y - this->rect_delta.y, this->old_rect.y + this->old_rect.h - ekg::ui::min_frame_size);
        new_rect.h = (this->old_rect.y + this->old_rect.h) - new_rect.y;
      }

      if (ekg_bitwise_contains(this->target_dock_resize, ekg::dock::bottom)) {
        new_rect.h = ekg_min(this->rect_delta.h + ((interact.y - new_rect.y) - this->rect_delta.y), ekg::ui::min_frame_size);
      }
    }

    ekg::set_rect_clamped(
      new_rect,
      ekg::ui::min_frame_size
    );

    if (rect != new_rect) {
      if (p_ui->has_parent()) {
        this->dimension.x = new_rect.x - this->p_parent->x;
        this->dimension.y = new_rect.y - this->p_parent->y;
      } else {
        this->p_parent->x = new_rect.x;
        this->p_parent->y = new_rect.y;
      }

      this->dimension.w = new_rect.w;
      this->dimension.h = new_rect.h;

      p_ui->ui() = new_rect;

      if (this->target_dock_resize != ekg::dock::none) {
        this->is_targeting_absolute_parent = true;
        ekg::synclayout(this);
        //ekg::reload(this);
      }

      ekg::ui::redraw = true;
    }
  } else if (resize_dock_flags != ekg::dock::none && this->flag.hovered && !this->flag.activity) {
    const ekg::vec2 vec_limit_offset {this->ui_theme_activity_offset, this->ui_theme_activity_offset};
    ekg::vec4 &interact {ekg::input::interact()};
    ekg::set_dock_scaled(this->scissor, vec_limit_offset / 4.0f, this->docker_activity_resize);
    shown_cursor_dock_flags = ekg::find_collide_dock(this->docker_activity_resize, resize_dock_flags, interact);
  }

  if (shown_cursor_dock_flags) {
    bool is_top {
      static_cast<bool>(ekg_bitwise_contains(shown_cursor_dock_flags, ekg::dock::top))
    };
    
    bool is_bottom {
      static_cast<bool>(ekg_bitwise_contains(shown_cursor_dock_flags, ekg::dock::bottom))
    };

    bool is_left {
      static_cast<bool>(ekg_bitwise_contains(shown_cursor_dock_flags, ekg::dock::left))
    };
    
    bool is_right {
      static_cast<bool>(ekg_bitwise_contains(shown_cursor_dock_flags, ekg::dock::right))
    };

    if ((is_top && is_left) || (is_bottom && is_right)) {
      ekg::cursor = ekg::system_cursor::size_nwse;
    } else if ((is_top && is_right) || (is_bottom && is_left)) {
      ekg::cursor = ekg::system_cursor::size_nesw;
    } else if (is_top || is_bottom) {
      ekg::cursor = ekg::system_cursor::size_ns;
    } else if (is_left || is_right) {
      ekg::cursor = ekg::system_cursor::size_we;
    }
  }

  if (released) {
    if (this->flag.activity) {
      this->flag.absolute = false;

      ekg_action_dispatch(
        this->flag.hovered,
        ekg::action::release
      );
    }

    this->target_dock_resize = ekg::dock::none;
    this->target_dock_drag = ekg::dock::none;
    this->flag.activity = false;
  }
}

void ekg::ui::frame_widget::on_update() {
  if (this->p_scroll_embedded != nullptr) {
    this->p_scroll_embedded->clamp_scroll();
    this->p_scroll_embedded->reset_scroll();
    this->p_scroll_embedded->scissor = this->scissor;
  }
}

void ekg::ui::frame_widget::on_draw_refresh() {
  ekg::rect &rect {this->get_abs_rect()};
  ekg::service::theme_scheme_t &theme_scheme {ekg::current_theme_scheme()};

  if (this->p_scroll_embedded != nullptr) {
    this->p_scroll_embedded->scissor = this->scissor;
    this->p_scroll_embedded->clamp_scroll();
  }

  ekg::draw::sync_scissor(this->scissor, rect, this->p_parent_scissor);
  ekg_draw_assert_scissor();

  ekg::draw::rect(
    rect,
    theme_scheme.frame_background,
    ekg::draw_mode::filled,
    ekg_layer(ekg::layer::background)
  );

  ekg::draw::rect(
    this->docker_activity_drag.top,
    theme_scheme.frame_border,
    ekg::draw_mode::filled
  );

  ekg::draw::rect(
    rect,
    theme_scheme.frame_outline,
    ekg::draw_mode::outline
  );
}