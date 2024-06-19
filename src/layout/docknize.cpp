#include "ekg/layout/docknize.hpp"
#include "ekg/ui/display.hpp"
#include "ekg/ekg.hpp"
#include "ekg/ui/frame/ui_frame_widget.hpp"

float ekg::layout::offset {2.0f};
ekg::rect ekg::layout::extent::h {};
ekg::rect ekg::layout::extent::v {};

void ekg::layout::extentnize(
  float &extent,
  ekg::ui::abstract_widget *p_widget,
  uint16_t flag_ok,
  uint16_t flag_stop,
  int64_t &begin_and_count,
  ekg::axis axis
) {
  extent = 0.0f;
  if (p_widget == nullptr) {
    return;
  }

  switch (axis) {
    case ekg::axis::horizontal: {
      uint16_t flags {};
      int32_t ids {};
      int64_t n {};
      int64_t it {begin_and_count};

      if (
          begin_and_count > static_cast<int64_t>(ekg::layout::extent::h.x) &&
          begin_and_count < static_cast<int64_t>(ekg::layout::extent::h.y)
        ) {
        begin_and_count = static_cast<int64_t>(ekg::layout::extent::h.h);
        extent = ekg::layout::extent::h.w;
        return;
      }

      ekg::layout::extent::h.x = static_cast<float>(it);
      ekg::ui::abstract_widget *p_widgets {};
      std::vector<int32_t> &child_id_list {p_widget->p_data->get_child_id_list()};
      uint64_t size {child_id_list.size()};
      bool is_last_index {};

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
        is_last_index = it >= size - 1 || p_widgets->p_data->get_type() == ekg::type::scrollbar;

        if (
            (ekg_bitwise_contains(flags, flag_stop) && it != begin_and_count) || is_last_index
          ) {
          extent -= ekg::layout::offset;
          n += (!ekg_bitwise_contains(flags, flag_stop) && ekg_bitwise_contains(flags, flag_ok) && is_last_index);

          ekg::layout::extent::h.y = static_cast<float>(it + is_last_index);
          ekg::layout::extent::h.w = extent;
          ekg::layout::extent::h.h = static_cast<float>(n == 0 ? 1 : n);
          break;
        }

        if (ekg_bitwise_contains(flags, flag_ok)) {
          n++;
          continue;
        }

        extent += p_widgets->dimension.w + ekg::layout::offset;
      }

      begin_and_count = n == 0 ? 1 : n;
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
      ekg::service::theme &theme {ekg::theme()};
      ekg::ui::frame_widget *p_frame {(ekg::ui::frame_widget *) p_widget_parent};

      initial_offset = static_cast<float>(theme.scrollbar_pixel_thickness);
      has_scroll_embedded = p_frame->p_scroll_embedded != nullptr;

      if (has_scroll_embedded) {
        p_frame->p_scroll_embedded->check_axis_states();
        is_vertical_enabled = p_frame->p_scroll_embedded->is_vertical_enabled;
        initial_offset *= static_cast<float>(!theme.symmetric_layout);

        group_rect.w -= initial_offset * static_cast<float>(is_vertical_enabled);
        group_rect.h -= initial_offset * static_cast<float>(p_frame->p_scroll_embedded->is_horizontal_enabled);
      }

      initial_offset = static_cast<float>(theme.scrollbar_pixel_thickness) * static_cast<float>(theme.symmetric_layout);
      break;
    }

    default: {
      break;
    }
  }

  group_rect.w -= (initial_offset + ekg::layout::offset) * 2.0f;
  group_rect.h -= (initial_offset + ekg::layout::offset) * 2.0f;

  ekg::ui::abstract_widget *p_widgets {};
  uint16_t flags {};
  int64_t it {};
  float dimensional_extent {};
  int64_t count {};

  ekg::rect widget_rect {};
  ekg::rect bottom_rect {};
  ekg::rect top_rect {ekg::layout::offset + initial_offset, ekg::layout::offset + initial_offset, 0.0f, 0.0f};
  ekg::rect prev_widget_layout {};

  uint16_t prev_flags {};
  bool should_reload_widget {};
  bool skip_widget {};
  float max_previous_height {};
  
  ekg::layout::extent::h = {};
  ekg::rect h_extent_backup {};
  ekg::rect v_extent_backup {};

  for (int32_t &ids: p_widget_parent->p_data->get_child_id_list()) {
    if (ids == 0 || (p_widgets = ekg::core->get_fast_widget_by_id(ids)) == nullptr) {
      continue;
    }

    ekg::rect &layout {p_widgets->dimension};
    flags = p_widgets->p_data->get_place_dock();

    // @TODO Prevent useless scrolling reload.
    p_widgets->on_reload();

    if (p_widgets->p_data->get_type() == ekg::type::scrollbar) {
      it++;
      continue;
    }

    if (ekg_bitwise_contains(flags, ekg::dock::fill) && ekg_bitwise_contains(flags, ekg::dock::next)) {
      top_rect.h += max_previous_height + ekg::layout::offset;
      top_rect.w = 0.0f;

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
        ((group_rect.w - dimensional_extent) - (count * ekg::layout::offset)) / static_cast<float>(count),
        p_widgets->min_size.x
      );

      top_rect.w += dimensional_extent + ekg::layout::offset;
      layout.w = dimensional_extent;
      should_reload_widget = true;
      max_previous_height = 0.0f;
    } else if (ekg_bitwise_contains(flags, ekg::dock::fill)) {
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
        ((group_rect.w - dimensional_extent) - (count * ekg::layout::offset)) / static_cast<float>(count),
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

    h_extent_backup = ekg::layout::extent::h;

    if (p_widgets->p_data->has_children()) {
      ekg::layout::docknize(p_widgets);
    }

    ekg::layout::extent::h = h_extent_backup;

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