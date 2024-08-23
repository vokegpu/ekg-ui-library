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
    public ekg::ui::abstract,
    public ekg::value_t<ekg::feature*, ekg::ui::slider>
  {
  public:
    template<typename t>
    struct range_t {
    public:
      t min {};
      t max {};
      ekg::value_t<t, ekg::ui::slider> value {};
    };

    template<typename t>
    struct serializer_t : public ekg::feature {
    public:
      std::vector<ekg::ui::slider::range_t<t>> range_list {};
    };
  public:
    ekg::number number {};
    ekg::flags dock_text {};
    ekg::axis axis {};
    ekg::font font_size {};
  public:
    template<typename t>
    ekg::ui::slider::serializer_t<t> &serializer() {
      return *static_cast<ekg::ui::slider::serializer_t<t>*>(this->get_value());
    };

    template<typename t>
    ekg::value_t<t, ekg::ui::slider> &value(uint64_t index = 0) {
      ekg::ui::slider::serializer_t<t> &serializer {
        this->serializer<t>()
      };

      ekg::ui::slider::range_t<t> &serialized_range {serializer.range_list.at(index)};
      serialized_range.value.registry(this);

      return serialized_range.value;
    };

    template<typename t>
    ekg::ui::slider::range_t<t> &range(uint64_t index, t min, t max) {
      ekg::ui::slider::serializer_t<t> &serializer {
        this->serializer<t>()
      };

      if (index >= serializer.range_list.size()) {
        serializer.range_list.resize(index + 1);
      }

      ekg::ui::slider::range_t<t> &serialized_range {serializer.range_list.at(index)};
      serialized_range.min = min;
      serialized_range.max = max;
      serialized_range.value.registry(this);

      return serialized_range;
    };
  public:
    /**
     * Set the numbebr, used to determine diff number serializer formats.
     * Unsafe due the loss-ptr address from serializer.
     **/
    ekg::ui::slider *unsafe_set_number(ekg::number number);
  public:
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