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

#ifndef EKG_H
#define EKG_H

#include "ekg/core/runtime.hpp"
#include "ekg/core/task.hpp"
#include "ekg/gpu/gpu_base_impl.hpp"
#include "ekg/util/gui.hpp"
#include "ekg/util/io.hpp"
#include "ekg/util/text.hpp"
#include "ekg/os/ekg_opengl.hpp"

#include "ekg/ui/frame/ui_frame.hpp"
#include "ekg/ui/label/ui_label.hpp"
#include "ekg/ui/button/ui_button.hpp"
#include "ekg/ui/checkbox/ui_checkbox.hpp"
#include "ekg/ui/slider/ui_slider.hpp"
#include "ekg/ui/popup/ui_popup.hpp"
#include "ekg/ui/textbox/ui_textbox.hpp"
#include "ekg/ui/listbox/ui_listbox.hpp"
#include "ekg/ui/scroll/ui_scroll.hpp"

#define ekg_version_id "1.0.5.0"
#define ekg_version_state "ALPHA"

namespace ekg {
  /*
   * The runtime core of ekg.
   */
  extern ekg::runtime *core;

  /*
   * Shaders version should be higher than 300.
   */
  extern std::string glsl_version;

  /*
   * When enabled, ekg automatically find the base resolution scale by monitor.
   */
  extern bool autoscale;

  /*
   * The scale interval between the base scaled resolution and root size.
   */
  extern float scaleinterval;

  /*
   * The resolution scale base, for edit, then you need to disable 'ekg::autoscale'.
   */
  extern ekg::vec2 scalebase;

  /*
   * Pre-decode clipboard content.
   */
  extern bool pre_decode_clipboard;

  /* The setup and handling functions of ekg. */

  struct runtime_property {
  public:
    const char *p_font_path {};
    ekg::gpu_api gpu_api {};
    void *p_sdl_win {};
  };

  /*
   * Init the runtime core of ekg, set window SDL instance and font renderer path (default but changeable).
   */
  void init(ekg::runtime *p_ekg_runtime, ekg::runtime_property *p_ekg_runtime_property);

  /*
   * Quit from all services and main runtime core.
   */
  void quit();

  /*
   * Poll events and sync UI events.
   */
  void poll_event(SDL_Event &);

  /*
   * Process events, tasks, services & widgets.
   */
  void update();

  /*
   * Draw all data from gpu allocator.
   */
  void render();

  /* The environment getters of ekg. */

  /*
   * Get the theme service, you can load custom theme files.
   */
  ekg::service::theme &theme();

  /*
   * Get font renderer based on scaled sizes.
   */
  ekg::draw::font_renderer &f_renderer(ekg::font font);

  /*
   * Create frame UI.
   */
  ekg::ui::frame *frame(std::string_view tag, const ekg::vec2 &pos, const ekg::vec2 &size);

  /*
   * Create embedded frame UI.
   */
  ekg::ui::frame *frame(std::string_view tag, const ekg::vec2 &size, uint16_t dock = ekg::dock::none);

  /*
   * Create button UI.
   */
  ekg::ui::button *button(std::string_view text, uint16_t dock = ekg::dock::none);

  /*
   * Create label UI.
   */
  ekg::ui::label *label(std::string_view text, uint16_t dock = ekg::dock::none);

  /*
   * Create checkbox UI.
   */
  ekg::ui::checkbox *checkbox(std::string_view text, bool checked, uint16_t dock = ekg::dock::none);

  /*
   * Create slider UI.
   */
  ekg::ui::slider *slider(std::string_view tag, float val, float min, float max, uint16_t dock = ekg::dock::none);

  /*
   * Create popup UI.
   */
  ekg::ui::popup *
  popup(std::string_view tag, const std::vector<std::string> &component_list, bool interact_position = true);

  /*
   * Create textbox UI.
   */
  ekg::ui::textbox *textbox(std::string_view tag, std::string_view text, uint16_t dock = ekg::dock::none);

  /*
   * Create listbox UI.
   */
  ekg::ui::listbox *listbox(std::string_view tag, const ekg::item &item, uint16_t dock = ekg::dock::none);

  /*
   * Create scroll UI.
   */
  ekg::ui::scroll *scroll(std::string_view tag);

  /*
   * Stop collecting UIs to group.
   */
  void pop_group();
}

#endif