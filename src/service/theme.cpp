/**
 * MIT License
 * 
 * Copyright (c) 2022-2024 Rina Wilk / vokegpu@gmail.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"); to deal
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

#include "ekg/service/theme.hpp"
#include "ekg/util/io.hpp"

std::map<std::string_view, ekg::service::theme_scheme_t> &ekg::service::theme::get_theme_scheme_map() {
  return this->theme_scheme_map;
}

void ekg::service::theme::add(ekg::service::theme_scheme_t theme) {
  this->theme_scheme_map[theme.name] = theme;
}

bool ekg::service::theme::set_current_theme_scheme(std::string_view name) {
  if (this->current_theme_scheme.name == name) {
    return true;
  }

  if (!this->theme_scheme_map.count(name)) {
    ekg::log() << "Failed to find theme '" << name << "' with this name!";
    return false;
  }

  this->current_theme_scheme = this->theme_scheme_map[name];
  return true;
}

ekg::service::theme_scheme_t &ekg::service::theme::get_current_theme_scheme() {
  return this->current_theme_scheme;
}

void ekg::service::theme::init() {
  ekg::log() << "Initialising default themes!";

  ekg::service::theme_scheme_t dark_theme_scheme {
    .name = "dark",
    .author = "Rina Wilk",
    .description = "Pasted dark-theme... mwm",
  };

  dark_theme_scheme.symmetric_layout = true;
  dark_theme_scheme.frame_background = ekg::color(43, 43, 43, 255);
  dark_theme_scheme.frame_border = ekg::color(190, 190, 190, 0);
  dark_theme_scheme.frame_outline = ekg::color(30, 40, 60, 100);
  dark_theme_scheme.frame_activity_offset = 18;
  dark_theme_scheme.button_background = ekg::color(85, 85, 85, 50);
  dark_theme_scheme.button_string = ekg::color(202, 202, 202, 255);
  dark_theme_scheme.button_outline = ekg::color(202, 207, 222, 0);
  dark_theme_scheme.button_activity = ekg::color(44, 166, 255, 100);
  dark_theme_scheme.button_highlight = ekg::color(44, 166, 255, 50);
  dark_theme_scheme.checkbox_background = ekg::color(85, 85, 85, 0);
  dark_theme_scheme.checkbox_string = ekg::color(202, 202, 202, 255);
  dark_theme_scheme.checkbox_outline = ekg::color(202, 207, 222, 0);
  dark_theme_scheme.checkbox_activity = ekg::color(44, 166, 255, 200);
  dark_theme_scheme.checkbox_highlight = ekg::color(44, 166, 255, 50);
  dark_theme_scheme.slider_background = ekg::color(85, 85, 85, 50);
  dark_theme_scheme.slider_bar_background = ekg::color(85, 85, 85, 50);
  dark_theme_scheme.slider_string = ekg::color(202, 202, 202, 255);
  dark_theme_scheme.slider_outline = ekg::color(202, 207, 222, 0);
  dark_theme_scheme.slider_activity = ekg::color(44, 166, 255, 200);
  dark_theme_scheme.slider_highlight = ekg::color(44, 166, 255, 50);
  dark_theme_scheme.slider_activity_bar = ekg::color(44, 166, 255, 200);
  dark_theme_scheme.slider_bar_outline = ekg::color(202, 207, 222, 0);
  dark_theme_scheme.slider_bar_thickness = 100;
  dark_theme_scheme.slider_target_thickness = 0;
  dark_theme_scheme.label_string = ekg::color(202, 202, 202, 255);
  dark_theme_scheme.popup_background = ekg::color(43, 43, 43, 255);
  dark_theme_scheme.popup_string = ekg::color(202, 202, 202, 255);
  dark_theme_scheme.popup_outline = ekg::color(43, 43, 43, 0);
  dark_theme_scheme.popup_highlight = ekg::color(44, 166, 255, 50);
  dark_theme_scheme.popup_separator = ekg::color(141, 141, 141, 50);
  dark_theme_scheme.popup_drop_animation_delay = 120;
  dark_theme_scheme.textbox_background = ekg::color(242, 242, 242, 255);
  dark_theme_scheme.textbox_string = ekg::color(141, 141, 141, 255);
  dark_theme_scheme.textbox_outline = ekg::color(141, 141, 141, 50);
  dark_theme_scheme.textbox_cursor = ekg::color(202, 202, 202, 255);
  dark_theme_scheme.textbox_select = ekg::color(44, 166, 255, 50);
  dark_theme_scheme.scrollbar_background = ekg::color(85, 85, 85, 255);
  dark_theme_scheme.scrollbar_outline = ekg::color(202, 207, 222, 150);
  dark_theme_scheme.scrollbar_activity = ekg::color(44, 166, 255, 200);
  dark_theme_scheme.scrollbar_highlight = ekg::color(44, 166, 255, 50);
  dark_theme_scheme.scrollbar_pixel_thickness = 5;
  dark_theme_scheme.scrollbar_min_bar_size = 30.0f;
  dark_theme_scheme.listbox_header_background = ekg::color(85, 85, 85, 255);
  dark_theme_scheme.listbox_item_highlight_outline = ekg::color(141, 141, 141, 0);
  dark_theme_scheme.listbox_header_highlight = ekg::color(44, 166, 255, 50);
  dark_theme_scheme.listbox_header_outline = ekg::color(141, 141, 141, 100);
  dark_theme_scheme.listbox_header_string = ekg::color(202, 202, 202, 255);
  dark_theme_scheme.listbox_item_outline = ekg::color(141, 141, 141, 0);
  dark_theme_scheme.listbox_item_background = ekg::color(85, 85, 85, 0);
  dark_theme_scheme.listbox_item_string = ekg::color(202, 202, 202, 255);
  dark_theme_scheme.listbox_item_highlight = ekg::color(44, 166, 255, 50);
  dark_theme_scheme.listbox_item_focused_outline = ekg::color(141, 141, 141, 0);
  dark_theme_scheme.listbox_item_focused = ekg::color(44, 166, 255, 100);
  dark_theme_scheme.listbox_line_separator = ekg::color(141, 141, 141, 100);
  dark_theme_scheme.listbox_outline = ekg::color(141, 141, 141, 100);
  dark_theme_scheme.listbox_background = ekg::color(85, 85, 85, 50);
  dark_theme_scheme.listbox_drag_outline = ekg::color(141, 141, 141, 100);
  dark_theme_scheme.listbox_drag_background = ekg::color(85, 85, 85, 50);

  this->add(dark_theme_scheme);

  ekg::service::theme_scheme_t light_theme_scheme {
    .name = "light",
    .author = "Rina Wilk",
    .description = "Pasted light-theme... moow",
  };

  light_theme_scheme.symmetric_layout = true;
  light_theme_scheme.frame_background = ekg::color(242, 242, 242, 255);
  light_theme_scheme.frame_border = ekg::color(190, 190, 190, 0);
  light_theme_scheme.frame_outline = ekg::color(202, 207, 222, 150);
  light_theme_scheme.frame_activity_offset = 18;
  light_theme_scheme.button_string = ekg::color(141, 141, 141, 255);
  light_theme_scheme.button_background = ekg::color(204, 204, 204, 50);
  light_theme_scheme.button_activity = ekg::color(44, 166, 255, 100);
  light_theme_scheme.button_outline = ekg::color(202, 207, 222, 0);
  light_theme_scheme.button_highlight = ekg::color(44, 166, 255, 50);
  light_theme_scheme.checkbox_string = ekg::color(141, 141, 141, 255);
  light_theme_scheme.checkbox_background = ekg::color(204, 204, 204, 0);
  light_theme_scheme.checkbox_activity = ekg::color(44, 166, 255, 200);
  light_theme_scheme.checkbox_outline = ekg::color(202, 207, 222, 0);
  light_theme_scheme.checkbox_highlight = ekg::color(44, 166, 255, 50);
  light_theme_scheme.slider_string = ekg::color(141, 141, 141, 255);
  light_theme_scheme.slider_background = ekg::color(204, 204, 204, 50);
  light_theme_scheme.slider_bar_background = ekg::color(204, 204, 204, 50);
  light_theme_scheme.slider_activity = ekg::color(44, 166, 255, 200);
  light_theme_scheme.slider_outline = ekg::color(202, 207, 222, 0);
  light_theme_scheme.slider_highlight = ekg::color(44, 166, 255, 50);
  light_theme_scheme.slider_activity_bar = ekg::color(44, 166, 255, 200);
  light_theme_scheme.slider_bar_outline = ekg::color(44, 166, 255, 200);
  light_theme_scheme.slider_bar_thickness = 16;
  light_theme_scheme.slider_target_thickness = 0;
  light_theme_scheme.label_string = ekg::color(141, 141, 141, 255);
  light_theme_scheme.popup_string = ekg::color(141, 141, 141, 255);
  light_theme_scheme.popup_background = ekg::color(242, 242, 242, 255);
  light_theme_scheme.popup_outline = ekg::color(30, 40, 60, 0);
  light_theme_scheme.popup_highlight = ekg::color(206, 225, 239, 255);
  light_theme_scheme.popup_separator = ekg::color(202, 207, 222, 150);
  light_theme_scheme.popup_drop_animation_delay = 120,
  light_theme_scheme.textbox_string = ekg::color(141, 141, 141, 255);
  light_theme_scheme.textbox_background = ekg::color(242, 242, 242, 255);
  light_theme_scheme.textbox_outline = ekg::color(202, 207, 222, 150);
  light_theme_scheme.textbox_select = ekg::color(44, 166, 255, 50);
  light_theme_scheme.textbox_cursor = ekg::color(141, 141, 141, 255);
  light_theme_scheme.scrollbar_background = ekg::color(202, 202, 202, 255);
  light_theme_scheme.scrollbar_outline = ekg::color(202, 207, 222, 150);
  light_theme_scheme.scrollbar_activity = ekg::color(44, 166, 255, 200);
  light_theme_scheme.scrollbar_highlight = ekg::color(44, 166, 255, 50);
  light_theme_scheme.scrollbar_pixel_thickness = 5;
  light_theme_scheme.scrollbar_min_bar_size = 30.0f;
  light_theme_scheme.listbox_header_background = ekg::color(204, 204, 204, 255);
  light_theme_scheme.listbox_header_highlight = ekg::color(44, 166, 255, 50);
  light_theme_scheme.listbox_header_outline = ekg::color(202, 207, 222, 50);
  light_theme_scheme.listbox_header_string = ekg::color(141, 141, 141, 255);
  light_theme_scheme.listbox_item_background = ekg::color(204, 204, 204, 0);
  light_theme_scheme.listbox_item_string = ekg::color(141, 141, 141, 255);
  light_theme_scheme.listbox_item_outline = ekg::color(202, 207, 222, 50);
  light_theme_scheme.listbox_item_highlight_outline = ekg::color(202, 207, 222, 0);
  light_theme_scheme.listbox_item_focused_outline = ekg::color(202, 207, 222, 0);
  light_theme_scheme.listbox_item_focused = ekg::color(44, 166, 255, 100);
  light_theme_scheme.listbox_item_highlight = ekg::color(44, 166, 255, 50);
  light_theme_scheme.listbox_line_separator = ekg::color(202, 207, 222, 100);
  light_theme_scheme.listbox_background = ekg::color(204, 204, 204, 50);
  light_theme_scheme.listbox_drag_background = ekg::color(204, 204, 204, 50);
  light_theme_scheme.listbox_drag_outline = ekg::color(202, 207, 222, 100);
  light_theme_scheme.listbox_outline = ekg::color(202, 207, 222, 100);

  this->add(light_theme_scheme);
  this->set_current_theme_scheme("dark");

  ekg::service::theme_scheme_t light_pinky_theme_scheme {
    .name = "light-pinky",
    .author = "Rina Wilk",
    .description = "Pasted light-theme... moow",
  };

  light_pinky_theme_scheme.symmetric_layout = true;
  light_pinky_theme_scheme.frame_background = ekg::color(242, 242, 242, 255);
  light_pinky_theme_scheme.frame_border = ekg::color(190, 190, 190, 0);
  light_pinky_theme_scheme.frame_outline = ekg::color(202, 207, 222, 150);
  light_pinky_theme_scheme.frame_activity_offset = 18;
  light_pinky_theme_scheme.button_string = ekg::color(141, 141, 141, 255);
  light_pinky_theme_scheme.button_background = ekg::color(204, 204, 204, 50);
  light_pinky_theme_scheme.button_activity = ekg::color(245, 169, 184, 100);
  light_pinky_theme_scheme.button_outline = ekg::color(202, 207, 222, 0);
  light_pinky_theme_scheme.button_highlight = ekg::color(245, 169, 184, 50);
  light_pinky_theme_scheme.checkbox_string = ekg::color(141, 141, 141, 255);
  light_pinky_theme_scheme.checkbox_background = ekg::color(204, 204, 204, 0);
  light_pinky_theme_scheme.checkbox_activity = ekg::color(245, 169, 184, 200);
  light_pinky_theme_scheme.checkbox_outline = ekg::color(202, 207, 222, 0);
  light_pinky_theme_scheme.checkbox_highlight = ekg::color(245, 169, 184, 50);
  light_pinky_theme_scheme.slider_string = ekg::color(141, 141, 141, 255);
  light_pinky_theme_scheme.slider_background = ekg::color(204, 204, 204, 50);
  light_pinky_theme_scheme.slider_bar_background = ekg::color(204, 204, 204, 50);
  light_pinky_theme_scheme.slider_activity = ekg::color(245, 169, 184, 200);
  light_pinky_theme_scheme.slider_outline = ekg::color(202, 207, 222, 0);
  light_pinky_theme_scheme.slider_highlight = ekg::color(245, 169, 184, 50);
  light_pinky_theme_scheme.slider_activity_bar = ekg::color(245, 169, 184, 200);
  light_pinky_theme_scheme.slider_bar_outline = ekg::color(245, 169, 184, 200);
  light_pinky_theme_scheme.slider_bar_thickness = 16;
  light_pinky_theme_scheme.slider_target_thickness = 0;
  light_pinky_theme_scheme.label_string = ekg::color(141, 141, 141, 255);
  light_pinky_theme_scheme.popup_string = ekg::color(141, 141, 141, 255);
  light_pinky_theme_scheme.popup_background = ekg::color(242, 242, 242, 255);
  light_pinky_theme_scheme.popup_outline = ekg::color(30, 40, 60, 0);
  light_pinky_theme_scheme.popup_highlight = ekg::color(206, 225, 239, 255);
  light_pinky_theme_scheme.popup_separator = ekg::color(202, 207, 222, 150);
  light_pinky_theme_scheme.popup_drop_animation_delay = 120,
  light_pinky_theme_scheme.textbox_string = ekg::color(141, 141, 141, 255);
  light_pinky_theme_scheme.textbox_background = ekg::color(242, 242, 242, 255);
  light_pinky_theme_scheme.textbox_outline = ekg::color(202, 207, 222, 150);
  light_pinky_theme_scheme.textbox_select = ekg::color(245, 169, 184, 50);
  light_pinky_theme_scheme.textbox_cursor = ekg::color(141, 141, 141, 255);
  light_pinky_theme_scheme.scrollbar_background = ekg::color(202, 202, 202, 255);
  light_pinky_theme_scheme.scrollbar_outline = ekg::color(202, 207, 222, 150);
  light_pinky_theme_scheme.scrollbar_activity = ekg::color(245, 169, 184, 200);
  light_pinky_theme_scheme.scrollbar_highlight = ekg::color(245, 169, 184, 50);
  light_pinky_theme_scheme.scrollbar_pixel_thickness = 5;
  light_pinky_theme_scheme.scrollbar_min_bar_size = 30.0f;
  light_pinky_theme_scheme.listbox_header_background = ekg::color(204, 204, 204, 255);
  light_pinky_theme_scheme.listbox_header_highlight = ekg::color(245, 169, 184, 50);
  light_pinky_theme_scheme.listbox_header_outline = ekg::color(202, 207, 222, 50);
  light_pinky_theme_scheme.listbox_header_string = ekg::color(141, 141, 141, 255);
  light_pinky_theme_scheme.listbox_item_background = ekg::color(204, 204, 204, 0);
  light_pinky_theme_scheme.listbox_item_string = ekg::color(141, 141, 141, 255);
  light_pinky_theme_scheme.listbox_item_outline = ekg::color(202, 207, 222, 50);
  light_pinky_theme_scheme.listbox_item_highlight_outline = ekg::color(202, 207, 222, 0);
  light_pinky_theme_scheme.listbox_item_focused_outline = ekg::color(202, 207, 222, 0);
  light_pinky_theme_scheme.listbox_item_focused = ekg::color(245, 169, 184, 100);
  light_pinky_theme_scheme.listbox_item_highlight = ekg::color(245, 169, 184, 50);
  light_pinky_theme_scheme.listbox_line_separator = ekg::color(202, 207, 222, 100);
  light_pinky_theme_scheme.listbox_background = ekg::color(204, 204, 204, 50);
  light_pinky_theme_scheme.listbox_drag_background = ekg::color(204, 204, 204, 50);
  light_pinky_theme_scheme.listbox_drag_outline = ekg::color(202, 207, 222, 100);
  light_pinky_theme_scheme.listbox_outline = ekg::color(202, 207, 222, 100);

  this->add(light_pinky_theme_scheme);

  ekg::service::theme_scheme_t dark_pinky_theme_scheme {
    .name = "dark-pinky",
    .author = "Rina Wilk",
    .description = "Pasted dark-theme... mooo mwm",
  };

  dark_pinky_theme_scheme.symmetric_layout = true;
  dark_pinky_theme_scheme.frame_background = ekg::color(43, 43, 43, 255);
  dark_pinky_theme_scheme.frame_border = ekg::color(190, 190, 190, 0);
  dark_pinky_theme_scheme.frame_outline = ekg::color(30, 40, 60, 100);
  dark_pinky_theme_scheme.frame_activity_offset = 18;
  dark_pinky_theme_scheme.button_background = ekg::color(85, 85, 85, 50);
  dark_pinky_theme_scheme.button_string = ekg::color(202, 202, 202, 255);
  dark_pinky_theme_scheme.button_outline = ekg::color(202, 207, 222, 0);
  dark_pinky_theme_scheme.button_activity = ekg::color(245, 169, 184, 100);
  dark_pinky_theme_scheme.button_highlight = ekg::color(245, 169, 184, 50);
  dark_pinky_theme_scheme.checkbox_background = ekg::color(85, 85, 85, 0);
  dark_pinky_theme_scheme.checkbox_string = ekg::color(202, 202, 202, 255);
  dark_pinky_theme_scheme.checkbox_outline = ekg::color(202, 207, 222, 0);
  dark_pinky_theme_scheme.checkbox_activity = ekg::color(245, 169, 184, 200);
  dark_pinky_theme_scheme.checkbox_highlight = ekg::color(245, 169, 184, 50);
  dark_pinky_theme_scheme.slider_background = ekg::color(85, 85, 85, 50);
  dark_pinky_theme_scheme.slider_bar_background = ekg::color(85, 85, 85, 50);
  dark_pinky_theme_scheme.slider_string = ekg::color(202, 202, 202, 255);
  dark_pinky_theme_scheme.slider_outline = ekg::color(202, 207, 222, 0);
  dark_pinky_theme_scheme.slider_activity = ekg::color(245, 169, 184, 200);
  dark_pinky_theme_scheme.slider_highlight = ekg::color(245, 169, 184, 50);
  dark_pinky_theme_scheme.slider_activity_bar = ekg::color(245, 169, 184, 200);
  dark_pinky_theme_scheme.slider_bar_outline = ekg::color(202, 207, 222, 0);
  dark_pinky_theme_scheme.slider_bar_thickness = 100;
  dark_pinky_theme_scheme.slider_target_thickness = 0;
  dark_pinky_theme_scheme.label_string = ekg::color(202, 202, 202, 255);
  dark_pinky_theme_scheme.popup_background = ekg::color(43, 43, 43, 255);
  dark_pinky_theme_scheme.popup_string = ekg::color(202, 202, 202, 255);
  dark_pinky_theme_scheme.popup_outline = ekg::color(43, 43, 43, 0);
  dark_pinky_theme_scheme.popup_highlight = ekg::color(245, 169, 184, 50);
  dark_pinky_theme_scheme.popup_separator = ekg::color(141, 141, 141, 50);
  dark_pinky_theme_scheme.popup_drop_animation_delay = 120;
  dark_pinky_theme_scheme.textbox_background = ekg::color(242, 242, 242, 255);
  dark_pinky_theme_scheme.textbox_string = ekg::color(141, 141, 141, 255);
  dark_pinky_theme_scheme.textbox_outline = ekg::color(141, 141, 141, 50);
  dark_pinky_theme_scheme.textbox_cursor = ekg::color(202, 202, 202, 255);
  dark_pinky_theme_scheme.textbox_select = ekg::color(245, 169, 184, 50);
  dark_pinky_theme_scheme.scrollbar_background = ekg::color(85, 85, 85, 255);
  dark_pinky_theme_scheme.scrollbar_outline = ekg::color(202, 207, 222, 150);
  dark_pinky_theme_scheme.scrollbar_activity = ekg::color(245, 169, 184, 200);
  dark_pinky_theme_scheme.scrollbar_highlight = ekg::color(245, 169, 184, 50);
  dark_pinky_theme_scheme.scrollbar_pixel_thickness = 5;
  dark_pinky_theme_scheme.scrollbar_min_bar_size = 30.0f;
  dark_pinky_theme_scheme.listbox_header_background = ekg::color(85, 85, 85, 255);
  dark_pinky_theme_scheme.listbox_item_highlight_outline = ekg::color(141, 141, 141, 0);
  dark_pinky_theme_scheme.listbox_header_highlight = ekg::color(245, 169, 184, 50);
  dark_pinky_theme_scheme.listbox_header_outline = ekg::color(141, 141, 141, 100);
  dark_pinky_theme_scheme.listbox_header_string = ekg::color(202, 202, 202, 255);
  dark_pinky_theme_scheme.listbox_item_outline = ekg::color(141, 141, 141, 0);
  dark_pinky_theme_scheme.listbox_item_background = ekg::color(85, 85, 85, 0);
  dark_pinky_theme_scheme.listbox_item_string = ekg::color(202, 202, 202, 255);
  dark_pinky_theme_scheme.listbox_item_highlight = ekg::color(245, 169, 184, 50);
  dark_pinky_theme_scheme.listbox_item_focused_outline = ekg::color(141, 141, 141, 0);
  dark_pinky_theme_scheme.listbox_item_focused = ekg::color(245, 169, 184, 100);
  dark_pinky_theme_scheme.listbox_line_separator = ekg::color(141, 141, 141, 100);
  dark_pinky_theme_scheme.listbox_outline = ekg::color(141, 141, 141, 100);
  dark_pinky_theme_scheme.listbox_background = ekg::color(85, 85, 85, 50);
  dark_pinky_theme_scheme.listbox_drag_outline = ekg::color(141, 141, 141, 100);
  dark_pinky_theme_scheme.listbox_drag_background = ekg::color(85, 85, 85, 50);

  this->add(dark_pinky_theme_scheme);
}