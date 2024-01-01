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

#ifndef EKG_UI_ABSTRACT_H
#define EKG_UI_ABSTRACT_H

#include "ekg/util/geometry.hpp"
#include "ekg/util/aspect.hpp"
#include <vector>

namespace ekg {
  enum class type {
    abstract,
    frame,
    button,
    label,
    slider,
    slider2d,
    checkbox,
    textbox,
    combobox,
    listbox,
    tab,
    popup,
    scroll
  };

  enum class level {
    bottom_level, top_level
  };

  enum class ui_sync {
    reset = 1,
    dimension = 2
  };

  namespace ui {
    struct item {
    public:
      std::string name {};
      bool value {};
      uint32_t id {};
      uint32_t linked_id {};
      uint16_t attributes {};
    public:
      ekg::rect rect_dimension {};
      ekg::rect rect_content {};
      ekg::rect rect_box {};
    };

    class abstract {
    protected:
      int32_t id {};
      int32_t parent_id {};
      std::vector<int32_t> child_id_list {};

      bool alive {true};
      uint16_t dock_flags {};
      uint16_t sync_flags {};
      std::string tag {};

      ekg::state state {};
      ekg::type type {ekg::type::abstract};
      ekg::level level {};
      ekg::rect rect_widget {};
      ekg::rect sync_ui {};

      int32_t scaled_height {};
    public:
      ekg::ui::abstract *unsafe_set_scaled_height_layout(int32_t scaled_size);

    public:
      abstract();

      ~abstract();

      ekg::ui::abstract *set_tag(std::string_view tag);

      std::string_view get_tag();

      ekg::ui::abstract *add_child(int32_t id);

      std::vector<int32_t> &get_child_id_list();

      ekg::ui::abstract *remove_child(int32_t id);

      ekg::ui::abstract *set_id(int32_t id);

      int32_t get_id();

      ekg::ui::abstract *set_parent_id(int32_t parent_id);

      int32_t get_parent_id();

      ekg::ui::abstract *set_alive(bool state);

      bool is_alive();

      void destroy();

      ekg::ui::abstract *set_state(const ekg::state &_state);

      ekg::state get_state();

      ekg::ui::abstract *set_level(const ekg::level &_level);

      ekg::level get_level();

      ekg::ui::abstract *set_type(const ekg::type &_type);

      ekg::type get_type();

      uint16_t get_place_dock();

      uint16_t &get_sync();

      void reset();

      ekg::rect &widget();

      ekg::rect &ui();

      bool has_parent();

      bool has_children();
    };
  }
}

#endif