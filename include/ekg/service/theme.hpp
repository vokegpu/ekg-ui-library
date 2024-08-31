
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

#include <vector>
#include <string_view>
#include <map>
#include "ekg/util/geometry.hpp"

namespace ekg::service {
  struct theme_scheme_t {
  public:
    std::string_view name {};
    std::string_view author {};
    std::string_view description {};
    std::string_view path {};
  public:
    bool symmetric_layout {};
    float min_widget_size {5};

    ekg::vec4 frame_background {};
    ekg::vec4 frame_border {};
    ekg::vec4 frame_outline {};
    int32_t frame_activity_offset {};

    ekg::vec4 button_background {};
    ekg::vec4 button_string {};
    ekg::vec4 button_outline {};
    ekg::vec4 button_activity {};
    ekg::vec4 button_highlight {};

    ekg::vec4 checkbox_background {};
    ekg::vec4 checkbox_string {};
    ekg::vec4 checkbox_outline {};
    ekg::vec4 checkbox_activity {};
    ekg::vec4 checkbox_highlight {};

    ekg::vec4 slider_background {};
    ekg::vec4 slider_bar_background {};
    ekg::vec4 slider_string {};
    ekg::vec4 slider_outline {};
    ekg::vec4 slider_activity {};
    ekg::vec4 slider_activity_bar {};
    ekg::vec4 slider_bar_outline {};
    ekg::vec4 slider_highlight {};
    int32_t slider_bar_thickness {};
    int32_t slider_target_thickness {};

    ekg::vec4 label_string {};
    ekg::vec4 label_outline {};
    ekg::vec4 label_background {};

    ekg::vec4 popup_background {};
    ekg::vec4 popup_string {};
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
    ekg::vec4 scrollbar_outline {};
    ekg::vec4 scrollbar_activity {};
    ekg::vec4 scrollbar_highlight {};
    int32_t scrollbar_pixel_thickness {};
    float scrollbar_min_bar_size {};

    ekg::vec4 listbox_header_background {};
    ekg::vec4 listbox_header_highlight_outline {};
    ekg::vec4 listbox_header_highlight {};
    ekg::vec4 listbox_header_outline {};
    ekg::vec4 listbox_header_string {};
    ekg::vec4 listbox_item_background {};
    ekg::vec4 listbox_item_highlight_outline {};
    ekg::vec4 listbox_item_highlight {};
    ekg::vec4 listbox_item_focused {};
    ekg::vec4 listbox_item_focused_outline {};
    ekg::vec4 listbox_item_string {};
    ekg::vec4 listbox_item_outline {};
    ekg::vec4 listbox_outline {};
    ekg::vec4 listbox_background {};
    ekg::vec4 listbox_line_separator {};
    ekg::vec4 listbox_drag_background {};
    ekg::vec4 listbox_drag_outline {};
    float listbox_subitem_offset_space {4.0f};
  };

  class theme {
  protected:
    std::map<std::string_view, ekg::service::theme_scheme_t> theme_scheme_map {};
    ekg::service::theme_scheme_t current_theme_scheme {};
  public:
    /**
     * Initialize default themes (dark, light, pinky etc) and update global theme scheme.
     **/
    void init();

    /**
     * Returns all mapped schemes from theme service.
     * Note: Use property register/deregister methods; may be unsafe.
     **/
    std::map<std::string_view, ekg::service::theme_scheme_t> &get_theme_scheme_map();

    /**
     * Set the current theme global.
     * Note: You must set a registered theme.
     * 
     * Returns true if exists, else false.
     **/
    bool set_current_theme_scheme(std::string_view name);

    /**
     * Returns the current theme scheme global loaded.
     **/
    ekg::service::theme_scheme_t &get_current_theme_scheme();

    /**
     * Dynamic registry one theme scheme on memory, you must not repeat themes name.
     * Note: May you want save theme scheme in a file, use `ekg::service::theme::save`
     * method to do that.
     **/
    void add(ekg::service::theme_scheme_t theme_scheme);

    /**
     * Local save one theme scheme to a file.
     * You need to register a theme before use this.
     **/
    void save(std::string_view name, std::string_view path);

    /**
     * Read one theme scheme from a file and load it to memory; may replacing one
     * if already exists.
     **/
    void read(std::string_view path, ekg::service::theme_scheme_t *p_theme_scheme);
  };
}

#endif