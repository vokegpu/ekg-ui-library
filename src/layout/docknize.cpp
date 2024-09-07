#include "ekg/layout/docknize.hpp"
#include "ekg/ui/display.hpp"
#include "ekg/ekg.hpp"
#include "ekg/ui/frame/ui_frame_widget.hpp"

float ekg::layout::offset {2.0f};
ekg::layout::extent_t ekg::layout::v_extent = {};
ekg::layout::extent_t ekg::layout::h_extent = {};

void ekg::layout::extentnize(
  float &extent,
  ekg::ui::abstract_widget *p_widget,
  ekg::flags flag_ok,
  ekg::flags flag_stop,
  int64_t &begin_and_count,
  ekg::axis axis
) {
  extent = 0.0f;
  if (p_widget == nullptr) {
    return;
  }

  switch (axis) {
    case ekg::axis::horizontal: {
      ekg::flags flags {};
      int32_t ids {};
      int64_t flag_ok_count {};
      int64_t it {begin_and_count};

      if (
          begin_and_count > static_cast<int64_t>(ekg::layout::h_extent.begin_index) &&
          begin_and_count < static_cast<int64_t>(ekg::layout::h_extent.end_index)
        ) {
        begin_and_count = static_cast<int64_t>(ekg::layout::h_extent.count);
        extent = ekg::layout::h_extent.extent;
        return;
      }

      ekg::layout::h_extent.begin_index = static_cast<float>(it);
      ekg::ui::abstract_widget *p_widgets {};
      std::vector<int32_t> &child_id_list {p_widget->p_data->get_child_id_list()};

      uint64_t size {child_id_list.size()};
      uint64_t latest_index {size - (!child_id_list.empty())};

      bool is_scrollbar {};
      bool is_last_index {};
      bool is_ok_flag {};

      /**
       * The last index does not check if contains a next flag,
       * so it is needed to brute-check to stop at end of index. 
       *
       * The extent data store the previous bounding indices,
       * in simply words, prevent useless iteration.
       *
       * The min offset is added for extent, because we need count
       * the offset position when split the fill width, but the
       * last extent space is not necessary, so we need to subtract.
       **/
      for (it = it; it < size; it++) {
        if ((p_widgets = ekg::core->get_fast_widget_by_id(child_id_list.at(it))) == nullptr) {
          continue;
        }

        flags = p_widgets->p_data->get_place_dock();
        is_scrollbar = p_widgets->p_data->get_type() == ekg::type::scrollbar;
        is_last_index = it == latest_index;

        if (
            (ekg_bitwise_contains(flags, flag_stop) && it != begin_and_count) || is_last_index || is_scrollbar
          ) {
          extent -= ekg::layout::offset;
          flag_ok_count += (
            (is_ok_flag = (!ekg_bitwise_contains(flags, flag_stop) && ekg_bitwise_contains(flags, flag_ok) && is_last_index))
          );

          /**
           * Basically if the container/frame mother ends with any non flag ok (ekg::dock::fill)
           * it MUST add the width size to extend.
           * But the point is the scrollbar, he is not docknized here, then just bypass with `!is_scrollbar`.
           *
           * :blush:
           **/
          extent += (
            (p_widgets->dimension.w + ekg::layout::offset) * (is_last_index && !ekg_bitwise_contains(flags, flag_ok) && !is_scrollbar)
          );

          ekg::layout::h_extent.end_index = it + is_last_index;
          ekg::layout::h_extent.extent = extent;
          ekg::layout::h_extent.count = flag_ok_count + (flag_ok_count == 0);

          break;
        }

        if (ekg_bitwise_contains(flags, flag_ok)) {
          flag_ok_count++;
          continue;
        }

        extent += p_widgets->dimension.w + ekg::layout::offset;
      }

      begin_and_count = flag_ok_count + (flag_ok_count == 0);
      break;
    }

    case ekg::axis::vertical: {
      break;
    }
  }
}

void ekg::layout::docknize(ekg::ui::abstract_widget *p_widget_parent) {
  if (p_widget_parent == nullptr || p_widget_parent->p_data == nullptr) {
    return;
  }

  ekg::type type {p_widget_parent->p_data->get_type()};
  bool is_group {type == ekg::type::frame};
  ekg::rect &abs_parent_rect {p_widget_parent->get_abs_rect()};

  if (!is_group || abs_parent_rect.w == 0 || abs_parent_rect.h == 0) {
    return;
  }

  if (p_widget_parent->is_targeting_absolute_parent) {
    p_widget_parent->is_targeting_absolute_parent = false;

    if (p_widget_parent->p_abs_parent_widget) {
      ekg::layout::docknize(p_widget_parent->p_abs_parent_widget);
    } else {
      ekg::layout::docknize(p_widget_parent);
    }

    return;
  }

  float group_top_offset {ekg::layout::offset};
  float initial_offset {};
 
  bool has_scroll_embedded {};
  bool is_vertical_enabled {};

  ekg::rect group_rect {p_widget_parent->dimension};

  switch (type) {
    case ekg::type::frame: {
      ekg::service::theme_scheme_t &theme_scheme {ekg::current_theme_scheme()};
      ekg::ui::frame_widget *p_frame {(ekg::ui::frame_widget *) p_widget_parent};

      initial_offset = static_cast<float>(theme_scheme.scrollbar_pixel_thickness);
      has_scroll_embedded = p_frame->p_scroll_embedded != nullptr;

      if (has_scroll_embedded) {
        p_frame->p_scroll_embedded->check_axis_states();
        is_vertical_enabled = p_frame->p_scroll_embedded->is_vertical_enabled;
        initial_offset *= static_cast<float>(!theme_scheme.symmetric_layout);

        group_rect.w -= initial_offset * static_cast<float>(is_vertical_enabled);
        group_rect.h -= initial_offset * static_cast<float>(p_frame->p_scroll_embedded->is_horizontal_enabled);
      }

      initial_offset = static_cast<float>(theme_scheme.scrollbar_pixel_thickness) * static_cast<float>(theme_scheme.symmetric_layout);
      break;
    }

    default: {
      break;
    }
  }

  float total_offset {(initial_offset + ekg::layout::offset) * 2.0f};

  group_rect.w -= total_offset;
  group_rect.h -= total_offset;

  ekg::ui::abstract_widget *p_widgets {};
  ekg::flags flags {};
  int64_t it {};
  float dimensional_extent {};
  int64_t count {};

  ekg::rect widget_rect {};
  ekg::rect bottom_rect {};
  ekg::rect top_rect {ekg::layout::offset + initial_offset, ekg::layout::offset + initial_offset, 0.0f, 0.0f};
  ekg::rect prev_widget_layout {};

  ekg::flags prev_flags {};
  bool should_reload_widget {};
  bool skip_widget {};
  float max_previous_height {};

  ekg::layout::h_extent = {};
  ekg::layout::extent_t h_extent_backup {};
  ekg::layout::extent_t v_extent_backup {};

  for (int32_t &ids: p_widget_parent->p_data->get_child_id_list()) {
    if (ids == 0 || (p_widgets = ekg::core->get_fast_widget_by_id(ids)) == nullptr) {
      continue;
    }

    ekg::rect &layout {p_widgets->dimension};
    flags = p_widgets->p_data->get_place_dock();

    // @TODO Prevent useless scrolling reload.
    p_widgets->on_reload();
    type = p_widgets->p_data->get_type();

    if (type == ekg::type::scrollbar) {
      it++;
      continue;
    }

    if (ekg_bitwise_contains(flags, ekg::dock::fill)) {
      if (ekg_bitwise_contains(flags, ekg::dock::next)) {
        top_rect.h += max_previous_height + ekg::layout::offset;
        top_rect.w = 0.0f;
        max_previous_height = 0.0f;
      }

      layout.x = top_rect.x + top_rect.w;
      layout.y = top_rect.y + top_rect.h;

      count = it;
      ekg::layout::extentnize(
        dimensional_extent,
        p_widget_parent,
        ekg::dock::fill,
        ekg::dock::next,
        count,
        ekg::axis::horizontal
      );

      dimensional_extent = ekg_min(
        ekg_layout_get_dimensional_extent(
          group_rect.w,
          dimensional_extent,
          ekg::layout::offset,
          count
        ),
        p_widgets->min_size.x
      );

      top_rect.w += dimensional_extent + ekg::layout::offset;
      layout.w = dimensional_extent;
      should_reload_widget = true;
    } else if (ekg_bitwise_contains(flags, ekg::dock::next)) {
      top_rect.h += max_previous_height + ekg::layout::offset;
      top_rect.w = 0.0f;

      layout.x = top_rect.x;
      layout.y = top_rect.y + top_rect.h;
      top_rect.w += layout.w + ekg::layout::offset;

      count = it;
      ekg::layout::extentnize(
        dimensional_extent,
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
      top_rect.w += layout.w + ekg::layout::offset;

      count = it;
      ekg::layout::extentnize(
        dimensional_extent,
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

    h_extent_backup = ekg::layout::h_extent;
    if (p_widgets->p_data->has_children()) {
      ekg::layout::docknize(p_widgets);
    }

    ekg::layout::h_extent = h_extent_backup;
    prev_widget_layout = layout;
    prev_flags = flags;
    it++;
  }

  if (has_scroll_embedded && !is_vertical_enabled && type == ekg::type::frame) {
    ekg::ui::frame_widget *p_frame {(ekg::ui::frame_widget *) p_widget_parent};
    has_scroll_embedded = p_frame->p_scroll_embedded != nullptr;

    if (has_scroll_embedded && !is_vertical_enabled && p_frame->p_scroll_embedded->is_vertical_enabled) {
      ekg::layout::docknize(p_widget_parent);
    }
  }
}

void ekg::layout::mask::extentnize(
  float &extent,
  ekg::flags flag_ok,
  ekg::flags flag_stop,
  int64_t &begin_and_count,
  ekg::axis axis
) {
  extent = 0.0f;
  switch (axis) {
    case ekg::axis::horizontal: {
      ekg::flags flags {};
      int32_t ids {};
      int64_t flag_ok_count {};
      int64_t it {begin_and_count};

      if (
          begin_and_count > this->h_extent.begin_index &&
          begin_and_count < this->h_extent.end_index
        ) {
        begin_and_count = this->h_extent.count;
        extent = this->h_extent.extent;
        return;
      }

      ekg::layout::h_extent.begin_index = static_cast<float>(it);
      uint64_t size {this->dock_rect_list.size()};
      uint64_t latest_index {size - (!this->dock_rect_list.empty())};
      int32_t should_skip_next {};

      bool is_last_index {};
      bool is_ok_flag {};
 
      extent += this->offset.x;

      /**
       * The last index does not check if contains a next flag,
       * so it is needed to brute-check to stop at end of index. 
       *
       * The extent data store the previous bounding indices,
       * in simply words, prevent useless iteration.
       *
       * The min offset is added for extent, because we need count
       * the offset position when split the fill width, but the
       * last extent space is not necessary, so we need to subtract.
       **/
      for (it = it; it < size; it++) {
        ekg::layout::mask::rect &dock_rect {this->dock_rect_list.at(it)};
        if (dock_rect.p_rect == nullptr) {
          continue;
        }

        is_last_index = it == latest_index;

        if (
            (ekg_bitwise_contains(dock_rect.flags, flag_stop) && it != begin_and_count) || is_last_index
          ) {
          extent -= this->offset.x;
          flag_ok_count += (
            (is_ok_flag = (!ekg_bitwise_contains(dock_rect.flags, flag_stop) && (ekg_bitwise_contains(dock_rect.flags, flag_ok)) && is_last_index))
          );

          /**
           * Basically if the container/frame mother ends with any non flag ok (ekg::dock::fill)
           * it MUST add the width size to extend.
           *
           * :blush:
           **/
          extent += ( 
            (dock_rect.p_rect->w + this->offset.x) * (is_last_index && (!ekg_bitwise_contains(dock_rect.flags, flag_ok) && should_skip_next == 0))
          );

          this->h_extent.end_index = it + is_last_index;
          this->h_extent.extent = extent;
          this->h_extent.count = flag_ok_count + (flag_ok_count == 0);
          break;
        }

        should_skip_next += static_cast<bool>(ekg_bitwise_contains(dock_rect.flags, ekg::dock::bind));

        if (should_skip_next > 0) {
          should_skip_next = (should_skip_next + 1) * (should_skip_next < 2);
          flag_ok_count += static_cast<bool>(ekg_bitwise_contains(dock_rect.flags, flag_ok));
          continue;
        }

        if (ekg_bitwise_contains(dock_rect.flags, flag_ok)) {
          flag_ok_count++;
          continue;
        }

        extent += dock_rect.p_rect->w + this->offset.x;
      }

      begin_and_count = flag_ok_count + (flag_ok_count == 0);
      break;
    }

    case ekg::axis::vertical: {
      break;
    }
  }
}

void ekg::layout::mask::preset(const ekg::vec3 &mask_offset, ekg::axis mask_axis, float initial_respective_size) {
  this->axis = mask_axis;
  this->offset = mask_offset;
  this->respective_all = initial_respective_size;
  this->v_extent = {};
  this->h_extent = {};
}

void ekg::layout::mask::insert(const ekg::layout::mask::rect &dock_rect) {
  if (ekg_bitwise_contains(dock_rect.flags, ekg::dock::none)) {
    dock_rect.p_rect->w = 0.0f;
    dock_rect.p_rect->h = 0.0f;
    return;
  }

  this->dock_rect_list.push_back(dock_rect);
}

void ekg::layout::mask::docknize() {
  float left_or_right {};
  float centered_dimension {this->offset.z / 2};
  float opposite {};
  float uniform {};
  float clamped_offset {};

  int64_t count {};
  float dimensional_extent {};
  float rect_height {};
  float rect_width {};
  float dimension_width {};
  float dimension_height {};
  float dimension_bind {};

  bool is_left {};
  bool is_right {};
  bool is_center {};
  bool is_top {};
  bool is_bottom {};
  bool is_not_bind {};
  bool is_bind_dimension_not_zero {};

  /**
   * All dock rects within left corner (ekg::dock::left)
   * starts from left to right.
   **/
  ekg::rect left_corner {};

  /**
   * All dock rects within right corner (ekg::dock::right)
   * starts from right (dimension width based) to left.
   **/
  ekg::rect right_corner {};

  /**
   * All dock rects within center left corner (ekg::dock::right & ekg::dock::center)
   * starts from right (dimension width divided by 2 based) to left.
   **/
  ekg::rect center_left_corner {};

  /**
   * All dock rects within center right corner (ekg::dock::right & ekg::dock::center)
   * starts from left (dimension width divided by 2 based) to right.
   **/
  ekg::rect center_right_corner {};

  switch (this->axis) {
  case ekg::axis::horizontal:
    if (this->dock_rect_list.empty()) {
      this->mask.w = this->respective_all;
      this->mask.h = this->offset.z;
      return;
    }

    this->mask.w = this->offset.x;
    this->mask.h = this->offset.z;

    dimension_width = this->respective_all;
    dimension_height = this->offset.z;

    left_corner.w += this->offset.x;
    right_corner.w += this->offset.x;

    center_left_corner.x = dimension_width / 2.0f;
    center_left_corner.w = this->offset.x;

    center_right_corner.x = dimension_width / 2.0f;
    center_right_corner.w = this->offset.x;

    for (uint64_t it {}; it < this->dock_rect_list.size(); it++) {
      ekg::layout::mask::rect &dock_rect {this->dock_rect_list.at(it)};
      if (dock_rect.p_rect == nullptr) {
        continue;
      }

      is_left = ekg_bitwise_contains(dock_rect.flags, ekg::dock::left);
      is_right = ekg_bitwise_contains(dock_rect.flags, ekg::dock::right);
      is_center = ekg_bitwise_contains(dock_rect.flags, ekg::dock::center);
      is_bottom = ekg_bitwise_contains(dock_rect.flags, ekg::dock::bottom);
      is_top = ekg_bitwise_contains(dock_rect.flags, ekg::dock::top);
      is_not_bind = !ekg_bitwise_contains(dock_rect.flags, ekg::dock::bind);
      is_bind_dimension_not_zero = (dimension_bind > 0.0f);

      rect_width = dock_rect.p_rect->w;
      rect_height = dock_rect.p_rect->h;

      if (ekg_bitwise_contains(dock_rect.flags, ekg::dock::fill)) {
        count = 0;
        this->extentnize(
          dimensional_extent,
          ekg::dock::fill,
          ekg::dock::none,
          count,
          ekg::axis::horizontal
        );

        rect_width = ekg_min(
          ekg_layout_get_dimensional_extent(
            this->respective_all,
            dimensional_extent,
            this->offset.x,
            count
          ),
          1.0f
        );
      }

      if (is_left) {
        dock_rect.p_rect->x = (is_bind_dimension_not_zero * this->offset.x) + left_corner.w;
        dock_rect.p_rect->w = rect_width;

        if (is_not_bind) {
          dimension_bind += (this->offset.x * is_bind_dimension_not_zero) + ((dock_rect.p_rect->w + this->offset.x) * !is_bind_dimension_not_zero);
          left_corner.w += dimension_bind;
          this->mask.w += dimension_bind;
        }
      } else if (is_right) {
        dock_rect.p_rect->w = rect_width;
        dock_rect.p_rect->x = (is_bind_dimension_not_zero * this->offset.x) + dimension_width - right_corner.w - dock_rect.p_rect->w;

        if (is_not_bind) {
          dimension_bind += (this->offset.x * is_bind_dimension_not_zero) + ((dock_rect.p_rect->w + this->offset.x) * !is_bind_dimension_not_zero);
          right_corner.w += dimension_bind;
          this->mask.w += dimension_bind;
        }
      } else if (is_left) {
        dock_rect.p_rect->w = rect_width;
        dock_rect.p_rect->x = (is_bind_dimension_not_zero * this->offset.x) + center_left_corner.x - center_left_corner.w - dock_rect.p_rect->w;

        if (is_not_bind) {
          dimension_bind += (this->offset.x * is_bind_dimension_not_zero) + ((dock_rect.p_rect->w + this->offset.x) * !is_bind_dimension_not_zero);
          center_left_corner.w += dimension_bind;
          this->mask.w += dimension_bind;
        }
      } else if (is_right) {
        dock_rect.p_rect->x = (is_bind_dimension_not_zero * this->offset.x) + center_right_corner.x + center_right_corner.w;
        dock_rect.p_rect->w = rect_width;

        if (is_not_bind) {
          dimension_bind += (this->offset.x * is_bind_dimension_not_zero) + ((dock_rect.p_rect->w + this->offset.x) * !is_bind_dimension_not_zero);
          center_right_corner.w += dimension_bind;
          this->mask.w += dimension_bind;
        }
      } else if (dock_rect.flags == ekg::dock::center) {
        dock_rect.p_rect->w = rect_width;
        dock_rect.p_rect->x = (dimension_width / 2.0f) - (dock_rect.p_rect->w / 2.0f);
      }

      if (is_top | is_bottom) {
        dock_rect.p_rect->y = (
          is_top ? (this->offset.y)  : (dimension_height - rect_height - this->offset.y)
        );
      } else if (is_center) {
        dock_rect.p_rect->y = (
          (dimension_height / 2.0f) - (rect_height / 2.0f)
        );
      }

      if (!is_not_bind && dock_rect.p_rect->w > dimension_bind) {
        dimension_bind = dock_rect.p_rect->w;
      } else if (is_not_bind) {
        dimension_bind = 0.0f;
      }

      this->mask.h = dimension_height;
    }

    this->mask.w = ekg_min(
      this->respective_all,
      this->mask.w
    );
    break;
  case ekg::axis::vertical:
    if (this->dock_rect_list.empty()) {
      this->mask.w = this->offset.z;
      this->mask.h = this->respective_all;
      return;
    }

    this->mask.w = this->offset.z;
    this->mask.h = this->offset.y;
    break;
  }

  this->dock_rect_list.clear();
}

ekg::rect &ekg::layout::mask::get_rect() {
  return this->mask;
}