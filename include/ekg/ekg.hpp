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
#include "ekg/util/gui.hpp"
#include "ekg/util/io.hpp"
#include "ekg/util/text.hpp"
#include "ekg/gpu/api.hpp"
#include "ekg/os/platform.hpp"

#include "ekg/ui/frame/ui_frame.hpp"
#include "ekg/ui/label/ui_label.hpp"
#include "ekg/ui/button/ui_button.hpp"
#include "ekg/ui/checkbox/ui_checkbox.hpp"
#include "ekg/ui/slider/ui_slider.hpp"
#include "ekg/ui/popup/ui_popup.hpp"
#include "ekg/ui/textbox/ui_textbox.hpp"
#include "ekg/ui/listbox/ui_listbox.hpp"
#include "ekg/ui/scrollbar/ui_scrollbar.hpp"

#define EKG_VERSION_ID "1.1.0"
#define EKG_VERSION_STATE "ALPHA"

namespace ekg {
  /**
   * The runtime core of ekg.
   */
  extern ekg::runtime *core;

  /** The setup and handling functions of ekg **/

  struct runtime_property {
  public:
    const char *p_font_path {};
    ekg::gpu::api *p_gpu_api {};
    ekg::os::platform *p_os_platform {};
  };

  /**
   * The initialization of EKG,
   * initialize `ekg::runtime_property` correct before calling this.
   * 
   * EKG does not has a font-service to handle system fonts, so it is required
   * to load a local (ttf, otf) font file.
   * 
   * GPU API tells which GPU API the application is built in; For OpenGL API
   * do not worry about the initialization setup process; Vulkan API requires
   * more of work to setup, EKG only initialize the necessary descriptor sets,
   * and PSO - you pre-initialize everything -.
   * Check: <link-about-RHI-concept-documentation-not-done>
   * 
   * The OS platform, under the system libraries (X11, Wayland, Win32) or
   * window library (SDL, GLFW, etc).
   * Check: <OS-platform-documention-aobut-not-done>  
   */
  void init(ekg::runtime *p_ekg_runtime, ekg::runtime_property *p_ekg_runtime_property);

  /**
   * Quit from all services and main runtime core.
   */
  void quit();

  /**
   * Process events, tasks, services & widgets.
   */
  void update();

  /**
   * Draw all data from gpu allocator.
   */
  void render();

  /** The environment getters of ekg */

  /**
   * Get the theme service, you can load custom theme files.
   */
  ekg::service::theme &theme();

  /**
   * Get font renderer based on scaled sizes.
   */
  ekg::draw::font_renderer &f_renderer(ekg::font font);

  /**
   * Create frame UI.
   */
  ekg::ui::frame *frame(std::string_view tag, const ekg::vec2 &pos, const ekg::vec2 &size);

  /**
   * Create embedded frame UI.
   */
  ekg::ui::frame *frame(std::string_view tag, const ekg::vec2 &size, uint16_t dock = ekg::dock::none);

  /**
   * Create button UI.
   */
  ekg::ui::button *button(std::string_view text, uint16_t dock = ekg::dock::none);

  /**
   * Create label UI.
   */
  ekg::ui::label *label(std::string_view text, uint16_t dock = ekg::dock::none);

  /**
   * Create immutable label UI.
   */
  ekg::ui::label *imut_label(std::string_view text, uint16_t dock = ekg::dock::none);

  /**
   * Create checkbox UI.
   */
  ekg::ui::checkbox *checkbox(std::string_view text, bool checked, uint16_t dock = ekg::dock::none);

  /**
   * Create slider UI.
   */
  ekg::ui::slider *slider(std::string_view tag, float val, float min, float max, uint16_t dock = ekg::dock::none);

  /**
   * Create popup UI.
   */
  ekg::ui::popup *
  popup(std::string_view tag, const std::vector<std::string> &component_list, bool interact_position = true);

  /**
   * Create textbox UI.
   */
  ekg::ui::textbox *textbox(std::string_view tag, std::string_view text, uint16_t dock = ekg::dock::none);

  /**
   * Create listbox container UI.
   */
  ekg::ui::listbox *listbox_container(std::string_view tag, std::vector<ekg::ui::abstract*> element_list, uint16_t dock = ekg::dock::none);

  /*
   * Create listbox container UI.
   */
  ekg::ui::listbox *listbox(std::string_view tag, std::vector<ekg::item> item_list, uint16_t dock = ekg::dock::none);

  /**
   * Create scrollbar UI.
   */
  ekg::ui::scrollbar *scrollbar(std::string_view tag);

  /**
   * Stop collecting UIs to group.
   */
  void pop_group();
}

#endif