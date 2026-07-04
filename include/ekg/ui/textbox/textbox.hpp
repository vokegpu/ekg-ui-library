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
#ifndef EKG_UI_TEXTBOX_HPP
#define EKG_UI_TEXTBOX_HPP

#include "ekg/ui/scrollbar/scrollbar.hpp"
#include "ekg/io/descriptor.hpp"
#include "ekg/io/utf.hpp"
#include "ekg/io/font.hpp"
#include "ekg/ui/property.hpp"
#include <regex>

namespace ekg {
  struct textbox_color_scheme_t {
  public:
    ekg::rgba_t<float> background {};
    ekg::rgba_t<float> outline {};
    ekg::rgba_t<float> text_foreground {};
    ekg::rgba_t<float> text_select_foreground {};
    ekg::rgba_t<float> text_cursor_foreground {};
    ekg::rgba_t<float> text_select_outline {};
    ekg::pixel_thickness_t cursor_thickness {2};
    bool caret_cursor {};
    ekg::pixel_thickness_t gutter_margin {2};
  };

  struct textbox_t {
  public:
    enum operation : size_t {
      modifier_left,
      modifier_right
    };
    static constexpr size_t operation_enum_size {ekg::textbox_t::operation::modifier_right+1};

    struct select_draw_layer_t {
    public:
      bool is_ab_equals {};
      bool is_always_static {};
      ekg::rect_t<float> rect {};
    };

    struct cursor_t {
    public:
      ekg::dock direction {};
      size_t highest_char_index {};
      ekg::vec2_t<size_t> a {};
      ekg::vec2_t<size_t> b {};
      ekg::vec2_t<size_t> delta {};
      ekg::rect_t<float> rect {};
      bool is_ignored {};
      bool is_deleted {};
    public:
      bool operator == (const ekg::vec2_t<size_t> &index) {
        return index.x == this->a.x && index.y == this->a.y && index.x == this->b.x && index.y == this->b.y;
      }

      bool operator != (const ekg::vec2_t<size_t> &index) {
        return !(*this == index);
      }

      bool operator > (const ekg::vec2_t<size_t> &index) {
        return (index.x > this->a.x && index.y == this->a.y) || (index.y > this->a.y);
      }

      bool operator >= (const ekg::vec2_t<size_t> &index) {
        return (index.x >= this->a.x && index.y == this->a.y) || (index.y > this->a.y);
      }

      bool operator < (const ekg::vec2_t<size_t> &index) {
        return (index.x < this->b.x && index.y == this->b.y) || (index.y < this->b.y);
      }

      bool operator <= (const ekg::vec2_t<size_t> &index) {
        return (index.x <= this->b.x && index.y == this->b.y) || (index.y < this->b.y);
      }

      bool operator == (const ekg::textbox_t::cursor_t &cursor) {
        return (this->a.x == cursor.a.x && this->a.y == cursor.a.y) && (this->b.x == cursor.b.x && this->b.y == cursor.b.y);
      }

      bool operator != (const ekg::textbox_t::cursor_t &cursor) {
        return !(*this == cursor);
      }
    };

    struct widget_t {
    public:
      ekg::rect_t<float> rect_text_size {};
      ekg::property_t scrollbar_property {};
      ekg::scrollbar_t scrollbar {};
      std::vector<ekg::textbox_t::cursor_t> cursors {};
      std::vector<ekg::textbox_t::select_draw_layer_t> layers_select {};

      size_t last_layers_select_size {};
      size_t view_line_index {UINT64_MAX};
      std::list<ekg::io::chunk_t>::iterator view_chunk_it {};
      size_t view_chunk_line_index {UINT64_MAX};

      ekg::vec2_t<size_t> picked_left {UINT64_MAX, UINT64_MAX};
      ekg::vec2_t<size_t> picked_right {UINT64_MAX, UINT64_MAX};
      size_t current_cursor_index {UINT64_MAX};

      ekg::timing_t cursor_timing {};
      bool set_cursor_static {};
      bool unset_cursor_static {};
    };

    enum class text_input_mode : uint8_t {
      none,
      text,
      ending,
      backspace,
      del
    };

    static constexpr ekg::type type {ekg::type::textbox};
    static ekg::textbox_t not_found;
  public:
    ekg::at_t property_at {};
  public:
    std::string tag {};
    ekg::text text {};
    ekg::font font_size {ekg::font::medium};
    ekg::flags_t dock {};
    ekg::rect_t<float> rect {.w = 100.0f};
    ekg::textbox_color_scheme_t color_scheme {};
    ekg::textbox_t::widget_t widget {};
    ekg::at_array_t<ekg::layer, ekg::enum_layer_size> layers {};
    
    std::array<std::regex, ekg::textbox_t::operation_enum_size> regex_operations {
      std::regex("(^)|([^\\s:&\\(\\);]+)|(:+)|(&+)|(\\(\\))|(\\()|(\\))|(;+)|([.]+)|(,+)"),
      std::regex("[^\\s:&\\(\\);)]([\\s:])|(:+)|(&+)|(\\(\\))|(\\()|(\\))|(;+)|(\\.+)|(,+)|($)")
    };
  public:
    ekg_descriptor(ekg::textbox_t);
  };
}

#endif
