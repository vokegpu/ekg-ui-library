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
#include "ekg/ui/scrollbar/widget.hpp"
#include "ekg/draw/allocator.hpp"
#include "ekg/draw/shape/shape.hpp"
#include "ekg/draw/typography/font.hpp"
#include "ekg/core/runtime.hpp"
#include "ekg/core/pools.hpp"
#include "ekg/math/floating_point.hpp"
#include "ekg/core/context.hpp"
#include "ekg/ui/abstract.hpp"

float ekg::ui::get_horizontal_scrollbar_normalized(
  ekg::property_t &property,
  ekg::rect_t<float> &rect_parent
) {
  return (property.scroll.position.x / rect_parent.w);
}

float ekg::ui::get_vertical_scrollbar_normalized(
  ekg::property_t &property,
  ekg::rect_t<float> &rect_parent
) {
  return (property.scroll.position.y / rect_parent.h);
}

void ekg::ui::check_scrollbar(
  ekg::property_t &property,
  ekg::scrollbar_t &scrollbar,
  ekg::rect_t<float> &rect_parent
) {
  property.scroll.is_enabled.x = scrollbar.rect.w > rect_parent.w;
  property.scroll.is_enabled.y = scrollbar.rect.h > rect_parent.h;
}

void ekg::ui::reset_scrollbar(
  ekg::property_t &property
) {
  property.scroll.position.x = property.scroll.position.z;
  property.scroll.position.y = property.scroll.position.w;
}

void ekg::ui::clamp_scrollbar(
  ekg::property_t &property,
  ekg::scrollbar_t &scrollbar,
  ekg::rect_t<float> &rect_parent
) {
  ekg::vec2_t<float> h_limit {0.0f, rect_parent.w - scrollbar.rect.w};
  if (scrollbar.rect.w < rect_parent.w) {
    property.scroll.position.x = h_limit.x;
    property.scroll.position.z = h_limit.x;
  } else if (property.scroll.position.x < h_limit.y) {
    property.scroll.position.x = h_limit.y;
    property.scroll.position.z = h_limit.y;
  } else if (property.scroll.position.x > h_limit.x) {
    property.scroll.position.x = h_limit.x;
    property.scroll.position.z = h_limit.x;
  }

  ekg::vec2_t<float> v_limit {0.0f, rect_parent.h - scrollbar.rect.h};
  if (scrollbar.rect.h < rect_parent.h) {
    property.scroll.position.y = v_limit.x;
    property.scroll.position.w = v_limit.x;
  } else if (property.scroll.position.y < v_limit.y) {
    property.scroll.position.y = v_limit.y;
    property.scroll.position.w = v_limit.y;
  } else if (property.scroll.position.y > v_limit.x) {
    property.scroll.position.y = v_limit.x;
    property.scroll.position.w = v_limit.x;
  }
}

bool ekg::ui::is_scrollbar_scrolling(
  ekg::property_t &property,
  bool state
) {
  if (
    !(
      state = (
        state
        ||
        (
          (
            static_cast<int32_t>(roundf(property.scroll.position.x))
            !=
            static_cast<int32_t>(roundf(property.scroll.position.z))
          )
          ||
          (
            static_cast<int32_t>(roundf(property.scroll.position.y))
            !=
            static_cast<int32_t>(roundf(property.scroll.position.w))
          )
        )
      )
    )
  ) {
    property.scroll.position.x = property.scroll.position.z;
    property.scroll.position.y = property.scroll.position.w;
  }

  return state;
}

void ekg::ui::reload(
  ekg::property_t &property,
  ekg::scrollbar_t &scrollbar,
  ekg::rect_t<float> &rect_parent,
  std::vector<ekg::at_t> &children,
  bool should_childnizate_metrics
) {
  ekg::ui::check_scrollbar(property, scrollbar, rect_parent);

  if (!should_childnizate_metrics) {
    return;
  }

  scrollbar.rect = {};
  scrollbar.acceleration.x = ekg::draw::get_font_renderer(ekg::font::medium).get_text_height();

  float place {};
  for (ekg::at_t &at : children) {
    ekg_core_widget_call(
      at.flags,
      at,

      scrollbar.rect.w = (
        ekg::max(scrollbar.rect.w, descriptor.rect.x + descriptor.rect.w)
      );

      scrollbar.rect.h = (
        ekg::max(scrollbar.rect.h, descriptor.rect.y + descriptor.rect.h)
      );

      scrollbar.acceleration.y = ekg::min(scrollbar.acceleration.y, descriptor.rect.h);
    );
  }

  scrollbar.rect.h += ekg::p_core->handler_theme.get_current_theme().layout_offset;
  scrollbar.acceleration.y += scrollbar.acceleration.x + (scrollbar.acceleration.x * 0.5f);
  ekg::ui::check_scrollbar(property, scrollbar, rect_parent);
  ekg::ui::clamp_scrollbar(property, scrollbar, rect_parent);
}

void ekg::ui::reload(
  ekg::property_t &property,
  ekg::scrollbar_t &scrollbar
) {
  ekg::property_t &parent {ekg::query<ekg::property_t>(property.parent_at)};
  if (parent == ekg::property_t::not_found) {
    return;
  }

  bool should_childnizate_metrics {true};
  ekg::ui::reload(
    property,
    scrollbar,
    parent.widget.rect,
    parent.children,
    should_childnizate_metrics
  );

  property.nearest_scrollbar_at = scrollbar.at;
  parent.scroll.is_enabled = property.scroll.is_enabled;
  parent.scroll.is_scrolling = property.scroll.is_scrolling;
  parent.scroll.nearest_scroll_bar_thickness = scrollbar.color_scheme.bar_thickness;
}

void ekg::ui::event(
  ekg::property_t &property,
  ekg::scrollbar_t &scrollbar,
  const ekg::io::stage &stage,
  ekg::rect_t<float> &rect_parent
) {
  switch (stage) {
    case ekg::io::stage::pre: {
      ekg::input_info_t &input {ekg::p_core->handler_input.input};
      ekg::vec2_t<float> interact {static_cast<ekg::vec2_t<float>>(input.interact)};

      if (
        input.was_pressed
        ||
        input.was_released
        ||
        input.has_motion
        ||
        input.was_wheel
      ) {
        bool is_visible {
          ekg::rect_collide_vec2<float>(property.widget.rect_scissor, interact)
        };

        property.states.is_active = (
          is_visible
          &&
          (property.scroll.is_enabled.x || property.scroll.is_enabled.y)
          &&
          (ekg::fired("scrollbar-scroll") || ekg::fired("scrollbar-scroll-horizontal"))
        );

        ekg::rect_t<float> h_bar {scrollbar.widget.rect_horizontal};
        h_bar.x += rect_parent.x;

        scrollbar.widget.states_horizontal_bar.is_hovering = (
          is_visible
          &&
          ekg::rect_collide_vec2(h_bar, interact)
        );

        ekg::rect_t<float> v_bar {scrollbar.widget.rect_vertical};
        v_bar.y += rect_parent.y;

        scrollbar.widget.states_vertical_bar.is_hovering = (
          is_visible
          &&
          ekg::rect_precise_collide_vec2(v_bar, interact)
        );

        property.states.is_focused = is_visible;
        property.states.is_hovering = (
          property.states.is_active
          ||
          scrollbar.widget.states_horizontal_bar.is_hovering
          ||
          scrollbar.widget.states_vertical_bar.is_hovering
        );
      }

      property.states.is_absolute = (
        (property.scroll.is_scrolling.x || property.scroll.is_scrolling.y)
        ||
        property.states.is_active
      );

      break;
    }

    case ekg::io::stage::post: {
      property.states.is_hovering = false;
      scrollbar.widget.states_horizontal_bar.is_hovering = false;
      scrollbar.widget.states_vertical_bar.is_hovering = false;
      break;
    }

    case ekg::io::stage::process: {
      ekg::ui::check_scrollbar(
        property,
        scrollbar,
        rect_parent
      );
    
      ekg::input_info_t &input {ekg::p_core->handler_input.input};
      bool is_scroll_fired {ekg::fired("scrollbar-scroll")};
    
      property.scroll.is_scrolling.x = false;
      property.scroll.is_scrolling.y = false;
    
      #if defined(ANDROID)
        if (property.states.is_hovering && property.states.is_enabled.x && is_scroll_fired) {
          property.scroll.is_scrolling.x = true;
          property.scroll.position.z = ekg::clamp<float>(
            property.scroll.position.x + (-input.interact.z * scrollbar.acceleration.y),
            rect_parent.w - scrollbar.rect.w,
            0.0f
          );
        }
    
        if (property.states.is_hovering && property.states.is_enabled.y && is_scroll_fired) {
          property.scroll.is_scrolling.y = true;
          property.scroll.position.w = ekg::clamp<float>(
            property.scroll.position.y + (input.interact.w * scrollbar.acceleration.y),
            rect_parent.h - scrollbar.rect.h,
            0.0f
          );
        }
      #else
        bool is_scroll_horizontal_fired {
          ekg::fired("scrollbar-scroll-horizontal")
        };
    
        if (
          is_scroll_horizontal_fired
          &&
          property.states.is_hovering
          &&
          property.scroll.is_enabled.x
        ) {
          property.scroll.is_scrolling.x = true;
          property.scroll.position.z = ekg::clamp<float>(
            property.scroll.position.x + (input.interact.w * scrollbar.acceleration.x),
            rect_parent.w - scrollbar.rect.w,
            0.0f
          );
        }

        if (
          is_scroll_fired
          &&
          !is_scroll_horizontal_fired
          &&
          property.states.is_hovering
          &&
          property.scroll.is_enabled.y
        ) {
          property.scroll.is_scrolling.y = true;
          property.scroll.position.w = ekg::clamp<float>(
            property.scroll.position.y + (input.interact.w * scrollbar.acceleration.y),
            rect_parent.h - scrollbar.rect.h,
            0.0f
          );
        }
      #endif
    
      if (input.has_motion) {
        ekg_set(
          property.widget.should_buffering,
          scrollbar.widget.states_horizontal_bar.is_highlight,
          scrollbar.widget.states_horizontal_bar.is_hovering
        );
    
        ekg_set(
          property.widget.should_buffering,
          scrollbar.widget.states_vertical_bar.is_highlight,
          scrollbar.widget.states_vertical_bar.is_hovering
        );
      }
    
      if (
        property.states.is_hovering
        &&
        input.was_pressed
        &&
        ekg::fired("scrollbar-drag")
      ) {
        ekg::rect_t<float> h_bar {scrollbar.widget.rect_horizontal};
        h_bar.x += rect_parent.x;
        scrollbar.widget.rect_delta.x = input.interact.x - h_bar.x;
    
        ekg_set(
          property.widget.should_buffering,
          scrollbar.widget.states_horizontal_bar.is_active,
          scrollbar.widget.states_horizontal_bar.is_hovering
        );

        ekg::rect_t<float> v_bar {scrollbar.widget.rect_vertical};
        v_bar.y += rect_parent.y;
        scrollbar.widget.rect_delta.y = input.interact.y - v_bar.y;

        ekg_set(
          property.widget.should_buffering,
          scrollbar.widget.states_vertical_bar.is_active,
          scrollbar.widget.states_vertical_bar.is_hovering
        );
    
        ekg_action(
          scrollbar.actions,
          ekg::action::press,
          (
            scrollbar.widget.states_horizontal_bar.is_active
            ||
            scrollbar.widget.states_vertical_bar.is_active
          )
        );
      }
    
      if (
        (
          scrollbar.widget.states_horizontal_bar.is_focused = (
            input.has_motion
            &&
            scrollbar.widget.states_horizontal_bar.is_active
            &&
            !is_scroll_fired
          )
        )
      ) {
        ekg::rect_t<float> h_bar {scrollbar.widget.rect_horizontal};
        h_bar.x = (input.interact.x - scrollbar.widget.rect_delta.x) - rect_parent.x;

        property.scroll.position.z = (
          -ekg::clamp<float>(
            h_bar.x / (rect_parent.w - scrollbar.widget.rect_horizontal.w),
            0.0f,
            1.0f
          )
          *
          (scrollbar.rect.w - rect_parent.w)
        );

        property.scroll.is_scrolling.x = true;
        property.scroll.position.x = property.scroll.position.z;
        property.widget.should_buffering = true;
        ekg::gui.ui.redraw = true;
      }
    
      if (
        (
          scrollbar.widget.states_vertical_bar.is_focused = (
            scrollbar.widget.states_vertical_bar.is_active
            &&
            input.has_motion
            &&
            !is_scroll_fired
          )
        )
      ) {
        ekg::rect_t<float> v_bar {scrollbar.widget.rect_vertical};
        v_bar.y = (input.interact.y - scrollbar.widget.rect_delta.y) - rect_parent.y;


        property.scroll.position.w = (
          -ekg::clamp<float>(
            v_bar.y / (rect_parent.h - v_bar.h),
            0.0f,
            1.0f
          )
          *
          (scrollbar.rect.h - rect_parent.h)
        );
    
        property.scroll.is_scrolling.y = true;
        property.scroll.position.y = property.scroll.position.w;
        property.widget.should_buffering = true;
        ekg::gui.ui.redraw = true;
      }
    
      if (input.was_released) {
        ekg_action(
          scrollbar.actions,
          ekg::action::release,
          (
            input.was_pressed
            &&
            (
              scrollbar.widget.states_horizontal_bar.is_active
              ||
              scrollbar.widget.states_vertical_bar.is_active
            )
          )
        );
    
        ekg_set(
          property.widget.should_buffering,
          scrollbar.widget.states_horizontal_bar.is_active,
          (property.scroll.is_scrolling.x = false)
        );
    
        ekg_set(
          property.widget.should_buffering,
          scrollbar.widget.states_vertical_bar.is_active,
          (property.scroll.is_scrolling.y = false)
        );
      }
    
      if (scrollbar.widget.states_horizontal_bar.is_hovering || scrollbar.widget.states_vertical_bar.is_hovering) {
        ekg::p_core->p_platform_base->system_cursor = ekg::system_cursor::arrow;
      }
    
      ekg_action(
        scrollbar.actions,
        ekg::action::drag,
        (
          input.has_motion
          &&
          (
            scrollbar.widget.states_horizontal_bar.is_focused
            ||
            scrollbar.widget.states_vertical_bar.is_focused
          )
          &&
          (ekg::timing_t::second > ekg::gui.ui.frequency)
        )
      );
    
      ekg_action(
        scrollbar.actions,
        ekg::action::hover,
        (
          input.has_motion
          &&
          (
            scrollbar.widget.states_horizontal_bar.is_hovering
            ||
            scrollbar.widget.states_vertical_bar.is_hovering
          )
          &&
          (ekg::timing_t::second > ekg::gui.ui.frequency)
        )
      );

      if (property.scroll.is_scrolling.x || property.scroll.is_scrolling.y) {
        ekg::gui.ui.redraw = true;
        property.widget.should_buffering = true;
      }

      break;
    }
  }
}

void ekg::ui::event(
  ekg::property_t &property,
  ekg::scrollbar_t &scrollbar,
  const ekg::io::stage &stage
) {
  ekg::property_t &parent {ekg::query<ekg::property_t>(property.parent_at)};
  if (parent == ekg::property_t::not_found) {
    return;
  }

  ekg::ui::event(
    property,
    scrollbar,
    stage,
    parent.widget.rect
  );

  if (stage == ekg::io::stage::pre) {
    /*
    property.states.is_hovering = (
      !property.states.is_active
      &&
      (
        property.states.is_focused
        ||
        scrollbar.widget.states_horizontal_bar.is_hovering
        ||
        scrollbar.widget.states_vertical_bar.is_hovering
      )
    );
    */
  }

  if (
    (
      property.scroll.is_scrolling.x
      ||
      property.scroll.is_scrolling.y
      ||
      property.states.is_hovering
    )    
    &&
    !property.widget.is_high_frequency
  ) {
    ekg::io::dispatch(
      ekg::io::operation::high_frequency,
      property.at
    );
  }
  
  parent.scroll.is_enabled = property.scroll.is_enabled;
  parent.scroll.is_scrolling = property.scroll.is_scrolling;
  parent.scroll.nearest_scroll_bar_thickness = scrollbar.color_scheme.bar_thickness;
  parent.scroll.position = property.scroll.position;
}

void ekg::ui::high_frequency(
  ekg::property_t &property,
  ekg::scrollbar_t &scrollbar,
  ekg::rect_t<float> &rect_parent
) {
  float speed {
    ekg::p_core->handler_input.input.scroll_speed
  };

  property.scroll.position.x = ekg::lerp<float>(
    property.scroll.position.x,
    property.scroll.position.z,
    ekg::gui.ui.dt * scrollbar.acceleration.x * speed
  );

  property.scroll.position.y = ekg::lerp<float>(
    property.scroll.position.y,
    property.scroll.position.w,
    ekg::gui.ui.dt * scrollbar.acceleration.y * speed
  );

  ekg::ui::clamp_scrollbar(
    property,
    scrollbar,
    rect_parent
  );

  if (
    (
      property.widget.is_high_frequency = ekg::ui::is_scrollbar_scrolling(
        property,
        property.states.is_hovering || property.states.is_active
      )
    )
  ) {
    property.widget.should_buffering = true;
    ekg::gui.ui.redraw = true;
  }
}

void ekg::ui::high_frequency(
  ekg::property_t &property,
  ekg::scrollbar_t &scrollbar
) {
  ekg::property_t &parent {ekg::query<ekg::property_t>(property.parent_at)};
  if (parent == ekg::property_t::not_found) {
    property.widget.is_high_frequency = false;
    return;
  }

  ekg::ui::high_frequency(
    property,
    scrollbar,
    parent.widget.rect
  );
} 

void ekg::ui::pass(
  ekg::property_t &property,
  ekg::scrollbar_t &scrollbar
) {
  ekg_draw_allocator_bind_local(
    &property.widget.geometry_buffer,
    &property.widget.gpu_data_buffer
  );

  if (property.widget.should_buffering) {
    return;
  }

  ekg_draw_allocator_pass();
}

void ekg::ui::buffering(
  ekg::property_t &property,
  ekg::scrollbar_t &scrollbar,
  ekg::rect_t<float> &rect_parent,
  ekg::rect_t<float> &rect_parent_scissor
) {
  scrollbar.widget.rect_horizontal.w = 0.0f;
  scrollbar.widget.rect_vertical.h = 0.0f;

  property.scroll.is_enabled.x = scrollbar.rect.w > rect_parent.w;
  property.scroll.is_enabled.y = scrollbar.rect.h > rect_parent.h;

  if (
    !property.scroll.is_enabled.x
    &&
    !property.scroll.is_enabled.y
  ) {
    return;
  }

  ekg::rect_t<float> bar {};

  /* start of horizontal bar */

  scrollbar.widget.rect_horizontal.h = static_cast<float>(
    scrollbar.color_scheme.bar_thickness * property.scroll.is_enabled.x
  );

  scrollbar.widget.rect_horizontal.y = (
    rect_parent.y
    +
    rect_parent.h
    -
    scrollbar.widget.rect_horizontal.h
  );

  float out_of_parent_rect_width {scrollbar.rect.w - rect_parent.w};
  float x_factor {abs(property.scroll.position.x) / out_of_parent_rect_width};

  scrollbar.widget.rect_horizontal.w = (
    rect_parent.w
    -
    (
      static_cast<ekg::pixel_t>(out_of_parent_rect_width) < 0
      ?
      rect_parent.w
      :
      ekg::clamp_max<float>(
        out_of_parent_rect_width,
        rect_parent.w - static_cast<float>(scrollbar.color_scheme.bar_size_limit)
      )
    )
  );

  scrollbar.widget.rect_horizontal.x = x_factor * (rect_parent.w - scrollbar.widget.rect_horizontal.w);

  ekg::draw::rect(
    rect_parent.x,
    scrollbar.widget.rect_horizontal.y,
    rect_parent.w,
    scrollbar.widget.rect_horizontal.h,
    scrollbar.color_scheme.background,
    ekg::draw::mode::fill,
    scrollbar.layers[ekg::layer::bg]
  );

  ekg::draw::rect(
    rect_parent.x,
    scrollbar.widget.rect_horizontal.y,
    rect_parent.w,
    scrollbar.widget.rect_horizontal.h,
    scrollbar.color_scheme.outline,
    ekg::draw::mode::outline,
    scrollbar.layers[ekg::layer::outline]
  );

  bar.x = rect_parent.x + scrollbar.widget.rect_horizontal.x;
  bar.y = scrollbar.widget.rect_horizontal.y;
  bar.w = scrollbar.widget.rect_horizontal.w;
  bar.h = scrollbar.widget.rect_horizontal.h;

  ekg::draw::rect(
    bar,
    scrollbar.color_scheme.bar_background,
    ekg::draw::mode::fill,
    scrollbar.layers[ekg::layer::bg]
  );

  if (scrollbar.widget.states_horizontal_bar.is_highlight) {
    ekg::draw::rect(
      bar,
      scrollbar.color_scheme.bar_highlight,
      ekg::draw::mode::fill,
      scrollbar.layers[ekg::layer::highlight_bg]
    );
  }

  if (scrollbar.widget.states_horizontal_bar.is_active) {
    ekg::draw::rect(
      bar,
      scrollbar.color_scheme.bar_active,
      ekg::draw::mode::fill,
      scrollbar.layers[ekg::layer::active_bg]
    );
  }

  /* start of vertical bar draw */

  scrollbar.widget.rect_vertical.w = static_cast<float>(
    scrollbar.color_scheme.bar_thickness * property.scroll.is_enabled.y
  );

  scrollbar.widget.rect_vertical.x = (
    rect_parent.x
    +
    rect_parent.w
    -
    scrollbar.widget.rect_vertical.w
  );

  float out_of_parent_rect_height {scrollbar.rect.h - rect_parent.h};
  float y_factor {abs(property.scroll.position.y) / out_of_parent_rect_height};

  scrollbar.widget.rect_vertical.h = (
    rect_parent.h
    -
    (
      static_cast<ekg::pixel_t>(out_of_parent_rect_height) < 0
      ?
      rect_parent.h
      :
      ekg::clamp_max<float>(
        out_of_parent_rect_height,
        rect_parent.h - static_cast<float>(scrollbar.color_scheme.bar_size_limit)
      )
    )
  );

  scrollbar.widget.rect_vertical.y = y_factor * (rect_parent.h - scrollbar.widget.rect_vertical.h);

  ekg::draw::rect(
    scrollbar.widget.rect_vertical.x,
    rect_parent.y,
    scrollbar.widget.rect_vertical.w,
    rect_parent.h,
    scrollbar.color_scheme.background,
    ekg::draw::mode::fill,
    scrollbar.layers[ekg::layer::bg]
  );

  ekg::draw::rect(
    scrollbar.widget.rect_vertical.x,
    rect_parent.y,
    scrollbar.widget.rect_vertical.w,
    rect_parent.h,
    scrollbar.color_scheme.outline,
    ekg::draw::mode::outline,
    scrollbar.layers[ekg::layer::outline]
  );

  bar.x = scrollbar.widget.rect_vertical.x;
  bar.y = rect_parent.y + scrollbar.widget.rect_vertical.y;
  bar.w = scrollbar.widget.rect_vertical.w;
  bar.h = scrollbar.widget.rect_vertical.h;

  ekg::draw::rect(
    bar,
    scrollbar.color_scheme.bar_background,
    ekg::draw::mode::fill,
    scrollbar.layers[ekg::layer::bg]
  );

  if (scrollbar.widget.states_vertical_bar.is_highlight) {
    ekg::draw::rect(
      bar,
      scrollbar.color_scheme.bar_highlight,
      ekg::draw::mode::fill,
      scrollbar.layers[ekg::layer::highlight_bg]
    );
  }

  if (scrollbar.widget.states_vertical_bar.is_active) {
    ekg::draw::rect(
      bar,
      scrollbar.color_scheme.bar_active,
      ekg::draw::mode::fill,
      scrollbar.layers[ekg::layer::active_bg]
    );
  }

  ekg::draw::rect(
    bar,
    scrollbar.color_scheme.bar_outline,
    ekg::draw::mode::outline,
    scrollbar.layers[ekg::layer::outline]
  );
}

void ekg::ui::buffering(
  ekg::property_t &property,
  ekg::scrollbar_t &scrollbar
) {
  ekg::property_t &property_parent {
    ekg::query<ekg::property_t>(property.parent_at)
  };

  ekg_draw_allocator_assert_scissor(
    property.widget.rect_scissor,
    property_parent.widget.rect,
    property_parent.widget.rect_scissor,
    ekg::always_parented
  );

  ekg::ui::buffering(
    property,
    scrollbar,
    property_parent.widget.rect,
    property_parent.widget.rect_scissor
  );

  ekg_draw_allocator_pass();
}

void ekg::ui::unmap(
  ekg::scrollbar_t &scrollbar
) {

}
