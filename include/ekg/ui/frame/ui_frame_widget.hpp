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

#ifndef EKG_UI_FRAME_WIDGET_H
#define EKG_UI_FRAME_WIDGET_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include "ekg/ui/scrollbar/ui_scrollbar_embedded_widget.hpp"

namespace ekg::ui {
  class frame_widget : public ekg::ui::abstract_widget {
  public:
    ekg::flags target_dock_drag {};
    ekg::flags target_dock_resize {};

    ekg::ui::frame_widget *p_frame_widget_top_level {};
    ekg::ui::scrollbar_embedded_widget *p_scroll_embedded {};
    ekg::docker docker_activity_drag {};
    ekg::docker docker_activity_resize {};
    ekg::rect rect_delta {};

    float ui_theme_activity_offset {};
  public:
    void on_reload() override;

    void on_event(ekg::os::io_event_serial &io_event_serial) override;

    void on_update() override;

    void on_draw_refresh() override;
  };
}

#endif