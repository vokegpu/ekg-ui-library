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

#ifndef EKG_SERVICE_THEME_H
#define EKG_SERVICE_THEME_H

#include "ekg/util/geometry.hpp"
#include <vector>

namespace ekg::service {
  class theme {
  protected:
    std::vector<std::string> loaded_theme_list {};
    std::string current_theme {};
  public:
    ekg::vec4 frame_background {};
    ekg::vec4 frame_border {};
    ekg::vec4 frame_outline {};
    int32_t frame_activy_offset {};

    ekg::vec4 button_background {};
    ekg::vec4 button_string {};
    ekg::vec4 button_outline {};
    ekg::vec4 button_activy {};
    ekg::vec4 button_highlight {};

    ekg::vec4 checkbox_background {};
    ekg::vec4 checkbox_string {};
    ekg::vec4 checkbox_outline {};
    ekg::vec4 checkbox_activy {};
    ekg::vec4 checkbox_highlight {};

    ekg::vec4 slider_string {};
    ekg::vec4 slider_background {};
    ekg::vec4 slider_outline {};
    ekg::vec4 slider_activy {};
    ekg::vec4 slider_activy_bar {};
    ekg::vec4 slider_highlight {};
    int32_t slider_bar_thickness {};
    int32_t slider_target_thickness {};

    ekg::vec4 label_string {};

    ekg::vec4 popup_string {};
    ekg::vec4 popup_background {};
    ekg::vec4 popup_outline {};
    ekg::vec4 popup_highlight {};
    ekg::vec4 popup_separator {};
    int64_t popup_drop_animation_delay {};

    ekg::vec4 textbox_string {};
    ekg::vec4 textbox_background {};
    ekg::vec4 textbox_outline {};
    ekg::vec4 textbox_cursor {};
    ekg::vec4 textbox_select {};

    ekg::vec4 scrollbar_background {};
    ekg::vec4 scrollbar_highlight {};
    ekg::vec4 scrollbar_activy {};
    ekg::vec4 scrollbar_outline {};
    int32_t scrollbar_pixel_thickness {};

    ekg::vec4 listbox_item_outline {};
    ekg::vec4 listbox_item_background {};
    ekg::vec4 listbox_item_string {};
    ekg::vec4 listbox_category_outline {};
    ekg::vec4 listbox_category_background {};
    ekg::vec4 listbox_category_string {};
    ekg::vec4 listbox_separator {};
    ekg::vec4 listbox_outline {};
    ekg::vec4 listbox_highlight {};

    float min_widget_size {5};
    bool symmetric_layout {};
  public:
    void init();

    void quit();

    std::string get_current_theme_name();

    void refresh_theme_list();

    void load_theme(const std::string &theme);

    void gen_default_light_theme();

    void gen_default_dark_theme();
  };
}

#endif