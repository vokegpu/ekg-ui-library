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

#include "ekg/ui/popup/ui_popup.hpp"
#include "ekg/util/gui.hpp"
#include "ekg/util/text.hpp"

int64_t ekg::ui::popup::contains(std::string_view item_name) {
  for (uint32_t it {}; it < this->item_list.size(); it++) {
    if (this->item_list.at(it).name == item_name) {
      return it;
    }
  }

  return -1;
}

ekg::ui::popup *ekg::ui::popup::insert(const std::vector<std::string> &item_name_list) {
  for (const std::string &item_name: item_name_list) {
    this->insert(item_name);
  }

  return this;
}

ekg::ui::popup *ekg::ui::popup::insert(std::string_view item_name) {
  uint16_t attribute_flags {};
  uint8_t start_index {ekg::check_attribute_flags(item_name, attribute_flags)};

  if (start_index) {
    item_name = item_name.substr(start_index, item_name.size());
  }

  if (item_name.empty() || this->contains(item_name) != -1) {
    return this;
  }

  ekg::ui::item item {};
  item.name = item_name;
  item.attributes = attribute_flags;
  this->item_list.push_back(item);
  return this;
}

ekg::ui::popup *ekg::ui::popup::link(std::string_view item_name, ekg::ui::popup *popup_linked) {
  int64_t index {this->contains(item_name)};

  if (index == -1) {
    this->insert(item_name);
  }

  auto &item {this->item_list.at(index)};
  item.linked_id = 0;

  if (popup_linked) {
    popup_linked->set_visible(false);
    item.linked_id = popup_linked->get_id();
    this->add_child(item.linked_id);
  }

  return this;
}

ekg::ui::popup *ekg::ui::popup::erase(std::string_view item_name) {
  std::vector<ekg::ui::item> new_list {};
  for (ekg::ui::item &item: this->item_list) {
    if (item.name == item_name) {
      this->remove_child(item.linked_id);
      continue;
    }

    new_list.push_back(item);
  }

  this->item_list.clear();
  this->item_list.insert(this->item_list.end(), new_list.begin(), new_list.end());

  return this;
}

ekg::ui::item &ekg::ui::popup::get(uint64_t index) {
  return this->item_list.at(index);
}

std::vector<ekg::ui::item> &ekg::ui::popup::get_item_list() {
  return this->item_list;
}

ekg::ui::popup *ekg::ui::popup::set_text_align(uint16_t flags) {
  if (this->dock_flags != flags) {
    this->dock_flags = flags;
    ekg::reload(this->id);
  }

  return this;
}

uint16_t ekg::ui::popup::get_text_align() {
  return this->dock_flags;
}

ekg::ui::popup *ekg::ui::popup::set_width(float width) {
  if (this->sync_ui.w != width) {
    this->sync_ui.w = width;

    ekg_bitwise_add(this->sync_flags, static_cast<uint16_t>(ekg::ui_sync::dimension));
    ekg::reload(this->id);
    ekg::synclayout(this->parent_id);
  }

  return this;
}

float ekg::ui::popup::get_width() {
  return this->rect_widget.w;
}

ekg::ui::popup *ekg::ui::popup::set_scaled_height(int32_t scaled_height_factor) {
  if (this->scaled_height != scaled_height_factor) {
    this->scaled_height = scaled_height_factor;

    ekg::reload(this->id);
    ekg::synclayout(this->parent_id);
  }

  return this;
}

int32_t ekg::ui::popup::get_scaled_height() {
  return this->scaled_height;
}

float ekg::ui::popup::get_height() {
  return this->rect_widget.h;
}

ekg::ui::popup *ekg::ui::popup::set_font_size(ekg::font f_size) {
  if (this->font_size != f_size) {
    this->font_size = f_size;

    ekg::reload(this->id);
    ekg::synclayout(this->parent_id);
  }

  return this;
}

ekg::font ekg::ui::popup::get_font_size() {
  return this->font_size;
}

ekg::ui::popup *ekg::ui::popup::set_pos(float x, float y) {
  if (this->sync_ui.x != x || this->sync_ui.y != y) {
    this->sync_ui.x = x;
    this->sync_ui.y = y;

    ekg_bitwise_add(this->sync_flags, static_cast<uint16_t>(ekg::ui_sync::dimension));
    ekg::reload(this->id);
  }

  return this;
}

ekg::vec2 ekg::ui::popup::get_pos() {
  return {this->rect_widget.x, this->rect_widget.y};
}