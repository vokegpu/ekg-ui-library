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
#include "ekg/handler/theme/handler.hpp"
#include "ekg/io/log.hpp"

void ekg::handler::theme::init() {
  ekg::log() << "Initializing default themes";

  ekg::theme_t light_pinky_theme {
    .tag = "light-pinky",
    .author = "Rina Wilk",
    .description = "amo vc amo vc"
  };

  light_pinky_theme.layout_offset = 2.0f;
  light_pinky_theme.layout_margin_thickness = 2;
  light_pinky_theme.frame_color_scheme.background = {242, 242, 242, 255};
  light_pinky_theme.frame_color_scheme.highlight = {242, 242, 242, 0};
  light_pinky_theme.frame_color_scheme.outline = {190, 190, 190, 0};
  light_pinky_theme.frame_color_scheme.active = {242, 242, 242, 0};
  light_pinky_theme.frame_color_scheme.focused_background = {242, 242, 242, 0};
  light_pinky_theme.frame_color_scheme.focused_outline = {242, 242, 242, 0};
  light_pinky_theme.frame_color_scheme.warning_outline = {242, 242, 0, 100};
  light_pinky_theme.frame_color_scheme.actions_margin_pixel_thickness = 18;

  light_pinky_theme.button_color_scheme.text_foreground = {141, 141, 141, 255};
  light_pinky_theme.button_color_scheme.background = {204, 204, 204, 50};
  light_pinky_theme.button_color_scheme.active = {245, 169, 184, 100};
  light_pinky_theme.button_color_scheme.outline = {202, 207, 222, 0};
  light_pinky_theme.button_color_scheme.highlight = {245, 169, 184, 50};
  light_pinky_theme.button_color_scheme.text_foreground = {141, 141, 141, 255};
  light_pinky_theme.button_color_scheme.box_outline = light_pinky_theme.button_color_scheme.outline;
  light_pinky_theme.button_color_scheme.box_active = {245, 169, 184, 200};
  light_pinky_theme.button_color_scheme.box_highlight = {245, 169, 184, 50};
  light_pinky_theme.button_color_scheme.box_background = {202, 207, 222, 100};

  light_pinky_theme.label_color_scheme.background = {204, 204, 204, 0};
  light_pinky_theme.label_color_scheme.outline = {202, 207, 222, 0};
  light_pinky_theme.label_color_scheme.text_foreground = {141, 141, 141, 255};

  light_pinky_theme.scrollbar_color_scheme.background = {204, 204, 204, 30};
  light_pinky_theme.scrollbar_color_scheme.outline = {204, 204, 204, 0};
  light_pinky_theme.scrollbar_color_scheme.bar_background = {245, 169, 184, 100};
  light_pinky_theme.scrollbar_color_scheme.bar_highlight = {245, 169, 184, 50};
  light_pinky_theme.scrollbar_color_scheme.bar_active = {245, 169, 184, 100};

  light_pinky_theme.slider_color_scheme.outline = {204, 204, 204, 0};
  light_pinky_theme.slider_color_scheme.background = {204, 204, 204, 30};
  light_pinky_theme.slider_color_scheme.bar_background = {204, 204, 204, 50};
  light_pinky_theme.slider_color_scheme.bar_highlight = {245, 169, 184, 50};
  light_pinky_theme.slider_color_scheme.bar_active = {245, 169, 184, 100};
  light_pinky_theme.slider_color_scheme.text_foreground = {141, 141, 141, 255};

  light_pinky_theme.popup_color_scheme = light_pinky_theme.frame_color_scheme;
  light_pinky_theme.popup_color_scheme.outline = {50, 50, 50, 100};
  light_pinky_theme.popup_color_scheme.popup_mode = true;

  light_pinky_theme.textbox_color_scheme.background = {242, 242, 242, 255};
  light_pinky_theme.textbox_color_scheme.outline = {190, 190, 190, 100};
  light_pinky_theme.textbox_color_scheme.text_foreground = {141, 141, 141, 255};
  light_pinky_theme.textbox_color_scheme.text_cursor_foreground = {141, 141, 141, 255};
  light_pinky_theme.textbox_color_scheme.text_select_foreground = {245, 169, 184, 50};
  light_pinky_theme.textbox_color_scheme.text_select_outline = {245, 169, 184, 100};

  this->registry(light_pinky_theme.tag) = light_pinky_theme;
  this->set_current_theme(light_pinky_theme.tag);

  ekg::theme_t black_light_pinky_theme {
    .tag = "black-light-pinky",
    .author = "Rina Wilk",
    .description = "misses"
  };

  black_light_pinky_theme.layout_offset = 2.0f;
  black_light_pinky_theme.layout_margin_thickness = 2;
  black_light_pinky_theme.frame_color_scheme.background = {40, 40, 40, 255};
  black_light_pinky_theme.frame_color_scheme.highlight = {242, 242, 242, 0};
  black_light_pinky_theme.frame_color_scheme.outline = {190, 190, 190, 255};
  black_light_pinky_theme.frame_color_scheme.active = {242, 242, 242, 0};
  black_light_pinky_theme.frame_color_scheme.focused_background = {242, 242, 242, 0};
  black_light_pinky_theme.frame_color_scheme.focused_outline = {242, 242, 242, 0};
  black_light_pinky_theme.frame_color_scheme.warning_outline = {242, 242, 0, 100};
  black_light_pinky_theme.frame_color_scheme.actions_margin_pixel_thickness = 18;

  black_light_pinky_theme.button_color_scheme.text_foreground = {141, 141, 141, 255};
  black_light_pinky_theme.button_color_scheme.background = {204, 204, 204, 50};
  black_light_pinky_theme.button_color_scheme.active = {245, 169, 184, 100};
  black_light_pinky_theme.button_color_scheme.outline = {202, 207, 222, 0};
  black_light_pinky_theme.button_color_scheme.highlight = {245, 169, 184, 50};
  black_light_pinky_theme.button_color_scheme.text_foreground = {141, 141, 141, 255};
  black_light_pinky_theme.button_color_scheme.box_outline = light_pinky_theme.button_color_scheme.outline;
  black_light_pinky_theme.button_color_scheme.box_active = {245, 169, 184, 200};
  black_light_pinky_theme.button_color_scheme.box_highlight = {245, 169, 184, 50};
  black_light_pinky_theme.button_color_scheme.box_background = {202, 207, 222, 100};

  black_light_pinky_theme.label_color_scheme.background = {204, 204, 204, 0};
  black_light_pinky_theme.label_color_scheme.outline = {202, 207, 222, 0};
  black_light_pinky_theme.label_color_scheme.text_foreground = {141, 141, 141, 255};

  black_light_pinky_theme.scrollbar_color_scheme.background = {204, 204, 204, 30};
  black_light_pinky_theme.scrollbar_color_scheme.outline = {204, 204, 204, 0};
  black_light_pinky_theme.scrollbar_color_scheme.bar_background = {245, 169, 184, 100};
  black_light_pinky_theme.scrollbar_color_scheme.bar_highlight = {245, 169, 184, 50};
  black_light_pinky_theme.scrollbar_color_scheme.bar_active = {245, 169, 184, 100};

  black_light_pinky_theme.slider_color_scheme.background = {204, 204, 204, 50};
  black_light_pinky_theme.slider_color_scheme.outline = {204, 204, 204, 0};
  black_light_pinky_theme.slider_color_scheme.bar_background = {204, 204, 204, 50};
  black_light_pinky_theme.slider_color_scheme.bar_highlight = {245, 169, 184, 50};
  black_light_pinky_theme.slider_color_scheme.bar_active = {245, 169, 184, 100};
  black_light_pinky_theme.slider_color_scheme.text_foreground = {141, 141, 141, 255};

  black_light_pinky_theme.popup_color_scheme = black_light_pinky_theme.frame_color_scheme;
  black_light_pinky_theme.popup_color_scheme.popup_mode = true;

  black_light_pinky_theme.textbox_color_scheme.background = {242, 242, 242, 255};
  black_light_pinky_theme.textbox_color_scheme.outline = {190, 190, 190, 100};
  black_light_pinky_theme.textbox_color_scheme.text_foreground = {141, 141, 141, 255};
  black_light_pinky_theme.textbox_color_scheme.text_cursor_foreground = {141, 141, 141, 255};
  black_light_pinky_theme.textbox_color_scheme.text_select_foreground = {245, 169, 184, 50};
  black_light_pinky_theme.textbox_color_scheme.text_select_outline = {245, 169, 184, 100};

  this->registry(black_light_pinky_theme.tag) = black_light_pinky_theme;
  //this->set_current_theme(black_light_pinky_theme.tag);

  ekg::theme_t black_pinky {
    .tag = "black-pinky",
    .author = "weasted",
    .description = "colors for show colors"
  };

  black_pinky.layout_offset = 2.0f;
  black_pinky.layout_margin_thickness = 2;
  black_pinky.frame_color_scheme.background = {242, 242, 242, 255};
  black_pinky.frame_color_scheme.highlight = {242, 242, 242, 0};
  black_pinky.frame_color_scheme.outline = {190, 190, 190, 0};
  black_pinky.frame_color_scheme.active = {242, 242, 242, 0};
  black_pinky.frame_color_scheme.focused_background = {242, 242, 242, 0};
  black_pinky.frame_color_scheme.focused_outline = {242, 242, 242, 0};
  black_pinky.frame_color_scheme.warning_outline = {242, 242, 0, 100};
  black_pinky.frame_color_scheme.actions_margin_pixel_thickness = 18;

  black_pinky.button_color_scheme.text_foreground = {141, 141, 141, 255};
  black_pinky.button_color_scheme.background = {204, 204, 204, 50};
  black_pinky.button_color_scheme.active = {245, 169, 184, 100};
  black_pinky.button_color_scheme.outline = {202, 207, 222, 0};
  black_pinky.button_color_scheme.highlight = {245, 169, 184, 50};
  black_pinky.button_color_scheme.text_foreground = {141, 141, 141, 255};
  black_pinky.button_color_scheme.box_outline = black_pinky.button_color_scheme.outline;
  black_pinky.button_color_scheme.box_active = {245, 169, 184, 200};
  black_pinky.button_color_scheme.box_highlight = {245, 169, 184, 50};
  black_pinky.button_color_scheme.box_background = {202, 207, 222, 100};

  black_pinky.label_color_scheme.background = {204, 204, 204, 0};
  black_pinky.label_color_scheme.outline = {202, 207, 222, 0};
  black_pinky.label_color_scheme.text_foreground = {141, 141, 141, 255};

  black_pinky.scrollbar_color_scheme.background = {204, 204, 204, 30};
  black_pinky.scrollbar_color_scheme.outline = {204, 204, 204, 0};
  black_pinky.scrollbar_color_scheme.bar_background = {245, 169, 184, 100};
  black_pinky.scrollbar_color_scheme.bar_highlight = {245, 169, 184, 50};
  black_pinky.scrollbar_color_scheme.bar_active = {245, 169, 184, 100};

  black_pinky.slider_color_scheme.outline = {204, 204, 204, 0};
  black_pinky.slider_color_scheme.background = {204, 204, 204, 30};
  black_pinky.slider_color_scheme.bar_background = {204, 204, 204, 50};
  black_pinky.slider_color_scheme.bar_highlight = {245, 169, 184, 50};
  black_pinky.slider_color_scheme.bar_active = {245, 169, 184, 100};
  black_pinky.slider_color_scheme.text_foreground = {141, 141, 141, 255};

  black_pinky.popup_color_scheme = black_pinky.frame_color_scheme;
  black_pinky.popup_color_scheme.outline = {50, 50, 50, 100};
  black_pinky.popup_color_scheme.popup_mode = true;

  black_pinky.textbox_color_scheme.background = {0, 0, 0, 255};
  black_pinky.textbox_color_scheme.outline = {190, 190, 190, 100};
  black_pinky.textbox_color_scheme.text_foreground = {242, 242, 242, 255};
  black_pinky.textbox_color_scheme.text_cursor_foreground = {141, 141, 141, 255};
  black_pinky.textbox_color_scheme.text_select_foreground = {245, 169, 184, 50};
  black_pinky.textbox_color_scheme.text_select_outline = {245, 169, 184, 100};

  this->registry(black_pinky.tag) = black_pinky;
  this->set_current_theme(black_pinky.tag);
}

void ekg::handler::theme::quit() {
}

ekg::theme_t &ekg::handler::theme::registry(const std::string &tag) {
  return this->themes[tag];
}

ekg::theme_t &ekg::handler::theme::set_current_theme(const std::string &tag) {
  this->current_theme_tag = tag;
  return this->themes[tag];
}

ekg::theme_t &ekg::handler::theme::get_current_theme() {
  return this->themes[this->current_theme_tag];
}
