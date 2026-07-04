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
#include "ekg/core/runtime.hpp"
#include "ekg/layout/scalenize.hpp"
#include "ekg/io/log.hpp"
#include "ekg/core/pools.hpp"
#include "ekg/core/context.hpp"
#include "ekg/layout/docknize.hpp"

void ekg::core::swap_collector(
  bool &was_found,
  ekg::at_t &property_at
) {
  if (property_at == ekg::at_t::not_found) {
    return;
  }

  ekg::property_t &parent_property {
    ekg::query<ekg::property_t>(property_at)
  };

  if (
    parent_property.at == ekg::gui.bind.swap_at
    ||
    parent_property.is_stack_top_level
  ) {
    was_found = true;
  }

  ekg::p_core->collector.push_back(property_at);

  if (parent_property.is_childnizate && parent_property.is_children_docknizable) {
    for (ekg::at_t &at : parent_property.children) {
      ekg::property_t &property {ekg::query<ekg::property_t>(at)};
      if (property == ekg::property_t::not_found) {
        continue;
      }

      ekg::core::swap_collector(was_found, at);
    }
  }
}

void ekg::core::swap(ekg::info_t &info) {
  if (
    ekg::p_core == nullptr
    ||
    ekg::gui.bind.swap_at == ekg::at_t::not_found
  ) {
    return;
  }

  ekg::p_core->stack.clear();

  bool was_found {};
  for (ekg::at_t &at : ekg::p_core->registry) {
    ekg::property_t &property {ekg::query<ekg::property_t>(at)};

    if (
      property == ekg::property_t::not_found
      ||
      property.parent_at != ekg::at_t::not_found
    ) {
      continue;
    }

    // recurse
    was_found = false;
    ekg::p_core->collector.clear();
    ekg::core::swap_collector(was_found, at);

    if (was_found) {
      ekg::p_core->top_level_stack.insert(
        ekg::p_core->top_level_stack.begin(),
        ekg::p_core->collector.begin(),
        ekg::p_core->collector.end()
      );

      ekg::io::dispatch(ekg::io::operation::docknize, property.at);
      continue;
    }

    ekg::p_core->stack.insert(
      ekg::p_core->stack.begin(),
      ekg::p_core->collector.begin(),
      ekg::p_core->collector.end()
    );
  }

  ekg::p_core->stack.insert(
    ekg::p_core->stack.end(),
    ekg::p_core->top_level_stack.begin(),
    ekg::p_core->top_level_stack.end()
  );

  ekg::p_core->top_level_stack.clear();
  ekg::p_core->collector.clear();
  ekg::gui.bind.swap_at = ekg::at_t::not_found;
}

void ekg::core::reload(ekg::info_t &info) {
  if (ekg::p_core == nullptr) {
    return;
  }

  for (ekg::at_t &at : ekg::p_core->reload) {
    ekg::property_t &property {ekg::query<ekg::property_t>(at)};
    if (property == ekg::property_t::not_found || !property.operation.should_reload) {
      continue;
    }

    ekg_core_widget_call(
      property.descriptor_at.flags,
      property.descriptor_at,
      ekg::ui::reload(property, descriptor);
    );

    property.operation.should_reload = false;
  }

  ekg::p_core->reload.clear();
}

void ekg::core::docknize(ekg::info_t &info) {
  if (ekg::p_core == nullptr) {
    return;
  }

  for (ekg::at_t &at : ekg::p_core->docknize) {
    ekg::property_t &property {ekg::query<ekg::property_t>(at)};
    if (property == ekg::property_t::not_found || !property.operation.should_docknize) {
      continue;
    }

    ekg::layout::docknize_widget(
      property
    );

    property.operation.should_docknize = false;
  }

  ekg::p_core->reload.clear();
}

void ekg::core::scalenize(ekg::info_t &info) {
  if (ekg::p_core == nullptr) {
    return;
  }

  ekg::layout::scalenize();

  ekg::dpi.font_scale = ekg::clamp<float>(
    ekg::dpi.font_scale,
    static_cast<float>(4),
    static_cast<float>(UINT8_MAX)
  );

  uint32_t font_size {
    ekg::clamp<uint32_t>(
      static_cast<uint32_t>(
        ekg::dpi.font_scale
        *
        ekg::dpi.factor_scale
      ),
      0,
      UINT8_MAX
    )
  };

  if (ekg::p_core->draw_font_medium.font_size != font_size) {
    ekg::log() << "Scalenize font size updated from-to: " << ekg::p_core->draw_font_medium.font_size << ' ' << font_size;

    ekg::p_core->draw_font_small.set_size(
      ekg::clamp_min(
        font_size - ekg::dpi.font_offset.x,
        ekg::minimum_small_font_height
      )
    );

    ekg::p_core->draw_font_medium.set_size(
      ekg::clamp_min(
        font_size,
        ekg::minimum_font_height
      )
    );

    ekg::p_core->draw_font_big.set_size(
      ekg::clamp_min(
        font_size + ekg::dpi.font_offset.y,
        ekg::minimum_big_font_height
      )
    );
  }

  for (ekg::at_t &at : ekg::p_core->stack) {
    ekg::property_t &property {ekg::query<ekg::property_t>(at)};
    if (property == ekg::property_t::not_found) {
      continue;
    }

    ekg_core_widget_call(
      property.descriptor_at.flags,
      property.descriptor_at,

      ekg::ui::reload(property, descriptor);

      if (!property.is_children_docknizable) {
        continue;
      }
    );

    property.widget.should_refresh_size = true;
    ekg::io::dispatch(ekg::io::operation::docknize, at);
  }
}

void ekg::core::poll_event() {
  if (ekg::p_core == nullptr) {
    return;
  }

  ekg::p_core->handler_input.poll_event();

  ekg::input_info_t &input {
    ekg::p_core->handler_input.input
  };

  bool is_on_scrolling_timeout {!ekg::reach(input.ui_scrolling_timing, 100)};
  ekg::property_t &abs_widget {ekg::query<ekg::property_t>(ekg::gui.ui.abs_widget_at)};
  if (
      abs_widget != ekg::property_t::not_found
      &&
      (abs_widget.states.is_absolute || is_on_scrolling_timeout)
    ) {

    ekg_core_widget_call(
      abs_widget.descriptor_at.flags,
      abs_widget.descriptor_at,

      ekg::ui::event(abs_widget, descriptor, ekg::io::stage::pre);
      ekg::ui::event(abs_widget, descriptor, ekg::io::stage::process);
    
      if (
        abs_widget.scroll.is_scrolling.x
        ||
        abs_widget.scroll.is_scrolling.y
      ) {
        ekg::reset(input.ui_scrolling_timing);
      }

      ekg::ui::event(abs_widget, descriptor, ekg::io::stage::post);
    );

    return;
  }

  if (is_on_scrolling_timeout) {
    return;
  }

  ekg::gui.ui.abs_widget_at = ekg::at_t::not_found;

  bool hovered {};
  bool first_absolute {};

  ekg::at_t focused_at {ekg::at_t::not_found};
  for (ekg::at_t &at : ekg::p_core->stack) {
    ekg::property_t &property {
      ekg::query<ekg::property_t>(at)
    };

    if (property == ekg::property_t::not_found) {
      continue;
    }

    ekg_core_widget_call(
      property.descriptor_at.flags,
      property.descriptor_at,

      ekg::ui::event(property, descriptor, ekg::io::stage::pre);

      hovered = (
        !(
          false
          //ekg::p_core->p_platform_base->event.type == ekg::io::event_type::key_down
          //||
          //ekg::p_core->p_platform_base->event.type == ekg::io::event_type::key_up
          //||
          //ekg::p_core->p_platform_base->event.type == ekg::io::event_type::text_input
        )
        &&
        property.states.is_hovering
        &&
        property.states.is_visible
        &&
        property.states.is_enabled
      );

      if (hovered) {
        ekg::property_t &focused_property {
          ekg::query<ekg::property_t>(focused_at)
        };

        if (first_absolute) {
          first_absolute = (
            focused_property != ekg::property_t::not_found
              && property.abs_parent_at == focused_property.abs_parent_at
          );
        }

        if (!first_absolute) {
          focused_property != ekg::property_t::not_found
            && (focused_property.states.is_hovering = false);          
          focused_at = at;
        }
      }

      if (property.states.is_absolute && !first_absolute) {
        focused_at = at;
        first_absolute = true;
      }

      ekg::ui::event(property, descriptor, ekg::io::stage::post);
      if (!hovered && !property.states.is_absolute) {
        ekg::ui::event(property, descriptor, ekg::io::stage::process);
      }
    );
  }

  ekg::gui.ui.hovered_type = ekg::type::unknown;
  ekg::gui.ui.hovered_at = ekg::at_t::not_found;

  ekg::property_t &focused_property {
    ekg::query<ekg::property_t>(focused_at)
  };

  if (focused_property != ekg::property_t::not_found) {
    ekg_core_widget_call(
      focused_at.flags,
      focused_at,

      ekg::gui.ui.hovered_type = static_cast<ekg::type>(focused_at.flags);
      ekg::gui.ui.hovered_at = focused_at;

      if (focused_property.states.is_absolute) {
        ekg::gui.ui.abs_widget_at = focused_at;
      }

      ekg::ui::event(focused_property, descriptor, ekg::io::stage::pre);
      ekg::ui::event(focused_property, descriptor, ekg::io::stage::process);
      ekg::ui::event(focused_property, descriptor, ekg::io::stage::post);
    );
  }

  // @TODO: ref this later

  if (input.was_pressed) {
    ekg::gui.ui.pressed_at = ekg::gui.ui.hovered_at;
    (
      // if p_widget_focused != nullptr ? focused_at.flags : ekg::type::unknown
      (focused_property != ekg::property_t::not_found && (ekg::gui.ui.pressed_type = static_cast<ekg::type>(focused_at.flags)))
      ||
      (ekg::gui.ui.pressed_type = ekg::type::unknown)
    );
  } else if (input.was_released) {
    ekg::gui.ui.released_at = ekg::gui.ui.hovered_at;
    (
      (focused_property != ekg::property_t::not_found && (ekg::gui.ui.released_type = static_cast<ekg::type>(focused_at.flags)))
      ||
      (ekg::gui.ui.released_type = ekg::type::unknown)
    );
  }

  if (
      ekg::gui.ui.last_hovered_at != ekg::gui.ui.hovered_at
      &&
      ekg::gui.ui.hovered_at != ekg::at_t::not_found
      &&
      (
        input.was_pressed || input.was_released
      )
  ) {
    ekg::gui.bind.swap_at = ekg::gui.ui.hovered_at;
    ekg::gui.ui.last_hovered_at = ekg::gui.ui.hovered_at;
    ekg::gui.ui.redraw = true;
    ekg::io::dispatch(ekg::io::operation::swap);
  }
}
