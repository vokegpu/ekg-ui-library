/*
 * MIT License
 *
 * Copyright (c) 2022-2023 Rina Wilk / vokegpu@gmail.com
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

#ifndef EKG_UI_SCROLL_EMBEDDED_WIDGET_H
#define EKG_UI_SCROLL_EMBEDDED_WIDGET_H

#include "ekg/os/ekg_sdl.hpp"
#include "ekg/util/geometry.hpp"
#include "ekg/util/io.hpp"

namespace ekg::ui {
  class scroll_embedded_widget {
  public:
    std::vector<int32_t> child_id_list {};
    ekg::rect *rect_mother {};
    ekg::rect rect_child {};
    ekg::rect rect_dimension_sync {};
    ekg::rect last_rect_child {};

    ekg::vec4 scroll {};
    ekg::rect rect_vertical_scroll_bar {};
    ekg::rect rect_horizontal_scroll_bar {};

    ekg::flag flag {};
    int32_t mother_id {};
    int32_t widget_id {};

    bool is_vertical_enabled {};
    bool is_horizontal_enabled {};

    ekg::vec2 acceleration {};
    ekg::vec2 bar_drag {};
  public:
    explicit scroll_embedded_widget();

    void calculate_rect_bar_sizes();

    void clamp_scroll();

    bool is_dragging_bar();

    bool check_activy_state(bool state);

    void reset_scroll();

    void check_axis_states();
  public: // virtuals
    void on_reload();

    void on_pre_event(SDL_Event &sdl_event);

    void on_event(SDL_Event &sdl_event);

    void on_update();

    void on_draw_refresh();
  };
}

#endif