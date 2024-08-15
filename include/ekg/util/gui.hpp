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

#ifndef EKG_UTIL_EVENT_H
#define EKG_UTIL_EVENT_H

#include <map>
#include <stdint.h>

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include "ekg/core/task.hpp"

namespace ekg {
  enum class env {
    refresh,
    swap,
    reload,
    synclayout,
    gc,
    redraw
  };

  /**
   * A widget stack collector, used in many things.
   * You may want to create GUI scenes with auto-destruction.
   * 
   * E.g code:
   * showcase = ekg::stack("gui-stack");
   * // insert widgets
   * showcase.end(); // detatch stack
   * 
   * // on GUI close
   * showcase.destroy();
   * // or destructor
   * ~showcase();
   **/
  struct stack {
  protected:
    bool was_destroyed {};
  public:
    std::string_view tag {};
    std::vector<ekg::ui::abstract_widget*> ordered_list {};
    int32_t target_id {};
    bool target_id_found {};
  public:
    stack() = default;

    /**
     * Create a stack and update on runtime, to disable it
     * set true `do_not_trigger_stack_update` param var.
     **/
    stack(std::string_view tag, bool do_not_trigger_stack_update = false);

    /**
     * Destructor where all widgets stacked are killed.
     **/
    ~stack();

    /**
     * Push this stack to runtime, then collect widgets generated.
     **/
    void push();

    /**
     * Stop to stack widgets.
     **/
    void pop();

    /**
     * Destroy all widgets stacked.
     **/
    void destroy();

    /**
     * Just clear the stack without kill.
     **/
    void clear();
  };
}

namespace ekg {
  void dispatch(task *p_ekg_event);

  void dispatch(ekg::env env);

  void reload(int32_t id);

  void reload(ekg::ui::abstract_widget *p_widget);

  void synclayout(int32_t id);

  void synclayout(ekg::ui::abstract_widget *p_widgets);

  void refresh(int32_t id);

  void refresh(ekg::ui::abstract_widget *p_widget);

  void update_high_frequency(int32_t id);

  void update_high_frequency(ekg::ui::abstract_widget *p_widget);

  void push_back_stack(ekg::ui::abstract_widget *p_widget, ekg::stack &stack);

  ekg::ui::abstract_widget *find_absolute_parent_master(ekg::ui::abstract_widget *p_widget);

  int32_t &set(int32_t &value, int32_t result);

  bool &set(bool &value, bool result);

  std::string &set(std::string &value, std::string_view result);
}

#endif