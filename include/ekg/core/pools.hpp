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
#ifndef EKG_CORE_POOLS_HPP
#define EKG_CORE_POOLS_HPP

#include "ekg/io/memory.hpp"
#include "ekg/handler/callback.hpp"
#include "ekg/gpu/sampler.hpp"
#include "ekg/ui/property.hpp"
#include "ekg/ui/stack.hpp"
#include "context.hpp"
#include "ekg/handler/theme.hpp"

#include "ekg/ui/button/button.hpp"
#include "ekg/ui/button/widget.hpp"

#include "ekg/ui/frame/frame.hpp"
#include "ekg/ui/frame/widget.hpp"

#include "ekg/ui/label/label.hpp"
#include "ekg/ui/label/widget.hpp"

#include "ekg/ui/scrollbar/scrollbar.hpp"
#include "ekg/ui/scrollbar/widget.hpp"

#include "ekg/ui/slider/slider.hpp"
#include "ekg/ui/slider/widget.hpp"

#include "ekg/ui/popup/popup.hpp"
#include "ekg/ui/popup/widget.hpp"

#include "ekg/ui/textbox/textbox.hpp"
#include "ekg/ui/textbox/widget.hpp"

namespace ekg::core {
  void registry(ekg::property_t &property);
}

#define ekg_core_widget_call_impl(descriptor_t, widget_descriptor_at, todo) \
    case descriptor_t::type: { \
      descriptor_t &descriptor { \
        ekg::query<descriptor_t>(widget_descriptor_at) \
      }; \
      if (descriptor == descriptor_t::not_found) { \
        break; \
      } \
      ekg::core::widget_call_result = true; \
      todo \
      break; \
    }

namespace ekg::core {
  extern bool widget_call_result;
}

#define ekg_core_widget_call(widget_descriptor_type, widget_descriptor_at, todo) \
  ekg::core::widget_call_result = false; \
  switch (widget_descriptor_type) { \
    ekg_core_widget_call_impl(ekg::frame_t, widget_descriptor_at, todo); \
    ekg_core_widget_call_impl(ekg::button_t, widget_descriptor_at, todo); \
    ekg_core_widget_call_impl(ekg::label_t, widget_descriptor_at, todo); \
    ekg_core_widget_call_impl(ekg::scrollbar_t, widget_descriptor_at, todo); \
    ekg_core_widget_call_impl(ekg::slider_t, widget_descriptor_at, todo); \
    ekg_core_widget_call_impl(ekg::popup_t, widget_descriptor_at, todo); \
    ekg_core_widget_call_impl(ekg::textbox_t, widget_descriptor_at, todo); \
  }

#define ekg_core_unique_widget_call(descriptor_t, widget_descriptor_type, widget_descriptor_at, todo) \
  switch (widget_descriptor_type) { \
    ekg_core_widget_call_impl(descriptor_t, widget_descriptor_at, todo); \
  }

#define ekg_registry_widget_impl(widget_descriptor_t, register_widget_pool, register_property_pool, is_container, register_settings) \
  widget_descriptor_t &widget { \
    register_widget_pool.push_back( \
      ekg::io::any_static_cast<widget_descriptor_t>(&descriptor) \
    ) \
  }; \
  ekg::property_t &property { \
    register_property_pool.push_back({}) \
  }; \
\
  widget.at.flags = widget_descriptor_t::type; \
  property.descriptor_at = widget.at; \
\
  property.at.flags = widget_descriptor_t::type; \
  widget.property_at = property.at; \
\
  ekg::theme_t &global_theme {ekg::theme()};\
\
  register_settings; \
\
  ekg::stack_t &stack {ekg::query<ekg::stack_t>(ekg::gui.bind.stack_at)}; \
  if (stack != ekg::stack_t::not_found) { \
    stack.widgets.push_back(widget.at); \
  } \
\
  ekg::property_t &parent {ekg::query<ekg::property_t>(ekg::gui.bind.parent_at)}; \
  if (is_container) { \
    if (parent != ekg::property_t::not_found && widget.dock != ekg::dock::none) { \
      property.parent_at = parent.at; \
      parent.children.push_back(widget.at); \
      property.abs_parent_at = parent.abs_parent_at; \
    } else if (parent == ekg::property_t::not_found) { \
      property.abs_parent_at = property.at; \
    } \
    ekg::gui.bind.parent_at = property.at; \
  } else if (parent != ekg::property_t::not_found) { \
    property.parent_at = ekg::gui.bind.parent_at; \
    property.abs_parent_at = parent.abs_parent_at; \
    parent.children.push_back(widget.at); \
  } \
\
  ekg::core::registry(property); \
  return ekg::io::any_static_cast<t>(&widget);

namespace ekg {
  extern struct pools_t {
  public:
    ekg::pool<ekg::stack_t> stack {};
    ekg::pool<ekg::callback_t> callback {};
    ekg::pool<ekg::sampler_t> sampler {};

    ekg::pool<ekg::property_t> button_property {};
    ekg::pool<ekg::button_t> button {};

    ekg::pool<ekg::property_t> frame_property {};
    ekg::pool<ekg::frame_t> frame {};

    ekg::pool<ekg::property_t> label_property {};
    ekg::pool<ekg::label_t> label {};

    ekg::pool<ekg::property_t> scrollbar_property {};
    ekg::pool<ekg::scrollbar_t> scrollbar {};

    ekg::pool<ekg::property_t> slider_property {};
    ekg::pool<ekg::slider_t> slider {};

    ekg::pool<ekg::property_t> popup_property {};
    ekg::pool<ekg::popup_t> popup {};

    ekg::pool<ekg::property_t> textbox_property {};
    ekg::pool<ekg::textbox_t> textbox {};
  } pools;

  template<typename t>
  t &query(
    ekg::at_t &at
  ) {
    switch (t::type) {
    case ekg::type::stack:
      return ekg::io::any_static_cast<t>(
        &ekg::pools.stack.query(at)
      );
    case ekg::type::callback:
      return ekg::io::any_static_cast<t>(
        &ekg::pools.callback.query(at)
      );
    case ekg::type::sampler:
      return ekg::io::any_static_cast<t>(
        &ekg::pools.sampler.query(at)
      );
    case ekg::type::property:
      switch (at.flags) {
      case ekg::type::button:
        return ekg::io::any_static_cast<t>(
          &ekg::pools.button_property.query(at)
        );
      case ekg::type::frame:
        return ekg::io::any_static_cast<t>(
          &ekg::pools.frame_property.query(at)
        );
      case ekg::type::label:
        return ekg::io::any_static_cast<t>(
          &ekg::pools.label_property.query(at)
        );
      case ekg::type::scrollbar:
        return ekg::io::any_static_cast<t>(
          &ekg::pools.scrollbar_property.query(at)
        );
      case ekg::type::slider:
        return ekg::io::any_static_cast<t>(
          &ekg::pools.slider_property.query(at)
        );
      case ekg::type::popup:
        return ekg::io::any_static_cast<t>(
          &ekg::pools.popup_property.query(at)
        );
      case ekg::type::textbox:
        return ekg::io::any_static_cast<t>(
          &ekg::pools.textbox_property.query(at)
        );
      }
    case ekg::type::button:
      return ekg::io::any_static_cast<t>(
        &ekg::pools.button.query(at)
      );
    case ekg::type::frame:
      return ekg::io::any_static_cast<t>(
        &ekg::pools.frame.query(at)
      );
    case ekg::type::label:
      return ekg::io::any_static_cast<t>(
        &ekg::pools.label.query(at)
      );
    case ekg::type::scrollbar:
      return ekg::io::any_static_cast<t>(
        &ekg::pools.scrollbar.query(at)
      );
    case ekg::type::slider:
      return ekg::io::any_static_cast<t>(
        &ekg::pools.slider.query(at)
      );
    case ekg::type::popup:
      return ekg::io::any_static_cast<t>(
        &ekg::pools.popup.query(at)
      );
    case ekg::type::textbox:
      return ekg::io::any_static_cast<t>(
        &ekg::pools.textbox.query(at)
      );
    }

    return t::not_found;
  }

  template<typename t>
  t &make(
    t descriptor = {}
  ) {
    switch (t::type) {
      case ekg::type::frame: {
        ekg_registry_widget_impl(
          ekg::frame_t,
          ekg::pools.frame,
          ekg::pools.frame_property,
          true,
          {
            property.is_childnizate = true;
            property.is_children_docknizable = true;
            widget.color_scheme = global_theme.frame_color_scheme;
          }
        );
      }

      case ekg::type::button: {
        ekg_registry_widget_impl(
          ekg::button_t,
          ekg::pools.button,
          ekg::pools.button_property,
          false,
          {
            property.is_childnizate = false;
            property.is_children_docknizable = false;
            widget.color_scheme = global_theme.button_color_scheme;
          }
        );
      }

      case ekg::type::label: {
        ekg_registry_widget_impl(
          ekg::label_t,
          ekg::pools.label,
          ekg::pools.label_property,
          false,
          {
            property.is_childnizate = false;
            property.is_children_docknizable = false;
            widget.color_scheme = global_theme.label_color_scheme;
          }
        );
      }

      case ekg::type::scrollbar: {
        ekg_registry_widget_impl(
          ekg::scrollbar_t,
          ekg::pools.scrollbar,
          ekg::pools.scrollbar_property,
          false,
          {
            property.is_childnizate = false;
            property.is_children_docknizable = false;
            widget.color_scheme = global_theme.scrollbar_color_scheme;
          }
        );
      }

      case ekg::type::slider: {
        ekg_registry_widget_impl(
          ekg::slider_t,
          ekg::pools.slider,
          ekg::pools.slider_property,
          false,
          {
            property.is_childnizate = false;
            property.is_children_docknizable = false;
            widget.color_scheme = global_theme.slider_color_scheme;
          }
        );
      }

      case ekg::type::popup: {
        ekg_registry_widget_impl(
          ekg::popup_t,
          ekg::pools.popup,
          ekg::pools.popup_property,
          true,
          {
            property.is_childnizate = true;
            property.is_stack_top_level = true;
            property.is_children_docknizable = true;
            property.states.is_visible = false;

            widget.color_scheme = global_theme.popup_color_scheme;
          }
        );
      }

      case ekg::type::textbox: {
        ekg_registry_widget_impl(
          ekg::textbox_t,
          ekg::pools.textbox,
          ekg::pools.textbox_property,
          false,
          {
            property.is_childnizate = false;
            property.is_children_docknizable = false;
            widget.color_scheme = global_theme.textbox_color_scheme;
          }
        );
      }

    case ekg::type::stack: {
      ekg::stack_t &stack {
        ekg::pools.stack.push_back(
          ekg::io::any_static_cast<ekg::stack_t>(&descriptor)
        )
      };

      ekg::gui.bind.stack_at = stack.at;

      return ekg::io::any_static_cast<t>(
        &stack
      );
    }
    case ekg::type::callback:
      return ekg::io::any_static_cast<t>( 
        &ekg::pools.callback.push_back(
          ekg::io::any_static_cast<ekg::callback_t>(&descriptor)
        )
      );
    case ekg::type::sampler:
      return ekg::io::any_static_cast<t>(  
        &ekg::pools.sampler.push_back(
          ekg::io::any_static_cast<ekg::sampler_t>(&descriptor)
        )
      );
    }

    return t::not_found;
  }

  template<typename t>
  void pop() {
    switch (t::type) {
    case ekg::type::stack:
      ekg::gui.bind.stack_at = ekg::at_t::not_found;
      break;
    case ekg::type::property:
      ekg::gui.bind.parent_at = ekg::query<ekg::property_t>(ekg::gui.bind.parent_at).parent_at;
      break;
    case ekg::type::frame:
      ekg::gui.bind.parent_at = ekg::query<ekg::property_t>(ekg::gui.bind.parent_at).parent_at;
      break;
    case ekg::type::popup:
      ekg::gui.bind.parent_at = ekg::query<ekg::property_t>(ekg::gui.bind.parent_at).parent_at;
      break;
    }
  }
}

#endif
