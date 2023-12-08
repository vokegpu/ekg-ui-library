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

#include "ekg/ui/frame/ui_frame.hpp"
#include "ekg/util/gui.hpp"

ekg::ui::frame *ekg::ui::frame::set_place(uint16_t _dock) {
  if (this->dock_flags != _dock) {
    this->dock_flags = _dock;

    ekg::reload(this->id);
    ekg::synclayout(this->id);
  }

  return this;
}

ekg::ui::frame *ekg::ui::frame::set_scale_factor(float x, float y) {
  this->scale_factor.x = x;
  this->scale_factor.y = y;

  return this;
}

ekg::vec2 ekg::ui::frame::get_scale_factor() {
  return this->scale_factor;
}

ekg::ui::frame *ekg::ui::frame::set_drag(uint16_t dock) {
  this->dock_drag = dock;
  return this;
}

uint16_t ekg::ui::frame::get_drag_dock() {
  return this->dock_drag;
}

ekg::ui::frame *ekg::ui::frame::set_resize(uint16_t dock) {
  this->dock_resize = dock;
  return this;
}

uint16_t ekg::ui::frame::get_resize_dock() {
  return this->dock_resize;
}

ekg::ui::frame *ekg::ui::frame::set_pos_initial(float x, float y) {
  this->rect_initial.x = x;
  this->rect_initial.y = y;
  return this;
}

ekg::vec2 ekg::ui::frame::get_pos_initial() {
  return {this->rect_initial.x, this->rect_initial.y};
}

ekg::ui::frame *ekg::ui::frame::set_size_initial(float w, float h) {
  this->rect_initial.w = w;
  this->rect_initial.h = h;
  return this;
}

ekg::vec2 ekg::ui::frame::get_size_initial() {
  return {this->rect_initial.w, this->rect_initial.h};
}

ekg::ui::frame *ekg::ui::frame::set_size(float w, float h) {
  if (this->sync_ui.w != w || this->sync_ui.h != h) {
    this->sync_ui.w = w;
    this->sync_ui.h = h;

    ekg::bitwise::add(this->sync_flags, (uint16_t) ekg::ui_sync::dimension);
    ekg::reload(this->id);
    ekg::synclayout(this->id);
    ekg::dispatch(ekg::env::redraw);
  }

  return this;
}

ekg::vec2 ekg::ui::frame::get_size() {
  return {this->rect_widget.w, rect_widget.h};
}

ekg::ui::frame *ekg::ui::frame::set_pos(float x, float y) {
  if (this->sync_ui.x != x || this->sync_ui.y != y) {
    this->sync_ui.x = x;
    this->sync_ui.y = y;

    ekg::bitwise::add(this->sync_flags, (uint16_t) ekg::ui_sync::dimension);
    ekg::reload(this->id);
    ekg::dispatch(ekg::env::redraw);
  }

  return this;
}

ekg::vec2 ekg::ui::frame::get_pos() {
  return {this->rect_widget.x, this->rect_widget.y};
}

ekg::ui::frame *ekg::ui::frame::set_initial_width(float w) {
  this->rect_initial.w = w;
  return this;
}

float ekg::ui::frame::get_initial_width() {
  return this->rect_initial.w;
}

ekg::ui::frame *ekg::ui::frame::set_initial_height(float h) {
  this->rect_initial.h = h;
  return this;
}

float ekg::ui::frame::get_initial_height() {
  return this->rect_initial.h;
}

ekg::ui::frame *ekg::ui::frame::set_width(float w) {
  if (this->sync_ui.w != w) {
    this->sync_ui.w = w;

    ekg::bitwise::add(this->sync_flags, (uint16_t) ekg::ui_sync::dimension);
    ekg::reload(this->id);
    ekg::synclayout(this->id);
    ekg::dispatch(ekg::env::redraw);
  }

  return this;
}

float ekg::ui::frame::get_width() {
  return this->rect_widget.w;
}

ekg::ui::frame *ekg::ui::frame::set_height(float h) {
  if (this->sync_ui.h != h) {
    this->sync_ui.h = h;

    ekg::bitwise::add(this->sync_flags, (uint16_t) ekg::ui_sync::dimension);
    ekg::reload(this->id);
    ekg::synclayout(this->id);
    ekg::dispatch(ekg::env::redraw);
  }

  return this;
}

float ekg::ui::frame::get_height() {
  return this->rect_widget.h;
}