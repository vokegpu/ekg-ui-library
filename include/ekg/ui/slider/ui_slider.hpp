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
    std::vector<ekg::ui::slider::range_t> range_list {};
  public:
    template<typename t>
    ekg::ui::slider::range_t &range(uint64_t index = 0) {
      if (index >= this->range_list.size()) {
        this->range_list.resize(index + 1);
      }

      return this->range_list.at(index);
    };

    template<typename t>
    ekg::ui::slider::range_t &value(uint64_t index, t val) {
      ekg::ui::slider::range_t &range {this->range_list.at(index)};
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

    template<typename t>
    ekg::ui::slider *range(uint64_t index, t val, t min, t max, int32_t display_precision = -1) {
      ekg::ui::slider::range_t &range {this->range<t>(index)};
      if (display_precision < 0) {
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
     * Get the range list from slider.
     **/
    std::vector<ekg::ui::slider::range_t> &get_range_list();

    ekg::number get_number();

    ekg::ui::slider *set_axis(ekg::axis new_axis);

    ekg::axis get_axis();

    ekg::ui::slider *set_font_size(ekg::font font);

    ekg::font get_font_size();

    ekg::ui::slider *set_place(ekg::flags dock);

    ekg::ui::slider *set_width(float w);

    float get_width();

    ekg::ui::slider *set_scaled_height(int32_t h);

    int32_t get_scaled_height();

    float get_height();

    ekg::ui::slider *set_text_align(ekg::flags dock);

    ekg::flags get_text_align();
  };
}

#endif