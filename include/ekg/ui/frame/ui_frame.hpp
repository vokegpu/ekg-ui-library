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

#ifndef EKG_UI_FRAME_H
#define EKG_UI_FRAME_H

#include "ekg/ui/abstract/ui_abstract.hpp"
#include "ekg/util/geometry.hpp"

namespace ekg::ui {
  class frame : public ekg::ui::abstract {
  protected:
    ekg::flags dock_resize {};
    ekg::flags dock_drag {};

    ekg::rect rect_initial {};
    std::string tag {};
    ekg::vec2 scale_factor {};
    uint32_t top_level_frame_id {};
  public:
    ekg::ui::frame *set_place(ekg::flags dock);

    ekg::ui::frame *set_scale_factor(float x, float y);

    ekg::vec2 get_scale_factor();

    ekg::ui::frame *set_drag(ekg::flags dock);

    ekg::flags get_drag_dock();

    ekg::ui::frame *set_resize(ekg::flags dock);

    ekg::flags get_resize_dock();

    ekg::ui::frame *set_pos_initial(float x, float y);

    ekg::vec2 get_pos_initial();

    ekg::ui::frame *set_size_initial(float x, float y);

    ekg::vec2 get_size_initial();

    ekg::ui::frame *set_initial_width(float w);

    float get_initial_width();

    ekg::ui::frame *set_initial_height(float h);

    float get_initial_height();

    ekg::ui::frame *set_size(float w, float h);

    ekg::vec2 get_size();

    ekg::ui::frame *set_pos(float x, float y);

    ekg::vec2 get_pos();

    ekg::ui::frame *set_width(float w);

    float get_width();

    ekg::ui::frame *set_height(float h);

    float get_height();

    /**
     * Make automatically the parent ID as a top-level frame.\n
     * By calling `set_top_level_frame_id` with the `parent_id`.
     **/
    ekg::ui::frame *make_parent_top_level();

    /**
     * Set the top-level frame.\n
     * A top-level frame bypass the drag action in the current frame,
     * dragging the top-level frame instead of currentt frame.
     **/
    ekg::ui::frame *set_top_level_frame(ekg::ui::frame *p_frame);

    /**
     * Set the top-level frame.\n
     * A top-level frame bypass the drag action in the current frame,
     * dragging the top-level frame instead of currentt frame.
     **/
    ekg::ui::frame *set_top_level_frame_id(uint32_t frame_id);

    /**
     * Get the top-level frame.
     **/
    uint32_t get_top_level_frame_id();
  };
}

#endif