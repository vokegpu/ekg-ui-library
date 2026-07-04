/**
 * MIT License
 * 
 * Copyright (c) 2022-2025 Rina Wilk / vokegpu@gmail.com
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
#include "ekg/ui/frame/widget.hpp"
#include "ekg/ui/abstract.hpp"
#include "ekg/handler/input.hpp"
#include "ekg/core/runtime.hpp"
#include "ekg/handler/callback.hpp"
#include "ekg/core/context.hpp"
#include "ekg/core/pools.hpp"
#include "ekg/layout/docknize.hpp"
#include "ekg/draw/shape/shape.hpp"
#include "ekg/io/log.hpp"

void ekg::ui::reload(
  ekg::property_t &property,
  ekg::frame_t &frame
) {
  ekg::ui::get_abs_rect(property, frame.rect);

  if (
    (
      property.widget.should_refresh_size
      &&
      static_cast<ekg::pixel_t>(frame.rect.h) == 0
    )
    ||
    frame.color_scheme.popup_mode
  ) {
    frame.rect.h = ekg::layout::get_widget_height_by_children(
      property
    );

    property.widget.should_buffering = true;
  }

  if (frame.color_scheme.popup_mode) {
    frame.rect.h = ekg::min<float>(frame.rect.h, frame.color_scheme.max_popup_height);
  }

  property.widget.should_refresh_size = false;
}

void ekg::ui::event(
  ekg::property_t &property,
  ekg::frame_t &frame,
  const ekg::io::stage &stage
) {
  switch (stage) {
    case ekg::io::stage::pre:
      ekg::ui::pre_event(property, frame.rect, frame.set_top_level);
      break;

    case ekg::io::stage::post:
      ekg::ui::post_event(property);
      break;

    case ekg::io::stage::process: {
      ekg::rect_t<float> &rect {ekg::ui::get_abs_rect(property, frame.rect)};
      ekg::input_info_t &input {ekg::p_core->handler_input.input};
      ekg::flags_t resize_over_dock {};

      ekg_action(
        frame.actions,
        ekg::action::hover,
        (
          input.has_motion
          &&
          property.states.is_hovering
          &&
          (ekg::timing_t::second && ekg::gui.ui.frequency)
        )
      );

      if (
          input.was_pressed
          &&
          (
            (frame.drag != ekg::dock::none && ekg::fired("frame-drag"))
            ||
            (frame.resize != ekg::dock::none && ekg::fired("frame-resize"))
          )
      ) {

        ekg::vec2_t<float> margin {
          static_cast<float>(frame.color_scheme.actions_margin_pixel_thickness),
          static_cast<float>(frame.color_scheme.actions_margin_pixel_thickness)
        };

        ekg::scale_docker_by_margin<float>(
          frame.widget.docker_drag,
          margin,
          rect
        );

        const float margin_factor {4.0f};
        ekg::scale_docker_by_margin<float>(
          frame.widget.docker_resize,
          margin / margin_factor,
          rect
        );

        frame.widget.target_dock_drag = ekg::vec2_collide_docker_if<float>(
          static_cast<ekg::vec2_t<float>>(input.interact),
          frame.widget.docker_drag,
          frame.drag
        );
      
        frame.widget.target_dock_resize = ekg::vec2_collide_docker_if<float>(
          static_cast<ekg::vec2_t<float>>(input.interact),
          frame.widget.docker_resize,
          frame.resize
        );

        resize_over_dock = frame.widget.target_dock_resize;

        frame.widget.rect_delta.x = input.interact.x - rect.x;
        frame.widget.rect_delta.y = input.interact.y - rect.y;
        frame.widget.rect_delta.w = rect.w;
        frame.widget.rect_delta.h = rect.h;
        frame.widget.rect_cache = rect;

        property.states.is_active = (
          frame.widget.target_dock_drag != ekg::dock::none
          ||
          frame.widget.target_dock_resize != ekg::dock::none
        );

        property.states.is_absolute = property.states.is_active;

        ekg_action(
          frame.actions,
          ekg::action::press,
          true
        );
      } else if (input.has_motion && property.states.is_active) {
        ekg::rect_t<float> new_rect {rect};
        ekg::vec2_t<float> interact {static_cast<ekg::vec2_t<float>>(input.interact)};

        resize_over_dock = frame.widget.target_dock_resize;

        if (frame.widget.target_dock_drag != ekg::dock::none && frame.widget.target_dock_resize == ekg::dock::none) {
          ekg_action(
            frame.actions,
            ekg::action::drag,
            ekg::timing_t::second > ekg::gui.ui.frequency
          );

          new_rect.x = interact.x - frame.widget.rect_delta.x;
          new_rect.y = interact.y - frame.widget.rect_delta.y;
          ekg::p_core->p_platform_base->system_cursor = ekg::system_cursor::size_all;
        }

        if (frame.widget.target_dock_resize != ekg::dock::none) {
          ekg_action(
            frame.actions,
            ekg::action::resize,
            ekg::timing_t::second > ekg::gui.ui.frequency
          );

          if (ekg::has(frame.widget.target_dock_resize, ekg::dock::left)) {
            interact.x = ekg::clamp_min<float>(interact.x, frame.widget.rect_delta.x);
            new_rect.x = ekg::clamp_max<float>(interact.x - frame.widget.rect_delta.x, frame.widget.rect_cache.x + frame.widget.rect_cache.w - ekg::dpi.min_sizes);
            new_rect.w = (frame.widget.rect_cache.x + frame.widget.rect_cache.w) - new_rect.x;
          }

          if (ekg::has(frame.widget.target_dock_resize, ekg::dock::right)) {
            new_rect.w = ekg::clamp_min<float>(frame.widget.rect_delta.w + ((interact.x - new_rect.x) - frame.widget.rect_delta.x), ekg::dpi.min_sizes);
          }

          if (ekg::has(frame.widget.target_dock_resize, ekg::dock::top)) {
            interact.y = ekg::clamp_min<float>(interact.y, frame.widget.rect_delta.y);
            new_rect.y = ekg::clamp_max<float>(interact.y - frame.widget.rect_delta.y, frame.widget.rect_cache.y + frame.widget.rect_cache.h - ekg::dpi.min_sizes);
            new_rect.h = (frame.widget.rect_cache.y + frame.widget.rect_cache.h) - new_rect.y;
          }

          if (ekg::has(frame.widget.target_dock_resize, ekg::dock::bottom)) {
            new_rect.h = ekg::clamp_min<float>(frame.widget.rect_delta.h + ((interact.y - new_rect.y) - frame.widget.rect_delta.y), ekg::dpi.min_sizes);
          }
        }

        new_rect = ekg::clamp_rect_by_square<float>(
          new_rect,
          ekg::dpi.min_sizes
        );

        if (rect != new_rect) {
          if (property.parent_at != ekg::at_t::not_found) {
            ekg::property_t &parent_property {ekg::query<ekg::property_t>(property.parent_at)};
            frame.rect.x = new_rect.x - parent_property.widget.rect.x;
            frame.rect.y = new_rect.y - parent_property.widget.rect.y;
          } else {
            frame.rect.x = (int32_t) new_rect.x;
            frame.rect.y = (int32_t) new_rect.y;
          }

          frame.rect.w = (int32_t) new_rect.w;
          frame.rect.h = (int32_t) new_rect.h;

          if (frame.widget.target_dock_resize != ekg::dock::none) {
            property.widget.is_targeting_absolute_parent = true;
            ekg::io::dispatch(
              ekg::io::operation::docknize,
              frame.at
            );
          }

          ekg::gui.ui.redraw = true;
        }
      } else if (
        frame.resize != ekg::dock::none
        &&
        !property.states.is_active
      ) {
        ekg::vec2_t<float> margin {
          static_cast<float>(frame.color_scheme.actions_margin_pixel_thickness),
          static_cast<float>(frame.color_scheme.actions_margin_pixel_thickness)
        };

        const float margin_factor {4.0f};
        ekg::scale_docker_by_margin<float>(
          frame.widget.docker_resize,
          margin / margin_factor,
          rect
        );

        resize_over_dock = ekg::vec2_collide_docker_if<float>(
          static_cast<ekg::vec2_t<float>>(input.interact),
          frame.widget.docker_resize,
          frame.resize
        );
      }

      if (resize_over_dock != ekg::dock::none) {
        bool is_top {ekg::has(resize_over_dock, ekg::dock::top)};
        bool is_bottom {ekg::has(resize_over_dock, ekg::dock::bottom)};
        bool is_left {ekg::has(resize_over_dock, ekg::dock::left)};
        bool is_right {ekg::has(resize_over_dock, ekg::dock::right)};

        if ((is_top && is_left) || (is_bottom && is_right)) {
          ekg::p_core->p_platform_base->system_cursor = ekg::system_cursor::size_nwse;
        } else if ((is_top && is_right) || (is_bottom && is_left)) {
          ekg::p_core->p_platform_base->system_cursor = ekg::system_cursor::size_nesw;
        } else if (is_top || is_bottom) {
          ekg::p_core->p_platform_base->system_cursor = ekg::system_cursor::size_ns;
        } else if (is_left || is_right) {
          ekg::p_core->p_platform_base->system_cursor = ekg::system_cursor::size_we;
        }
      }

      if (input.was_released) {
        if (property.states.is_active) {
          ekg_action(
            frame.actions,
            ekg::action::release,
            property.states.is_hovering
          );
        }

        frame.widget.target_dock_resize = ekg::dock::none;
        frame.widget.target_dock_drag = ekg::dock::none;
        property.states.is_active = false;
        property.states.is_absolute = false;
      }

      break;
    }
  }
}

void ekg::ui::high_frequency(
  ekg::property_t &property,
  ekg::frame_t &frame
) {

}

void ekg::ui::pass(
  ekg::property_t &property,
  ekg::frame_t &frame
) {
  ekg_draw_allocator_bind_local(&property.widget.geometry_buffer, &property.widget.gpu_data_buffer);

  if (property.widget.should_buffering) {
    return;
  }

  ekg_draw_allocator_pass();
}

void ekg::ui::buffering(
  ekg::property_t &property,
  ekg::frame_t &frame
) {
  ekg::rect_t<float> &rect {ekg::ui::get_abs_rect(property, frame.rect)};

  ekg_draw_allocator_assert_scissor(
    property.widget.rect_scissor,
    rect,
    ekg::query<ekg::property_t>(property.parent_at).widget.rect_scissor,
    property.parent_at != ekg::at_t::not_found
  );

  /**
   * This allows scrolling and dragging behavior possible,
   * every child from this frame will be spaced always
   * ahead of margin.
   **/
  float margin_bound {frame.color_scheme.margin * 2.0f};
  property.widget.rect_scissor.x += frame.color_scheme.margin;
  property.widget.rect_scissor.y += frame.color_scheme.margin;
  property.widget.rect_scissor.w -= margin_bound;
  property.widget.rect_scissor.h -= margin_bound;

  ekg::draw::rect(
    rect,
    property.states.is_focused ? frame.color_scheme.focused_background : frame.color_scheme.background,
    ekg::draw::mode::fill,
    frame.layers[ekg::layer::bg]
  );

  if (property.states.is_active) {
    ekg::draw::rect(
      rect,
      frame.color_scheme.highlight,
      ekg::draw::mode::fill,
      frame.layers[ekg::layer::active_bg]
    );
  }

  if (property.states.is_highlight) {
    ekg::draw::rect(
      rect,
      frame.color_scheme.highlight,
      ekg::draw::mode::fill,
      frame.layers[ekg::layer::highlight_bg]
    );
  }

  ekg::draw::rect(
    rect,
    property.states.is_focused ? frame.color_scheme.focused_outline : frame.color_scheme.outline,
    ekg::draw::mode::outline,
    frame.layers[ekg::layer::outline]
  );

  if (property.states.is_warning) {
    ekg::draw::rect(
      rect,
      frame.color_scheme.warning_outline,
      ekg::draw::mode::outline,
      frame.layers[ekg::layer::warning_outline]
    );
  }

  ekg_draw_allocator_pass();
}

void ekg::ui::unmap(
  ekg::frame_t &frame
) {

}
