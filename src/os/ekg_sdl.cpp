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

#include <cinttypes>
#include <iostream>
#include <fstream>

#include "ekg/ekg.hpp"
#include "ekg/os/ekg_sdl.hpp"

ekg::os::sdl::sdl(SDL_Window *p_sdl_win) {
  this->p_sdl_win = p_sdl_win;
}

void ekg::os::sdl::init() {
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::arrow)] = SDL_CreateSystemCursor(
      SDL_SYSTEM_CURSOR_ARROW);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::ibeam)] = SDL_CreateSystemCursor(
      SDL_SYSTEM_CURSOR_IBEAM);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::wait)] = SDL_CreateSystemCursor(
      SDL_SYSTEM_CURSOR_WAIT);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::crosshair)] = SDL_CreateSystemCursor(
      SDL_SYSTEM_CURSOR_CROSSHAIR);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::wait_arrow)] = SDL_CreateSystemCursor(
      SDL_SYSTEM_CURSOR_WAITARROW);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::size_nwse)] = SDL_CreateSystemCursor(
      SDL_SYSTEM_CURSOR_SIZENWSE);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::size_nesw)] = SDL_CreateSystemCursor(
      SDL_SYSTEM_CURSOR_SIZENESW);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::size_we)] = SDL_CreateSystemCursor(
      SDL_SYSTEM_CURSOR_SIZEWE);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::size_ns)] = SDL_CreateSystemCursor(
      SDL_SYSTEM_CURSOR_SIZENS);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::size_all)] = SDL_CreateSystemCursor(
      SDL_SYSTEM_CURSOR_SIZEALL);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::no)] = SDL_CreateSystemCursor(
      SDL_SYSTEM_CURSOR_NO);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::hand)] = SDL_CreateSystemCursor(
      SDL_SYSTEM_CURSOR_HAND);

  this->update_cursor(ekg::system_cursor::arrow);
  ekg::cursor = ekg::system_cursor::arrow;
}

void ekg::os::sdl::update_cursor(ekg::system_cursor sys_cursor) {
  SDL_SetCursor(this->loaded_system_cursor_list[static_cast<uint32_t>(sys_cursor)]);
}

void ekg::os::sdl::update_monitor_resolution() {
  SDL_DisplayMode sdl_display_mode {};
  SDL_GetDisplayMode(0, 0, &sdl_display_mode);

  this->monitor_resolution[0] = static_cast<int32_t>(sdl_display_mode.w);
  this->monitor_resolution[1] = static_cast<int32_t>(sdl_display_mode.h)
}

void ekg::os::sdl::get_key_name(int32_t key, std::string_view &name) {
  name = SDL_GetKeyName(key);
}

void ekg::os::sdl_poll_event(SDL_Event &sdl_event) {
  ekg::event::serialized serialized {};

  switch (sdl_event.type) {
  case SDL_WINDOWEVENT:
    switch (sdl_event.window.event) {
      case SDL_WINDOWEVENT_SIZE_CHANGED:
        ekg::display::width = sdl_event.window.data1;
        ekg::display::height = sdl_event.window.data2;
        
        ekg::core->p_gpu_api->update_viewport(ekg::display::width, ekg::display::height);
        ekg::core->update_size_changed();

        break;
    }

    break;

  case SDL_KEYDOWN:
    io_event_serialized.is_key_down = true;
    io_event_serialized.key = static_cast<int32_t>(sdl_event.key.keysym.sym);
    break;

  case SDL_KEYUP:
    io_event_serialized.is_key_up = true;
    io_event_serialized.key = static_cast<int32_t>(sdl_event.key.keysym.sym);
    break;

  case SDL_TEXTINPUT:
    io_event_serialized.is_text_input = true;
    io_event_serialized.text_input = sdl_event.text;
    break;

  case SDL_MOUSEBUTTONUP:
    io_event_serialized.is_mouse_button_up = true;
    io_event_serialized.mouse_button = sdl_event.button.button;
    break;

  case SDL_MOUSEBUTTONDOWN:
    io_event_serialized.is_mouse_button_down = true;
    io_event_serialized.mouse_button = sdl_event.button.button;
    break;

  case SDL_MOUSEWHEEL:
    io_event_serialized.is_mouse_wheel = true;
    io_event_serialized.mouse_wheel_x = sdl_event.wheel.x;
    io_event_serialized.mouse_wheel_y = sdl_event.wheel.y;
    io_event_serialized.mouse_wheel_precise_x = sdl_event.wheel.preciseX;
    io_event_serialized.mouse_wheel_precise_y = sdl_event.wheel.preciseY;
    break;

  case SDL_FINGERUP:
    io_event_serialized.is_finger_up = true;
    io_event_serialized.finger_x = sdl_event.tfinger.x;
    io_event_serialized.finger_y = sdl_event.tfinger.y;        
    break;

  case SDL_FINGERDOWN:
    io_event_serialized.is_finger_down = true;
    io_event_serialized.finger_x = sdl_event.tfinger.x;
    io_event_serialized.finger_y = sdl_event.tfinger.y;   
    break;

  case SDL_FINGERMOTION:
    io_event_serialized.is_finger_motion = true;
    io_event_serialized.finger_x = sdl_event.tfinger.x;
    io_event_serialized.finger_y = sdl_event.tfinger.y;   
    break;
  }
}