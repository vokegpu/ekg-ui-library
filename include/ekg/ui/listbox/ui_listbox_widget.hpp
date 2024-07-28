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

#ifndef EKG_UI_LISTBOX_WIDGET_H
#define EKG_UI_LISTBOX_WIDGET_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include "ekg/util/io.hpp"
#include "ekg/ui/scrollbar/ui_scrollbar_embedded_widget.hpp"
#include "ekg/ui/listbox/ui_listbox.hpp"
#include "ekg/service/theme.hpp"
#include "ekg/draw/font_renderer.hpp"

namespace ekg::ui {
  class listbox_widget : public ekg::ui::abstract_widget {
  public:
    void render_item(
      ekg::item &item_header,
      ekg::placement &placement_header,
      ekg::rect widget_absolute_rect_scissor,
      ekg::rect scrollable_rect,
      ekg::rect content_scissor_bounding,
      float header_relative_x,
      float bottom_place,
      float scrolling_cropy,
      bool is_scroll_on_top,
      bool is_header_targeted,
      bool is_column_header_top,
      ekg::draw::font_renderer &f_renderer
    );
  public:
    ekg::ui::scrollbar_embedded_widget embedded_scroll {};
    ekg::item item_rendering_cache {};
    ekg::rect rect_content_abs {};
    ekg::rect rect_content_place {};
    ekg::rect rect_header_delta {};
    ekg::rect rect_header_target {};
    ekg::rect rect_targeting_header {};
    ekg::rect rect_current_dragging_targeted_header {};
    ekg::rect rect_original_dragging_targeted_header {};

    float column_header_height {};
    bool was_hovered {};
    bool was_selected {};
    bool must_update_items {};

    int32_t targeting_header_to_resize {};
    int32_t targeting_header_to_drag {};
    int32_t target_resizing {-1};
    int32_t target_dragging {-1};
    int32_t latest_target_dragging {-1};
    int32_t target_drag_set_index {-1};
  public:
    void on_create() override;

    void on_reload() override;

    void on_update() override;

    void on_post_event(ekg::os::io_event_serial &io_event_serial) override;

    void on_event(ekg::os::io_event_serial &io_event_serial) override;

    void on_pre_event(ekg::os::io_event_serial &io_event_serial) override;

    void on_draw_refresh() override;
  };
}

namespace ekg::ui {
  void listbox_template_reload(
    ekg::item &rendering_cache,
    ekg::item &parent,
    ekg::rect &ui_rect,
    ekg::rect &header_rect,
    ekg::font &item_font,
    ekg::rect &relative_rect,
    int32_t item_scaled_height,
    uint64_t header_index,
    uint64_t &arbitrary_index_pos,
    uint64_t &rendering_cache_arbitrary_index_pos,
    bool &opened,
    ekg::mode mode,
    bool *p_semaphore
  );

  void listbox_template_on_event(
    ekg::os::io_event_serial &io_event_serial,
    uint64_t &arbitrary_index_pos,
    ekg::item &rendering_cache,
    bool motion,
    bool released,
    bool pressed_select_many,
    bool pressed_select,
    bool pressed_open,
    bool &was_selected,
    ekg::item &parent,
    ekg::vec2 &ui_pos,
    ekg::rect &ui_rect,
    ekg::rect &relative_rect,
    ekg::mode mode
  );
}

#endif
