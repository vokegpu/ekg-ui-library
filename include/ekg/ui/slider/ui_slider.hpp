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

#ifndef EKG_UI_SLIDER_H
#define EKG_UI_SLIDER_H

#include "ekg/ui/abstract/ui_abstract.hpp"
#include "ekg/ui/display.hpp"
#include "ekg/util/io.hpp"

namespace ekg::ui {
  class slider :
    public ekg::ui::abstract
  {
  public:
    struct range_t {
    public:
      ekg::value_t<double, ekg::ui::slider> f64 {};
      double f64_min {};
      double f64_max {};

      ekg::value_t<float, ekg::ui::slider> f32 {};
      float f32_min {};
      float f32_max {};

      ekg::value_t<uint64_t, ekg::ui::slider> u64 {};
      uint64_t u64_min {};
      uint64_t u64_max {};

      ekg::value_t<int64_t, ekg::ui::slider> i64 {};
      int64_t i64_min {};
      int64_t i64_max {};

      ekg::value_t<uint32_t, ekg::ui::slider> u32 {};
      uint32_t u32_min {};
      uint32_t u32_max {};

      ekg::value_t<int32_t, ekg::ui::slider> i32 {};
      int32_t i32_min {};
      int32_t i32_max {};

      ekg::value_t<uint16_t, ekg::ui::slider> u16 {};
      uint16_t u16_min {};
      uint16_t u16_max {};

      ekg::value_t<int16_t, ekg::ui::slider> i16 {};
      int16_t i16_min {};
      int16_t i16_max {};

      ekg::value_t<uint8_t, ekg::ui::slider> u8 {};
      uint8_t u8_min {};
      uint8_t u8_max {};

      ekg::value_t<int8_t, ekg::ui::slider> i8 {};
      int8_t i8_min {};
      int8_t i8_max {};

      int32_t display_precision {2};
    public:
      // idc meow
    };
  protected:
    ekg::number number {};
    ekg::flags dock_text {};
    ekg::axis axis {};
    ekg::font font_size {};
    std::map<uint64_t, ekg::ui::slider::range_t> range_map {};
    float bar_offset {1.0f};
  public:
    /**
     * Returns a range-based slider with `ekg::dock::value_t<number_type, ekg::ui::slider>` option.
     **/
    template<typename t>
    ekg::ui::slider::range_t &range(uint64_t index = 0) {
      if (!this->range_map.count(index)) {
        this->range_map.insert(
          {index, ekg::ui::slider::range_t {}}
        );
      }

      return this->range_map.at(index);
    };

    /**
     * Set a new value to an existing range.
     * Returns an reference to the range.
     **/
    template<typename t>
    ekg::ui::slider::range_t &value(uint64_t index, t val) {
      ekg::ui::slider::range_t &range {this->range<t>(index)};
      switch (this->number) {
      case ekg::number::float64:
        range.f64.set_value(static_cast<t>(val));
        range.f64.registry(this);
        range.f64.align_ownership_mem_if_necessary();
        break;
      case ekg::number::float32:
        range.f32.set_value(static_cast<t>(val));
        range.f32.registry(this);
        range.f32.align_ownership_mem_if_necessary();
        break;
      case ekg::number::int64:
        range.i64.set_value(static_cast<t>(val));
        range.i64.registry(this);
        range.i64.align_ownership_mem_if_necessary();
        break;
      case ekg::number::uint64:
        range.u64.set_value(static_cast<t>(val));
        range.u64.registry(this);
        range.u64.align_ownership_mem_if_necessary();
        break;
      case ekg::number::int32:
        range.i32.set_value(static_cast<t>(val));
        range.i32.registry(this);
        range.i32.align_ownership_mem_if_necessary();
        break;
      case ekg::number::uint32:
        range.u32.set_value(static_cast<t>(val));
        range.u32.registry(this);
        range.u32.align_ownership_mem_if_necessary();
        break;
      case ekg::number::int16:
        range.i16.set_value(static_cast<t>(val));
        range.i16.registry(this);
        range.i16.align_ownership_mem_if_necessary();
        break;
      case ekg::number::uint16:
        range.u16.set_value(static_cast<t>(val));
        range.u16.registry(this);
        range.u16.align_ownership_mem_if_necessary();
        break;
      case ekg::number::int8:
        range.i8.set_value(static_cast<t>(val));
        range.i8.registry(this);
        range.i8.align_ownership_mem_if_necessary();
        break;
      case ekg::number::uint8:
        range.u8.set_value(static_cast<t>(val));
        range.u8.registry(this);
        range.u8.align_ownership_mem_if_necessary();
        break;
      }

      return range;
    }

    /**
     * Create a new range but instead of returning the range,
     * returns the UI.
     **/
    template<typename t>
    ekg::ui::slider *range(uint64_t index, t val, t min, t max, int32_t display_precision = -1) {
      ekg::ui::slider::range_t &range {this->range<t>(index)};
      if (display_precision != -1) {
        range.display_precision = display_precision;
      }

      switch (this->number) {
      case ekg::number::float64:
        range.f64.set_value(static_cast<t>(val));
        range.f64_min = min;
        range.f64_max = max;
        range.f64.registry(this);
        range.f64.align_ownership_mem_if_necessary();
        break;
      case ekg::number::float32:
        range.f32.set_value(static_cast<t>(val));
        range.f32_min = min;
        range.f32_max = max;
        range.f32.registry(this);
        range.f32.align_ownership_mem_if_necessary();
        break;
      case ekg::number::int64:
        range.i64.set_value(static_cast<t>(val));
        range.i64_min = min;
        range.i64_max = max;
        range.i64.registry(this);
        range.i64.align_ownership_mem_if_necessary();
        break;
      case ekg::number::uint64:
        range.u64.set_value(static_cast<t>(val));
        range.u64_min = min;
        range.u64_max = max;
        range.u64.registry(this);
        range.u64.align_ownership_mem_if_necessary();
        break;
      case ekg::number::int32:
        range.i32.set_value(static_cast<t>(val));
        range.i32_min = min;
        range.i32_max = max;
        range.i32.registry(this);
        range.i32.align_ownership_mem_if_necessary();
        break;
      case ekg::number::uint32:
        range.u32.set_value(static_cast<t>(val));
        range.u32_min = min;
        range.u32_max = max;
        range.u32.registry(this);
        range.u32.align_ownership_mem_if_necessary();
        break;
      case ekg::number::int16:
        range.i16.set_value(static_cast<t>(val));
        range.i16_min = min;
        range.i16_max = max;
        range.i16.registry(this);
        range.i16.align_ownership_mem_if_necessary();
        break;
      case ekg::number::uint16:
        range.u16.set_value(static_cast<t>(val));
        range.u16_min = min;
        range.u16_max = max;
        range.u16.registry(this);
        range.u16.align_ownership_mem_if_necessary();
        break;
      case ekg::number::int8:
        range.i8.set_value(static_cast<t>(val));
        range.i8_min = min;
        range.i8_max = max;
        range.i8.registry(this);
        range.i8.align_ownership_mem_if_necessary();
        break;
      case ekg::number::uint8:
        range.u8.set_value(static_cast<t>(val));
        range.u8_min = min;
        range.u8_max = max;
        range.u8.registry(this);
        range.u8.align_ownership_mem_if_necessary();
        break;
      }

      return this;
    }
  public:
    /**
     * Set the numbebr, used to determine diff number serializer formats.
     * Unsafe due the loss-ptr address from serializer.
     **/
    ekg::ui::slider *unsafe_set_number(ekg::number number);
  public:
    /**
     * Returns the size of range list (multi-ranger slider).
     **/
    uint64_t get_range_count();

    /**
     * Get the number type used in slider value.
     **/
    ekg::number get_number();

    /**
     * Set the current slider axis. Horizontal or vertical.
     **/
    ekg::ui::slider *set_axis(ekg::axis new_axis);

    /**
     * Get the current axis set.
     **/
    ekg::axis get_axis();

    /**
     * Set the base font size.
     * Note: Only `ekg::dock::left` or `ekg::dock::right` applies.
     **/
    ekg::ui::slider *set_font_size(ekg::font font);

    /**
     * Get the font size base.
     **/
    ekg::font get_font_size();

    /**
     * Set the dock place.
     **/
    ekg::ui::slider *set_place(ekg::flags dock);

    /**
     * Set a new width.
     **/
    ekg::ui::slider *set_width(float w);

    /**
     * Get the width.
     **/
    float get_width();

    /**
     * Set the scaled height.
     **/
    ekg::ui::slider *set_scaled_height(int32_t h);

    /**
     * Get the scaled height.
     **/
    int32_t get_scaled_height();

    /**
     * Get the height.
     **/
    float get_height();

    /**
     * Set the text dock align.
     **/
    ekg::ui::slider *set_text_align(ekg::flags dock);

    /**
     * Get the text dock align flags.
     **/
    ekg::flags get_text_align();

    /**
     * Set the offset between bars and text.
     **/
    ekg::ui::slider *set_bar_offset(float offset);

    /**
     * Get the offset between bars and text.
     **/
    float get_bar_offset();
  };
}

#endif