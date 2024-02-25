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
  auto &rect {this->get_abs_rect()};
  this->ui_theme_activity_offset = static_cast<float>(ekg::theme().frame_activity_offset);
}

void ekg::ui::frame_widget::on_event(ekg::os::io_event_serial &io_event_serial) {
  auto &interact {ekg::input::interact()};
  auto p_ui {(ekg::ui::frame *) this->p_data};
  auto &rect {this->get_abs_rect()};

  uint16_t drag_dock_flags {p_ui->get_drag_dock()};
  uint16_t resize_dock_flags {p_ui->get_resize_dock()};
  uint16_t shown_cursor_dock_flags {};

  bool pressed {ekg::input::pressed()};
  bool released {ekg::input::released()};
  bool motion {ekg::input::motion()};

  ekg_action_dispatch(
    motion && ekg::timing::second > ekg::display::latency && this->flag.hovered,
    ekg::action::motion
  );

  if ((drag_dock_flags != ekg::dock::none || resize_dock_flags != ekg::dock::none) && pressed &&
      this->flag.hovered && !this->flag.activity &&
      (ekg::input::action("frame-drag-activity") || ekg::input::action("frame-resize-activity"))) {
    ekg::draw::get_visible(this->p_data->get_id(), this->scissor);

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

    this->flag.activity = this->target_dock_drag != ekg::dock::none || this->target_dock_resize != ekg::dock::none;
    this->flag.absolute = this->flag.activity;
  
    ekg_action_dispatch(
      true,
      ekg::action::press
    );
  } else if (motion && this->flag.activity) {
    ekg::rect new_rect {rect};
    shown_cursor_dock_flags = this->target_dock_resize;

    if (this->target_dock_drag != ekg::dock::none && this->target_dock_resize == ekg::dock::none) {
      ekg_action_dispatch(
        ekg::timing::second > ekg::display::latency && this->flag.hovered,
        ekg::action::drag
      );

      new_rect.x = interact.x - this->rect_delta.x;
      new_rect.y = interact.y - this->rect_delta.y;
    }

    if (this->target_dock_resize != ekg::dock::none) {
      ekg_action_dispatch(
        ekg::timing::second > ekg::display::latency && this->flag.hovered,
        ekg::action::resize
      );

      if (ekg_bitwise_contains(this->target_dock_resize, ekg::dock::left)) {
        float diff {((interact.x - new_rect.x) - this->rect_delta.x)};
        new_rect.x = (interact.x - this->rect_delta.x);
        new_rect.w -= diff;
      }

      if (ekg_bitwise_contains(this->target_dock_resize, ekg::dock::right)) {
        new_rect.w = this->rect_delta.w + ((interact.x - new_rect.x) - this->rect_delta.x);
      }

      if (ekg_bitwise_contains(this->target_dock_resize, ekg::dock::top)) {
        float diff {((interact.y - new_rect.y) - this->rect_delta.y)};
        new_rect.y = (interact.y - this->rect_delta.y);
        new_rect.h -= diff;
      }

      if (ekg_bitwise_contains(this->target_dock_resize, ekg::dock::bottom)) {
        new_rect.h = this->rect_delta.h + ((interact.y - new_rect.y) - this->rect_delta.y);
      }
    }

    ekg::set_rect_clamped(new_rect, 30.0f);

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
      }

      ekg::reload(this);
    }
  } else if (resize_dock_flags != ekg::dock::none && this->flag.hovered && !this->flag.activity) {
    ekg::draw::get_visible(this->p_data->get_id(), this->scissor);

    const ekg::vec2 vec_limit_offset {this->ui_theme_activity_offset, this->ui_theme_activity_offset};
    ekg::set_dock_scaled(this->scissor, vec_limit_offset / 4.0f, this->docker_activity_resize);
    shown_cursor_dock_flags = ekg::find_collide_dock(this->docker_activity_resize, resize_dock_flags, interact);
  }

  if (shown_cursor_dock_flags) {
    bool top {ekg_bitwise_contains(shown_cursor_dock_flags, ekg::dock::top)};
    bool bottom {ekg_bitwise_contains(shown_cursor_dock_flags, ekg::dock::bottom)};
    bool left {ekg_bitwise_contains(shown_cursor_dock_flags, ekg::dock::left)};
    bool right {ekg_bitwise_contains(shown_cursor_dock_flags, ekg::dock::right)};

    if ((top && left) || (bottom && right)) {
      ekg::cursor = ekg::system_cursor::size_nwse;
    } else if ((top && right) || (bottom && left)) {
      ekg::cursor = ekg::system_cursor::size_nesw;
    } else if (top || bottom) {
      ekg::cursor = ekg::system_cursor::size_ns;
    } else if (left || right) {
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
  }
}

void ekg::ui::frame_widget::on_draw_refresh() {
  auto &rect {this->get_abs_rect()};
  auto &theme {ekg::theme()};

  if (this->p_scroll_embedded != nullptr) {
    this->p_scroll_embedded->clamp_scroll();
  }

  ekg::draw::sync_scissor(this->scissor, rect, this->p_parent_scissor);
  ekg_draw_assert_scissor();

  ekg::draw::rect(rect, theme.frame_background);
  ekg::draw::rect(this->docker_activity_drag.top, theme.frame_border);
  ekg::draw::rect(rect, theme.frame_outline, ekg::draw_mode::outline);
}