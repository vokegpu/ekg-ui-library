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
#include "ekg/ekg.hpp"
#include "ekg/io/log.hpp"
#include "ekg/core/context.hpp"
#include "ekg/core/pools.hpp"

#include "ekg/ui/abstract.hpp"
#include "ekg/ui/button/widget.hpp"

ekg::runtime_t *ekg::p_core {nullptr};
ekg::metrics_t ekg::metrics {};
ekg::dpi_t ekg::dpi {};
ekg::gui_t ekg::gui {};
ekg::pools_t ekg::pools {};

std::ostringstream ekg::log::buffer {};
bool ekg::log::buffered {};

ekg::flags_t ekg::init(
  ekg::runtime_properties_info_t &runtime_properties_info,
  ekg::runtime_t &runtime
) {
  ekg::log() << "Initializing EKG version " << EKG_VERSION;

  ekg::p_core = &runtime;

  ekg::p_core->p_platform_base = runtime_properties_info.p_platform_base;
  ekg::p_core->p_gpu_api = runtime_properties_info.p_gpu_api;
  ekg::p_core->ft_library = runtime_properties_info.ft_library;

  ekg::p_core->p_platform_base->init();
  ekg::p_core->p_gpu_api->init();

  ekg::p_core->handler_callback.init();
  ekg::p_core->handler_input.init();
  ekg::p_core->handler_theme.init();
  ekg::p_core->draw_allocator.init();

  /* deprecated soon */

  ekg::p_core->draw_font_small.init();
  ekg::p_core->draw_font_small.set_font(runtime_properties_info.default_font_path_text);
  ekg::p_core->draw_font_small.set_font_emoji(runtime_properties_info.default_font_path_emoji);

  ekg::p_core->draw_font_medium.init();
  ekg::p_core->draw_font_medium.set_font(runtime_properties_info.default_font_path_text);
  ekg::p_core->draw_font_medium.set_font_emoji(runtime_properties_info.default_font_path_emoji);

  ekg::p_core->draw_font_big.init();
  ekg::p_core->draw_font_big.set_font(runtime_properties_info.default_font_path_text);
  ekg::p_core->draw_font_big.set_font_emoji(runtime_properties_info.default_font_path_emoji);

  ekg::info_t info {};
  ekg::core::scalenize(info);

  ekg::log() << "Successfully initialized";
  return ekg::result::success;
}

void ekg::quit() {
  if (ekg::p_core == nullptr) {
    return;
  }
}

void ekg::update() {
  if (ekg::p_core == nullptr) {
    return;
  }

  size_t size {ekg::p_core->high_frequency.size()};
  for (size_t it {}; it < size; it++) {
    ekg::property_t &property {
      ekg::query<ekg::property_t>(ekg::p_core->high_frequency.at(it))
    };

    ekg_core_widget_call(
      property.descriptor_at.flags,
      property.descriptor_at,
      ekg::ui::high_frequency(property, descriptor);
    );

    if (
      // no not-found-check because `t::not_found` bool fields are always false
      !property.widget.is_high_frequency
    ) {
      property.operation.should_enable_high_frequency = false; // same here

      ekg::p_core->high_frequency.erase(
        ekg::p_core->high_frequency.begin() + it
      );

      size = ekg::p_core->high_frequency.size();
    }
  }

  ekg::p_core->handler_input.update();
  ekg::p_core->handler_callback.update();
  ekg::p_core->p_platform_base->update();
  ekg::p_core->p_platform_base->event.type = ekg::io::event_type::none;
}

void ekg::render() {
  if (ekg::p_core == nullptr) {
    return;
  }

  if (ekg::gui.ui.redraw) {
    ekg::gui.ui.redraw = false;
    
    ekg::p_core->draw_allocator.invoke();

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

        /**
         * Sync the widget states and position by getting the abs.
         **/
        ekg::ui::get_abs_rect(property, descriptor.rect);
        if (!property.states.is_visible) {
          break;
        }

        /**
         * @TODO: Fix this stupid glitch where scrolling is jittering because of something
         * 
         * I do not know why this is hapenning, likely wtf, if I remove this
         * scrolling is horrible, I am not sure why this is happening, I tried make the make redraw always,
         * rendering everything, no allocators asserts. Nothing. I tried do a stupid `meow<t>` where t receives a descriptor,
         * but nothing too. I do not know why this happens but it is very insanely weird. Post does not affect the performance
         * a lot, may we consider to let for some long time. While no solution was found.
         * 
         * - @mrsrina / Rina Wilk
         * 12:45am 6/17/2025
         * 
         **/
        ekg::ui::event(property, descriptor, ekg::io::stage::post);
        /* */

        ekg::ui::pass(property, descriptor);

        if (!property.widget.should_buffering) {
          break;
        }

        ekg::ui::buffering(property, descriptor);
        //property.widget.should_buffering = false;
      );
    }

    ekg::p_core->draw_allocator.revoke();
  }

  ekg::p_core->draw_allocator.to_gpu();
}
