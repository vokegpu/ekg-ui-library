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

#include "ekg/service/layout.hpp"
#include "ekg/os/platform.hpp"
#include "ekg/ekg.hpp"
#include "ekg/ui/frame/ui_frame_widget.hpp"
#include "ekg/ui/display.hpp"

void ekg::service::layout::set_preset_mask(const ekg::vec3 &offset, ekg::axis axis, float initial_respective_size) {
  this->dock_axis_mask = axis;
  this->offset_mask = offset;
  this->respective_mask_all = initial_respective_size;
}

void ekg::service::layout::insert_into_mask(const ekg::dock_rect &dock_rect) {
  if ((static_cast<int32_t>(dock_rect.p_rect->w == 0) || static_cast<int32_t>(dock_rect.p_rect->h) == 0) ||
      ekg_bitwise_contains(dock_rect.dock, ekg::dock::none)) {
    return;
  }

  this->dock_rect_list.push_back(dock_rect);
}

void ekg::service::layout::process_layout_mask() {
  /*
   * V is the the respective size (axis horizontal == width | axis vertical == height)
   */
  bool axis {this->dock_axis_mask == ekg::axis::horizontal};
  if (this->dock_rect_list.empty()) {
    this->layout_mask.w = axis ? this->respective_mask_all : this->offset_mask.z;
    this->layout_mask.h = axis ? this->offset_mask.z : this->respective_mask_all;
    return;
  }

  float left_or_right {};
  float v {this->respective_mask_all};
  float centered_dimension {this->offset_mask.z / 2};
  float opposite {}, uniform {};
  float clamped_offset {};

  /* check for opposite dock and get the full size respective for the axis dock */
  if (v == 0) {
    v = this->get_respective_mask_size();
  }

  /* offset z is the dimension respective (width if height else height) size */
  this->layout_mask.w = axis ? this->offset_mask.x : this->offset_mask.z;
  this->layout_mask.h = axis ? this->offset_mask.z : this->offset_mask.y;

  /* axis false is equals X else is equals Y */
  for (ekg::dock_rect &dock_rect: this->dock_rect_list) {
    if (dock_rect.p_rect == nullptr) {
      continue;
    }

    if (axis) {
      clamped_offset = (dock_rect.p_rect->h + this->offset_mask.y) - this->offset_mask.z > 0 ? 0 : this->offset_mask.y;
      left_or_right = ekg_bitwise_contains(dock_rect.dock, ekg::dock::left) ||
                      ekg_bitwise_contains(dock_rect.dock, ekg::dock::right);

      if (ekg_bitwise_contains(dock_rect.dock, ekg::dock::center) && !left_or_right) {
        dock_rect.p_rect->x = (v / 2) - (dock_rect.p_rect->w / 2);
        dock_rect.p_rect->y = centered_dimension - (dock_rect.p_rect->h / 2);
        this->layout_mask.w += dock_rect.p_rect->w + this->offset_mask.x;
      } else if (ekg_bitwise_contains(dock_rect.dock, ekg::dock::center)) {
        dock_rect.p_rect->y = centered_dimension - (dock_rect.p_rect->h / 2);
      }

      /* when there is a opposite dock, layout should x the dock position to actual position */
      if (ekg_bitwise_contains(dock_rect.dock, ekg::dock::left)) {
        if (static_cast<int32_t>(opposite) != 0) {
          this->layout_mask.w -= opposite;
        }

        dock_rect.p_rect->x = this->layout_mask.w;
        this->layout_mask.w += dock_rect.p_rect->w + this->offset_mask.x + opposite;
        opposite = 0;
        uniform = dock_rect.p_rect->w + this->offset_mask.x;
      }

      if (ekg_bitwise_contains(dock_rect.dock, ekg::dock::right)) {
        if (static_cast<int32_t>(uniform) != 0) {
          this->layout_mask.w -= uniform;
        }

        this->layout_mask.w += dock_rect.p_rect->w;
        dock_rect.p_rect->x = v - this->layout_mask.w;
        this->layout_mask.w += uniform + this->offset_mask.x;
        opposite = dock_rect.p_rect->w + this->offset_mask.x;
        uniform = 0;
      }

      if (ekg_bitwise_contains(dock_rect.dock, ekg::dock::top)) {
        dock_rect.p_rect->y = clamped_offset;
      } else if (ekg_bitwise_contains(dock_rect.dock, ekg::dock::bottom)) {
        dock_rect.p_rect->y = this->offset_mask.z - clamped_offset - dock_rect.p_rect->h;
      }
    } else {
      clamped_offset = ekg_clamp((dock_rect.p_rect->w + this->offset_mask.x) - this->offset_mask.z, 0,
                                  this->offset_mask.x);
      left_or_right = ekg_bitwise_contains(dock_rect.dock, ekg::dock::left) ||
                      ekg_bitwise_contains(dock_rect.dock, ekg::dock::right);

      if (ekg_bitwise_contains(dock_rect.dock, ekg::dock::center) && !left_or_right) {
        dock_rect.p_rect->y = (this->respective_mask_center) + (dock_rect.p_rect->h / 2);
        dock_rect.p_rect->x = centered_dimension - (dock_rect.p_rect->w / 2);
      } else if (ekg_bitwise_contains(dock_rect.dock, ekg::dock::center)) {
        dock_rect.p_rect->x = centered_dimension - (dock_rect.p_rect->w / 2);
      }

      /* when there is a opposite dock, layout should fix the dock position to actual position */
      if (ekg_bitwise_contains(dock_rect.dock, ekg::dock::top)) {
        if (static_cast<int32_t>(opposite) != 0) {
          this->layout_mask.h -= opposite;
        }

        dock_rect.p_rect->y = this->layout_mask.h;
        this->layout_mask.h += dock_rect.p_rect->h + this->offset_mask.y + opposite;
        opposite = 0;
        uniform = dock_rect.p_rect->h + this->offset_mask.y;
      }

      if (ekg_bitwise_contains(dock_rect.dock, ekg::dock::bottom)) {
        if (static_cast<int32_t>(uniform) != 0) {
          this->layout_mask.h -= uniform;
        }

        this->layout_mask.h += dock_rect.p_rect->h;
        dock_rect.p_rect->y = v - this->layout_mask.h;
        this->layout_mask.h += uniform + this->offset_mask.y;
        opposite = dock_rect.p_rect->h + this->offset_mask.y;
        uniform = 0;
      }

      if (ekg_bitwise_contains(dock_rect.dock, ekg::dock::left)) {
        dock_rect.p_rect->x = clamped_offset;
      }

      if (ekg_bitwise_contains(dock_rect.dock, ekg::dock::right)) {
        dock_rect.p_rect->x = this->offset_mask.z - clamped_offset - dock_rect.p_rect->w;
      }
    }
  }

  if (axis) {
    this->layout_mask.w = ekg_min(v, this->layout_mask.w);
  } else {
    this->layout_mask.h = ekg_min(v, this->layout_mask.h);
  }

  this->dock_rect_list.clear();
}

ekg::rect &ekg::service::layout::get_layout_mask() {
  return this->layout_mask;
}

float ekg::service::layout::get_respective_mask_size() {
  if (this->dock_rect_list.empty()) {
    return 0;
  }

  float respective_size {(this->dock_axis_mask == ekg::axis::horizontal) ? this->offset_mask.x : this->offset_mask.y};
  float respective_center_size {respective_size};
  float size {};

  int32_t only_center_count {};

  for (ekg::dock_rect &dock_rect: this->dock_rect_list) {
    if (dock_rect.p_rect == nullptr) {
      continue;
    }

    size = (
      this->dock_axis_mask == ekg::axis::horizontal ?
      (dock_rect.p_rect->w + this->offset_mask.x) : (dock_rect.p_rect->h + this->offset_mask.y)
    );

    if (
        ekg_bitwise_contains(dock_rect.dock, ekg::dock::center) &&
        !(ekg_bitwise_contains(dock_rect.dock, ekg::dock::left) || ekg_bitwise_contains(dock_rect.dock, ekg::dock::right))
      ) {
      respective_center_size += size;
      only_center_count++;
    }

    respective_size += size;
  }

  this->respective_mask_center = (only_center_count != 0 ? (respective_center_size / only_center_count) : 0.0f);
  this->respective_mask_all = ekg_min(this->respective_mask_all, respective_size);

  return this->respective_mask_all;
}

float ekg::service::layout::get_dimensional_extent(
  ekg::ui::abstract_widget *p_widget,
  uint16_t flag_ok,
  uint16_t flag_stop,
  int64_t &begin_and_count,
  ekg::axis axis
) {
  if (p_widget == nullptr) {
    return 0.0f;
  }

  uint16_t flags {};
  int32_t ids {};
  int64_t n {};
  int64_t it {begin_and_count};

  if (begin_and_count > static_cast<int64_t>(this->extent_data[0]) &&
      begin_and_count < static_cast<int64_t>(this->extent_data[1])) {
    begin_and_count = static_cast<int64_t>(this->extent_data[3]);
    return this->extent_data[2];
  }

  this->extent_data[0] = static_cast<float>(it);

  float extent {};
  ekg::ui::abstract_widget *p_widgets {};
  std::vector<int32_t> &child_id_list {p_widget->p_data->get_child_id_list()};
  uint64_t size {child_id_list.size()};
  bool is_last_index {};

  /*
   * The last index does not check if there is some next flag,
   * because of this, there is a last index checker.
   *
   * The extent data store the previous bounding indices,
   * in simply words, prevent useless iteration.
   *
   * The min offset is added for extent, because we need count
   * the offset position when split the fill width, but the
   * last extent space is not necessary, so we need to subtract.
   */
  for (it = it; it < size; it++) {
    ids = child_id_list.at(it);
    if ((p_widgets = ekg::core->get_fast_widget_by_id(ids)) == nullptr) {
      continue;
    }

    flags = p_widgets->p_data->get_place_dock();
    is_last_index = it >= size - 1 || p_widgets->p_data->get_type() == ekg::type::scrollbar;

    if (
        (ekg_bitwise_contains(flags, flag_stop) && it != begin_and_count) || is_last_index
      ) {
      extent -= this->min_offset;
      n += (!ekg_bitwise_contains(flags, flag_stop) && ekg_bitwise_contains(flags, flag_ok) && is_last_index);

      this->extent_data[1] = static_cast<float>(it + is_last_index);
      this->extent_data[2] = extent;
      this->extent_data[3] = static_cast<float>(n == 0 ? 1 : n);
      break;
    }

    if (ekg_bitwise_contains(flags, flag_ok)) {
      n++;
      continue;
    }

    switch (axis) {
      case ekg::axis::horizontal:
        extent += p_widgets->dimension.w + this->min_offset;
        break;
      case ekg::axis::vertical:
        extent += p_widgets->dimension.h;
        break;
    }
  }

  begin_and_count = n == 0 ? 1 : n;
  return extent;
}

void ekg::service::layout::process_scaled(ekg::ui::abstract_widget *p_widget_parent) {
  if (p_widget_parent == nullptr || p_widget_parent->p_data == nullptr) {
    return;
  }

  auto type {p_widget_parent->p_data->get_type()};
  auto is_group {type == ekg::type::frame};
  auto &parent_rect = p_widget_parent->get_abs_rect();

  if (!is_group || parent_rect.w == 0 || parent_rect.h == 0) {
    return;
  }

  if (p_widget_parent->is_targeting_absolute_parent) {
    p_widget_parent->is_targeting_absolute_parent = false;
    this->process_scaled(ekg::find_absolute_parent_master(p_widget_parent));
    return;
  }

  float group_top_offset {this->min_offset};
  ekg::rect group_rect {p_widget_parent->dimension};

  bool has_scroll_embedded {};
  bool is_vertical_enabled {};
  float initial_offset {};

  switch (type) {
    case ekg::type::frame: {
      auto &theme {ekg::theme()};
      auto frame {(ekg::ui::frame_widget *) p_widget_parent};

      initial_offset = static_cast<float>(theme.scrollbar_pixel_thickness);
      has_scroll_embedded = frame->p_scroll_embedded != nullptr;

      if (has_scroll_embedded) {
        frame->p_scroll_embedded->check_axis_states();
        is_vertical_enabled = frame->p_scroll_embedded->is_vertical_enabled;
        initial_offset *= static_cast<float>(!theme.symmetric_layout);

        group_rect.w -= initial_offset * static_cast<float>(is_vertical_enabled);
        group_rect.h -= initial_offset * static_cast<float>(frame->p_scroll_embedded->is_horizontal_enabled);
      }

      initial_offset = static_cast<float>(theme.scrollbar_pixel_thickness) * static_cast<float>(theme.symmetric_layout);
      break;
    }

    default: {
      break;
    }
  }

  /*
   * The dimension of mother/parent group is not scaled by min offsets,
   * when transforming unuscaled sizes, it can return a wrong position,
   * in simple words, non-scaled size return non aligned positions.
   */
  group_rect.w -= (initial_offset + this->min_offset) * 2.0f;
  group_rect.h -= (initial_offset + this->min_offset) * 2.0f;

  this->scaled_width_divided = parent_rect.w / 3;
  this->scaled_width_divided = parent_rect.h / 2;
  this->enum_docks_flag = 0;

  ekg::ui::abstract_widget *p_widgets {};
  uint16_t flags {};
  int64_t it {};
  float dimensional_extent {};
  int64_t count {};

  ekg::rect widget_rect {};
  ekg::rect bottom_rect {};
  ekg::rect top_rect {this->min_offset + initial_offset, this->min_offset + initial_offset, 0.0f, 0.0f};
  ekg::rect prev_widget_layout {};

  uint16_t prev_flags {};

  /*
   * To find the precise fill width, then this data store
   * the previous extent finding and check the current it,
   * this prevents useless iterations and tick waste.
   */
  this->extent_data[0] = 0.0f;
  this->extent_data[1] = 0.0f;
  this->extent_data[2] = 0.0f;

  bool should_reload_widget {};
  bool skip_widget {};

  float max_previous_height {};
  float extent_data_backup[4] {};

  /*
   * Rect == absolute position of current widget
   * Layout == position based on parent absolute widget
   */
  for (int32_t &ids: p_widget_parent->p_data->get_child_id_list()) {
    if (ids == 0 || (p_widgets = ekg::core->get_fast_widget_by_id(ids)) == nullptr) {
      continue;
    }

    auto &layout {p_widgets->dimension};
    flags = p_widgets->p_data->get_place_dock();
    skip_widget = false;

    switch (p_widgets->p_data->get_type()) {
      case ekg::type::scrollbar: {
        skip_widget = true;
        break;
      }

      default: {
        break;
      }
    }

    // @TODO Prevent useless scrolling reload.
    p_widgets->on_reload();

    if (skip_widget) {
      it++;
      continue;
    }

    if (ekg_bitwise_contains(flags, ekg::dock::fill) && ekg_bitwise_contains(flags, ekg::dock::next)) {
      top_rect.h += max_previous_height + this->min_offset;
      top_rect.w = 0.0f;

      layout.x = top_rect.x + top_rect.w;
      layout.y = top_rect.y + top_rect.h;

      count = it;
      dimensional_extent = this->get_dimensional_extent(
        p_widget_parent,
        ekg::dock::fill,
        ekg::dock::next,
        count,
        ekg::axis::horizontal
      );

      dimensional_extent = ekg_min(
        ((group_rect.w - dimensional_extent) - (count * this->min_offset)) / static_cast<float>(count),
        p_widgets->min_size.x
      );

      top_rect.w += dimensional_extent + this->min_offset;
      layout.w = dimensional_extent;
      should_reload_widget = true;
      max_previous_height = 0.0f;
    } else if (ekg_bitwise_contains(flags, ekg::dock::fill)) {
      layout.x = top_rect.x + top_rect.w;
      layout.y = top_rect.y + top_rect.h;

      count = it;
      dimensional_extent = this->get_dimensional_extent(
        p_widget_parent,
        ekg::dock::fill,
        ekg::dock::next,
        count,
        ekg::axis::horizontal
      );

      dimensional_extent = ekg_min(
        ((group_rect.w - dimensional_extent) - (count * this->min_offset)) / static_cast<float>(count),
        p_widgets->min_size.x
      );

      top_rect.w += dimensional_extent + this->min_offset;
      layout.w = dimensional_extent;
      should_reload_widget = true;
    } else if (ekg_bitwise_contains(flags, ekg::dock::next)) {
      top_rect.h += max_previous_height + this->min_offset;
      top_rect.w = 0.0f;

      layout.x = top_rect.x;
      layout.y = top_rect.y + top_rect.h;
      top_rect.w += layout.w + this->min_offset;

      count = it;
      dimensional_extent = this->get_dimensional_extent(
        p_widget_parent,
        ekg::dock::fill,
        ekg::dock::next,
        count,
        ekg::axis::horizontal
      );

      max_previous_height = 0.0f;
    } else if (flags == ekg::dock::none) {
      layout.x = top_rect.x + top_rect.w;
      layout.y = top_rect.y + top_rect.h;
      top_rect.w += layout.w + this->min_offset;

      count = it;
      dimensional_extent = this->get_dimensional_extent(
        p_widget_parent,
        ekg::dock::fill,
        ekg::dock::next,
        count,
        ekg::axis::horizontal
      );
    }

    if (ekg_bitwise_contains(flags, ekg::dock::resize)) {
      should_reload_widget = true;
    }

    max_previous_height = layout.h > max_previous_height ? layout.h : max_previous_height;
    if (should_reload_widget) {
      p_widgets->on_reload();
    }

    extent_data_backup[0] = this->extent_data[0];
    extent_data_backup[1] = this->extent_data[1];
    extent_data_backup[2] = this->extent_data[2];
    extent_data_backup[3] = this->extent_data[3];

    if (p_widgets->p_data->has_children()) {
      this->process_scaled(p_widgets);
    }

    this->extent_data[0] = extent_data_backup[0];
    this->extent_data[1] = extent_data_backup[1];
    this->extent_data[2] = extent_data_backup[2];
    this->extent_data[3] = extent_data_backup[3];

    prev_widget_layout = layout;
    prev_flags = flags;
    it++;
  }

  if (has_scroll_embedded && !is_vertical_enabled) {
    switch (type) {
      case ekg::type::frame: {
        auto frame {(ekg::ui::frame_widget *) p_widget_parent};
        has_scroll_embedded = frame->p_scroll_embedded != nullptr;

        if (has_scroll_embedded && !is_vertical_enabled &&
            frame->p_scroll_embedded->is_vertical_enabled) {
          this->process_scaled(p_widget_parent);
        }

        break;
      }

      default: {
        break;
      }
    }
  }
}

void ekg::service::layout::init() {
  this->min_factor_height = 1;
  this->min_height = ekg::core->f_renderer_normal.get_text_height();
  this->min_fill_width = this->min_height;

  /*
   * Same as font.cpp:
   * A common issue with rendering overlay elements is flot32 imprecision, for this reason
   * the cast float32 to int32 is necessary.
   * Extra:
   * I am totally wrong about this imprecision, I guess, the possible solution for this
   * big bizzare calculations, is, uses a scale-based position and size, the horrible part
   * of implementing it, is the unknown LOGIC.
   * god, do not read this.
   */
  this->min_offset = (this->min_height / 6.0f) / 2.0f;
  this->min_offset = static_cast<int32_t>(this->min_offset);
}

void ekg::service::layout::quit() {

}

float ekg::service::layout::get_min_offset() {
  return this->min_offset;
}

void ekg::service::layout::update_scale_factor() {
  this->viewport_scale = {
    static_cast<float>(ekg::ui::width), static_cast<float>(ekg::ui::height)
  };

  ekg::vec2 monitor_resolution {ekg::ui::scale};
  ekg::vec2 input_resolution {this->viewport_scale};

  if (ekg::ui::auto_scale) {
    ekg::core->p_os_platform->update_monitor_resolution();
    monitor_resolution.x = static_cast<float>(ekg::core->p_os_platform->monitor_resolution[0]);
    monitor_resolution.y = static_cast<float>(ekg::core->p_os_platform->monitor_resolution[1]);
    ekg::ui::scale = {1920.0f, 1080.0f};
    input_resolution = monitor_resolution;
  }

  /**
   * The scale is step-based, each step change the scale, e.g:
   * scale percent interval = 25
   * scale percent = 100 (scale GUI resolution == window size)
   *
   * scale percent in:
   * 0   == 0
   * 25  == 1
   * 50  == 2
   * 75  == 3
   * 100 == 4
   *
   * Then it is divided by 4 (4 is the maximum value)
   * e.g: 2/4 = 0.5f --> 3/4 = 0.75f
   **/

  float base_scale {
    ekg::ui::scale.x * ekg::ui::scale.y
  };

  float monitor_scale {
    monitor_resolution.x * monitor_resolution.y
  };

  float monitor_factor {
    monitor_scale / base_scale
  };

  float monitor_scale_percent {
    monitor_factor * 100.0f
  };
  
  float factor {
    ((input_resolution.x * input_resolution.y) / base_scale) * 100.0f
  };

  factor = (
    roundf(factor / ekg::ui::scale_interval) / (monitor_scale_percent / ekg::ui::scale_interval)
  );

  this->scale_factor = ekg_clamp(factor, 0.5f, 2.0f);
}

float ekg::service::layout::get_scale_factor() const {
  return this->scale_factor;
}