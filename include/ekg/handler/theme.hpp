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
#ifndef EKG_HANDLER_THEME_HPP
#define EKG_HANDLER_THEME_HPP

#include "ekg/ui/button/button.hpp"
#include "ekg/ui/frame/frame.hpp"
#include "ekg/ui/label/label.hpp"
#include "ekg/ui/scrollbar/scrollbar.hpp"
#include "ekg/ui/slider/slider.hpp"
#include "ekg/ui/popup/popup.hpp"
#include "ekg/ui/textbox/textbox.hpp"

namespace ekg {
  struct theme_t {
  public:
    std::string tag {};
    std::string author {};
    std::string description {};
  public:
    float layout_offset {};
    ekg::pixel_t layout_margin_thickness {2};
    ekg::button_color_scheme_t button_color_scheme {};
    ekg::frame_color_scheme_t frame_color_scheme {};
    ekg::label_color_scheme_t label_color_scheme {};
    ekg::scrollbar_color_scheme_t scrollbar_color_scheme {};
    ekg::slider_color_scheme_t slider_color_scheme {};
    ekg::popup_color_scheme_t popup_color_scheme {};
    ekg::textbox_color_scheme_t textbox_color_scheme {};
  };

  ekg::theme_t &theme(std::string tag = "");
  ekg::theme_t &set_current_theme(const std::string &tag);
}

#endif
