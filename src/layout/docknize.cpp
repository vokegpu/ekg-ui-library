#include "ekg/layout/docknize.hpp"
#include "ekg/ui/display.hpp"

ekg::rect ekg::layout::extent::h {};
ekg::rect ekg::layout::extent::v {};

/*
void ekg::layout::docknize(ekg::ui::abstract_widget *p_parent_widget) {
  if (p_parent_widget == nullptr || p_parent_widget->p_data == nullptr) {
    return;
  }

  ekg::type type {p_parent_widget->p_data->get_type()};
  bool is_group {type == ekg::type::frame};
  ekg::rect &abs_parent_rect {p_parent_widget->get_abs_rect()};

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


  this->extent_data[0] = 0.0f;
  this->extent_data[1] = 0.0f;
  this->extent_data[2] = 0.0f;

  bool should_reload_widget {};
  bool skip_widget {};

  float max_previous_height {};
  float extent_data_backup[4] {};


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

  if (has_scroll_embedded && !is_vertical_enabled && type == ekg::type::fram) {
    ekg::ui::frame_widget p_frame {(ekg::ui::frame_widget *) p_widget_parent};
    has_scroll_embedded = p_frame->p_scroll_embedded != nullptr;

    if (has_scroll_embedded && !is_vertical_enabled && p_frame->p_scroll_embedded->is_vertical_enabled) {
      this->process_scaled(p_widget_parent);
    }
  }
}

*/