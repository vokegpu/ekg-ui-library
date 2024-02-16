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

#ifndef EKG_UI_POPUP_WIDGET_H
#define EKG_UI_POPUP_WIDGET_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"

// @TODO State for components/elements disabled
namespace ekg::ui {
  class popup_widget : public ekg::ui::abstract_widget {
  public:
    float scissor_opened_height {};
    float separator_offset {};

    uint64_t elapsed_animation_ticks {};
    bool parent_id_popup_opened {false};

    int32_t hovered_element {};
    int32_t popup_opened {-1};
    int32_t top_level_popup {};
  public:
    bool is_hovering_any_popup(int32_t top_level);

    void unset_visible_all_sub_popup();

    void get_popup_path(std::string &path);
  public:
    void on_reload() override;

    void on_pre_event(ekg::os::io_event_serial &io_event_serial) override;

    void on_event(ekg::os::io_event_serial &io_event_serial) override;

    void on_update() override;

    void on_draw_refresh() override;
  };
}

#endif