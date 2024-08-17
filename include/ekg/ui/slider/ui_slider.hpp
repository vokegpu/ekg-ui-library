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
    struct serializer_t : public ekg::feature {
    public:
      t min {};
      t max {};

      std::vector<ekg::value_t<t, ekg::ui::slider>> value_list {};
    public:
      serializer_t<t>() {
        this->value_list.emplace_back();
      }
    };
  public:
    ekg::number number {};
    ekg::flags dock_text {};
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

      ekg::value_t<t, ekg::ui::slider> &serialized_value {serializer.value_list.at(index)};
      serialized_value.registry(this);

      return serialized_value;
    };
  public:
    /**
     * Set the numbebr, used to determine diff number serializer formats.
     * Unsafe due the loss-ptr address from serializer.
     **/
    ekg::ui::slider *unsafe_set_number(ekg::number number);
  };
}

#endif