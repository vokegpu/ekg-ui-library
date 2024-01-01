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

#include "ekg/service/theme.hpp"
#include "ekg/util/io.hpp"

std::string ekg::service::theme::get_current_theme_name() {
  return this->current_theme;
}

void ekg::service::theme::init() {
  ekg::log() << "Initialising default theme";
  this->gen_default_light_theme();
}

void ekg::service::theme::quit() {

}

void ekg::service::theme::gen_default_dark_theme() {
  this->current_theme = "dark";
  this->symmetric_layout = true;

  this->frame_background = ekg::color(43, 43, 43, 255);
  this->frame_border = ekg::color(190, 190, 190, 0);
  this->frame_outline = ekg::color(30, 40, 60, 100);
  this->frame_activy_offset = 18;

  this->button_string = ekg::color(202, 202, 202, 255);
  this->button_background = ekg::color(85, 85, 85, 50);
  this->button_activy = ekg::color(44, 166, 255, 100);
  this->button_outline = ekg::color(202, 207, 222, 0);
  this->button_highlight = ekg::color(44, 166, 255, 50);

  this->checkbox_string = ekg::color(202, 202, 202, 255);
  this->checkbox_background = ekg::color(85, 85, 85, 0);
  this->checkbox_activy = ekg::color(44, 166, 255, 200);
  this->checkbox_outline = ekg::color(202, 207, 222, 0);
  this->checkbox_highlight = ekg::color(44, 166, 255, 50);

  this->slider_string = ekg::color(202, 202, 202, 255);
  this->slider_background = ekg::color(85, 85, 85, 50);
  this->slider_activy = ekg::color(44, 166, 255, 200);
  this->slider_outline = ekg::color(202, 207, 222, 0);
  this->slider_highlight = ekg::color(44, 166, 255, 50);
  this->slider_activy_bar = this->slider_activy;
  this->slider_bar_thickness = 16;
  this->slider_target_thickness = 0;

  this->label_string = ekg::color(202, 202, 202, 255);
  this->popup_string = ekg::color(202, 202, 202, 255);
  this->popup_background = ekg::color(43, 43, 43, 255);
  this->popup_outline = ekg::color(43, 43, 43, 0);
  this->popup_highlight = ekg::color(44, 166, 255, 50);
  this->popup_separator = ekg::color(141, 141, 141, 50);
  this->popup_drop_animation_delay = 120;

  this->textbox_string = ekg::color(202, 202, 202, 255);
  this->textbox_background = ekg::color(43, 43, 43, 255);
  this->textbox_outline = ekg::color(141, 141, 141, 50);
  this->textbox_select = ekg::color(44, 166, 255, 50);
  this->textbox_cursor = ekg::color(202, 202, 202, 255);

  this->scrollbar_background = ekg::color(85, 85, 85, 255);
  this->scrollbar_outline = ekg::color(202, 207, 222, 150);
  this->scrollbar_activy = ekg::color(44, 166, 255, 200);
  this->scrollbar_highlight = ekg::color(44, 166, 255, 50);
  this->scrollbar_pixel_thickness = 5;

  this->listbox_item_outline = ekg::color(141, 141, 141, 0);
  this->listbox_item_background = ekg::color(85, 85, 85, 0);
  this->listbox_item_string = ekg::color(202, 202, 202, 255);
  this->listbox_category_background = ekg::color(85, 85, 85, 0);
  this->listbox_category_string = ekg::color(202, 202, 202, 255);
  this->listbox_category_outline = ekg::color(141, 141, 141, 50);
  this->listbox_separator = ekg::color(141, 141, 141, 50);
  this->listbox_outline = ekg::color(141, 141, 141, 50);
  this->listbox_highlight = ekg::color(44, 166, 255, 50);
}

void ekg::service::theme::gen_default_light_theme() {
  this->current_theme = "light";
  this->symmetric_layout = true;

  this->frame_background = ekg::color(242, 242, 242, 255);
  this->frame_border = ekg::color(190, 190, 190, 0);
  this->frame_outline = ekg::color(202, 207, 222, 150);
  this->frame_activy_offset = 18;

  this->button_string = ekg::color(141, 141, 141, 255);
  this->button_background = ekg::color(204, 204, 204, 50);
  this->button_activy = ekg::color(44, 166, 255, 100);
  this->button_outline = ekg::color(202, 207, 222, 0);
  this->button_highlight = ekg::color(44, 166, 255, 50);

  this->checkbox_string = ekg::color(141, 141, 141, 255);
  this->checkbox_background = ekg::color(204, 204, 204, 0);
  this->checkbox_activy = ekg::color(44, 166, 255, 200);
  this->checkbox_outline = ekg::color(202, 207, 222, 0);
  this->checkbox_highlight = ekg::color(44, 166, 255, 50);

  this->slider_string = ekg::color(141, 141, 141, 255);
  this->slider_background = {204, 204, 204, 50};
  this->slider_activy = ekg::color(44, 166, 255, 200);
  this->slider_outline = ekg::color(202, 207, 222, 0);
  this->slider_highlight = ekg::color(44, 166, 255, 50);
  this->slider_activy_bar = this->slider_activy;
  this->slider_bar_thickness = 16;
  this->slider_target_thickness = 0;

  this->label_string = ekg::color(141, 141, 141, 255);

  this->popup_string = ekg::color(141, 141, 141, 255);
  this->popup_background = ekg::color(242, 242, 242, 255);
  this->popup_outline = ekg::color(30, 40, 60, 0);
  this->popup_highlight = ekg::color(206, 225, 239, 255);
  this->popup_separator = ekg::color(202, 207, 222, 150);
  this->popup_drop_animation_delay = 120;

  this->textbox_string = ekg::color(141, 141, 141, 255);
  this->textbox_background = ekg::color(242, 242, 242, 255);
  this->textbox_outline = ekg::color(202, 207, 222, 150);
  this->textbox_select = ekg::color(44, 166, 255, 50);
  this->textbox_cursor = ekg::color(141, 141, 141, 255);

  this->scrollbar_background = ekg::color(202, 202, 202, 255);
  this->scrollbar_outline = ekg::color(202, 207, 222, 150);
  this->scrollbar_activy = ekg::color(44, 166, 255, 200);
  this->scrollbar_highlight = ekg::color(44, 166, 255, 50);
  this->scrollbar_pixel_thickness = 5;

  this->listbox_item_background = ekg::color(204, 204, 204, 0);
  this->listbox_item_string = ekg::color(141, 141, 141, 255);
  this->listbox_item_outline = ekg::color(202, 207, 222, 0);
  this->listbox_category_background = ekg::color(204, 204, 204, 0);
  this->listbox_category_string = ekg::color(141, 141, 141, 255);
  this->listbox_category_outline = ekg::color(202, 207, 222, 150);
  this->listbox_separator = ekg::color(202, 207, 222, 150);
  this->listbox_outline = ekg::color(202, 207, 222, 150);
  this->listbox_highlight = ekg::color(44, 166, 255, 50);
}

void ekg::service::theme::refresh_theme_list() {
  ekg::log() << "Analysing files for themes";

  this->loaded_theme_list.clear();

  /*
   * Important: do not use filesystem, it is not compatible with mostly recently version of NDK SDK.
   */
}

void ekg::service::theme::load_theme(const std::string &theme) {
  bool keep_load_theme {};

  for (std::string &themes: this->loaded_theme_list) {
    keep_load_theme = themes == theme;

    if (keep_load_theme) {
      break;
    }
  }

  if (keep_load_theme) {
    // todo add theme load.
  }
}
