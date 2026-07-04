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
#include "ekg/ui/button/widget.hpp"
#include "ekg/layout/docknize.hpp"
#include "ekg/io/log.hpp"
#include "ekg/ui/abstract.hpp"
#include "ekg/draw/shape/shape.hpp"
#include "ekg/draw/typography/font.hpp"
#include "ekg/core/context.hpp"
#include "ekg/core/runtime.hpp"
#include "ekg/core/pools.hpp"

void ekg::ui::reload(
  ekg::property_t &property,
  ekg::button_t &button
) {
  ekg::ui::get_abs_rect(
    property,
    button.rect
  );

  ekg::axis pick_axis {
    ekg::axis::horizontal
  };

  if (button.checks.empty()) {
    button.checks.push_back(
      {}
    );
  }

  ekg::theme_t &global_theme {ekg::p_core->handler_theme.get_current_theme()};
  ekg::aligned_t aligned_dimension {};

  float width {};
  for (ekg::button_t::check_t &check : button.checks) {
    ekg::draw::font &draw_font {
      ekg::draw::get_font_renderer(check.font_size)
    };

    check.widget.rect_text.w = draw_font.get_text_width(check.text.get());
    check.widget.rect_text.h = draw_font.get_text_height();

    aligned_dimension = {};
    ekg::align_rect_dimension(
      pick_axis,
      check.widget.rect_text,
      ekg::dpi.min_sizes,
      aligned_dimension
    );

    check.widget.rect_box = check.widget.rect_text;
    check.widget.rect_box.w += global_theme.layout_offset * 2.0f;

    if (check.box == ekg::dock::none) {
      check.widget.rect_text.x = (check.widget.rect_text.w / 2) - (check.widget.rect_box.w / 2);
      //check.widget.rect_box.h = aligned_dimension.h;
    }
  }

  button.rect.scaled_height = ekg::max<ekg::pixel_thickness_t>(1, button.rect.scaled_height);
  button.rect.h = aligned_dimension.h * button.rect.scaled_height;

  ekg::layout::mask mask {};
  mask.preset(
    {
      aligned_dimension.offset,
      aligned_dimension.offset,
      button.rect.h
    },
    pick_axis,
    button.rect.w
  );

  for (ekg::button_t::check_t &check : button.checks) {
    ekg::draw::font &draw_font {
      ekg::draw::get_font_renderer(check.font_size)
    };

    if (check.box != ekg::dock::none) {
      check.widget.rect_box.w = check.widget.rect_text.h;
      check.widget.rect_box.h = check.widget.rect_text.h;

      mask.insert(
        {
          .p_rect = &check.widget.rect_box,
          .dock = check.box
        }
      );
    }

    mask.insert(
      {
        .p_rect = &check.widget.rect_text,
        .dock = check.dock
      }
    );
  }

  mask.docknize();

  if (property.widget.should_refresh_size) {
    button.rect.w = ekg::max(ekg::dpi.min_sizes, mask.get_rect().w);
    property.widget.should_buffering = true;
    property.widget.should_refresh_size = false;
  }
}

void ekg::ui::event(
  ekg::property_t &property,
  ekg::button_t &button,
  const ekg::io::stage &stage
) {
  switch (stage) {
    case ekg::io::stage::process: {
      ekg::input_info_t &input {ekg::p_core->handler_input.input};
      if (!input.has_motion && !input.was_pressed && !input.was_released) {
        return;
      }

      ekg::vec2_t interact {static_cast<ekg::vec2_t<float>>(input.interact)};
      ekg::rect_t<float> &abs {ekg::ui::get_abs_rect(property, button.rect)};
      ekg::rect_t<float> rect {};

      bool was_pressed_any {};
      bool was_released_any {};
      bool is_hovering_any {};
      bool is_active_any {};
      bool is_checks_size_equals_to_one {button.checks.size() == 1};
      bool is_checkbox {};

      for (ekg::button_t::check_t &check : button.checks) {
        rect = (
          is_checks_size_equals_to_one
            ?
            property.widget.rect
            :
            check.widget.rect_box + property.widget.rect
        );

        is_checkbox = check.box != ekg::dock::none;

        ekg_set(
          property.widget.should_buffering,
          check.states.is_highlight,
          (property.states.is_hovering && ekg::rect_collide_vec2(rect, interact))
        );

        ekg_action(
          check.actions,
          ekg::action::hover,
          (
            check.states.is_highlight
          )
          &&
          (is_hovering_any = true)
          &&
          ekg::timing_t::second > ekg::gui.ui.frequency
        );

        if (
          input.was_pressed
          &&
          check.states.is_highlight
          &&
          ekg::fired("button-active")
        ) {
          ekg_action(
            check.actions,
            ekg::action::press,
            (
              ekg_set(
                property.widget.should_buffering,
                check.states.is_active,
                true
              )
            )
            &&
            (was_pressed_any = true)
          );

          if (!is_checkbox) {
            property.states.is_sensitive = true;
            check.value.set(true);
          }
        }

        if (
          input.was_released
          &&
          check.states.is_active
        ) {
          ekg_set(
            property.widget.should_buffering,
            check.states.is_active,
            false
          );

          ekg_action(
            check.actions,
            ekg::action::active,
            check.states.is_highlight
            &&
            (is_checkbox ? (check.value.set(!check.value.get())) : (check.value.set(true)))
            &&
            (is_active_any = true)
          );

          if (!is_checkbox) {
            property.states.is_sensitive = false;
            check.value.set(false);
          }

          ekg_action(
            check.actions,
            ekg::action::release,
            (
              true
            )
            &&
            (was_released_any = true)
          );
        }
      }
  
      property.states.is_highlight = is_hovering_any;

      ekg_action(
        button.actions,
        ekg::action::hover,
        (
          is_hovering_any && ekg::timing_t::second > ekg::gui.ui.frequency
        )
      );

      ekg_action(
        button.actions,
        ekg::action::active,
        (
          is_active_any
        )
      );

      ekg_action(
        button.actions,
        ekg::action::press,
        (
          was_pressed_any
        )
      );

      ekg_action(
        button.actions,
        ekg::action::release,
        (
          was_released_any
        )
      );

      break;
    }
  case ekg::io::stage::pre:
    ekg::ui::pre_event(
      property,
      button.rect,
      false
    );
    break;
  case ekg::io::stage::post:
    ekg::ui::post_event(
      property
    );
    break;
  }
}

void ekg::ui::high_frequency(
  ekg::property_t &property,
  ekg::button_t &button
) {

}

void ekg::ui::pass(
  ekg::property_t &property,
  ekg::button_t &button
) {
  ekg_draw_allocator_bind_local(
    &property.widget.geometry_buffer,
    &property.widget.gpu_data_buffer
  );

  if (property.widget.should_buffering) {
    return;
  }

  for (ekg::button_t::check_t &check : button.checks) {
    if (check.value.was_changed()) {
      property.widget.should_buffering = true;
      return;
    }
  }

  ekg_draw_allocator_pass();
}

void ekg::ui::buffering(
  ekg::property_t &property,
  ekg::button_t &button
) {
  ekg::rect_t<float> &rect_abs {
    ekg::ui::get_abs_rect(property, button.rect)
  };

  ekg_draw_allocator_assert_scissor(
    property.widget.rect_scissor,
    rect_abs,
    ekg::query<ekg::property_t>(property.parent_at).widget.rect_scissor,
    ekg::always_parented
  );

  ekg::draw::rect(
    rect_abs,
    button.color_scheme.background,
    ekg::draw::mode::fill,
    button.layers[ekg::layer::bg]
  );

  ekg::rect_t<float> rect {};

  bool is_checks_size_equals_to_one {button.checks.size() == 1};
  bool is_checkbox {};

  for (ekg::button_t::check_t &check : button.checks) {
    is_checkbox = check.box != ekg::dock::none;
    rect = (
      is_checks_size_equals_to_one
        ?
        property.widget.rect
        :
        check.widget.rect_box + property.widget.rect
    );

    if (is_checkbox) {
      ekg::draw::rect(
        rect,
        button.color_scheme.box_background,
        ekg::draw::mode::fill,
        check.layers[ekg::layer::bg]
      );

      if (check.value.get()) {
        ekg::draw::rect(
          rect,
          button.color_scheme.box_active,
          ekg::draw::mode::fill,
          check.layers[ekg::layer::active_bg]
        );
      }

      if (check.states.is_highlight) {
        ekg::draw::rect(
          rect,
          button.color_scheme.box_highlight,
          ekg::draw::mode::fill,
          check.layers[ekg::layer::highlight_bg]
        );
      }

      ekg::draw::rect(
        rect,
        button.color_scheme.box_outline,
        ekg::draw::mode::outline,
        check.layers[ekg::layer::outline]
      );

      ekg::draw::get_font_renderer(check.font_size)
        .blit(
          check.text.get(),
          rect_abs.x + check.widget.rect_text.x, rect_abs.y + check.widget.rect_text.y,
          button.color_scheme.text_foreground
        );
    } else {
      if (!is_checks_size_equals_to_one) {
        ekg::draw::rect(
          rect,
          button.color_scheme.background,
          ekg::draw::mode::fill,
          check.layers[ekg::layer::bg]
        );
      }

      if (check.value.get()) {
        ekg::draw::rect(
          rect,
          button.color_scheme.active,
          ekg::draw::mode::fill,
          check.layers[ekg::layer::active_bg]
        );
      }

      if (check.states.is_highlight) {
        ekg::draw::rect(
          rect,
          button.color_scheme.highlight,
          ekg::draw::mode::fill,
          check.layers[ekg::layer::highlight_bg]
        );
      }

      ekg::draw::get_font_renderer(check.font_size)
        .blit(
          check.text.get(),
          rect_abs.x + check.widget.rect_text.x, rect_abs.y + check.widget.rect_text.y,
          button.color_scheme.text_foreground
        );
    }
  }

  ekg::draw::rect(
    rect_abs,
    button.color_scheme.outline,
    ekg::draw::mode::outline,
    button.layers[ekg::layer::bg]
  );

  ekg_draw_allocator_pass();
}

void ekg::ui::unmap(
  ekg::button_t &button
) {
  for (ekg::button_t::check_t &check : button.checks) {
    check.text.ownership(nullptr);
    check.value.ownership(nullptr);
  }
}
