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

void ekg::os::sdl::set_clipboard_text(const char *p_text) {
  SDL_SetClipboardText(p_text);
}

bool ekg::os::sdl::has_clipboard_text() {
  return SDL_HasClipboardText();
}

const char *ekg::os::sdl::get_clipboard_text() {
  return SDL_GetClipboardText();
}

void ekg::os::sdl::init() {
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::arrow)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::ibeam)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::wait)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::crosshair)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::wait_arrow)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAITARROW);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::size_nwse)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::size_nesw)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::size_we)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::size_ns)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::size_all)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::no)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);
  this->loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::hand)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

  this->update_cursor(ekg::system_cursor::arrow);
  ekg::cursor = ekg::system_cursor::arrow;

  SDL_GetWindowSize(this->p_sdl_win, &ekg::ui::width, &ekg::ui::height);

  SDL_version sdl_version {};
  SDL_GetVersion(&sdl_version);

  ekg::log() << "SDL version: "
             << static_cast<int32_t>(sdl_version.major)
             << '.'
             << static_cast<int32_t>(sdl_version.minor)
             << '.'
             << static_cast<int32_t>(sdl_version.patch);
}

void ekg::os::sdl::quit() {
  
}

void ekg::os::sdl::update_cursor(ekg::system_cursor sys_cursor) {
  SDL_SetCursor(this->loaded_system_cursor_list[static_cast<uint32_t>(sys_cursor)]);
}

void ekg::os::sdl::update_monitor_resolution() {
  SDL_DisplayMode sdl_display_mode {};
  SDL_GetDisplayMode(0, 0, &sdl_display_mode);

  this->monitor_resolution[0] = static_cast<int32_t>(sdl_display_mode.w);
  this->monitor_resolution[1] = static_cast<int32_t>(sdl_display_mode.h);
}

uint64_t ekg::os::sdl::get_ticks() {
  return SDL_GetTicks64();
}

void ekg::os::sdl::get_key_name(int32_t key, std::string &name) {
  switch (key) {
    case SDLK_LCTRL:
      name = "lctrl";
      break;
    case SDLK_RCTRL:
      name = "rctrl";
      break;
    case SDLK_LSHIFT:
      name = "lshift";
      break;
    case SDLK_RSHIFT:
      name = "rshift";
      break;
    case SDLK_LALT:
      name = "alt";
      break;
    case SDLK_RALT:
      name = "altgr";
      break;
    case SDLK_TAB:
      name = "tab";
      break;
    default:
      name = SDL_GetKeyName(key);
      break;
  }
}

void ekg::os::sdl::get_special_key(int32_t key, ekg::special_key &special_key) {
  switch (key) {
    case SDLK_LCTRL:
      special_key = ekg::special_key::left_ctrl;
      break;
    case SDLK_RCTRL:
      special_key = ekg::special_key::right_ctrl;
      break;
    case SDLK_LSHIFT:
      special_key = ekg::special_key::left_shift;
      break;
    case SDLK_RSHIFT:
      special_key = ekg::special_key::right_shift;
      break;
    case SDLK_LALT:
      special_key = ekg::special_key::left_alt;
      break;
    case SDLK_RALT:
      special_key = ekg::special_key::right_alt;
      break;
    case SDLK_TAB:
      special_key = ekg::special_key::tab;
      break;
    default:
      special_key = ekg::special_key::unknown;
      break;
  }
}

void ekg::os::sdl_poll_event(SDL_Event &sdl_event) {
  ekg::os::io_event_serial &serialized {ekg::core->io_event_serial};
  serialized = ekg::os::io_event_serial {};

  switch (sdl_event.type) {
  case SDL_WINDOWEVENT:
    switch (sdl_event.window.event) {
      case SDL_WINDOWEVENT_SIZE_CHANGED:
        ekg::ui::width = sdl_event.window.data1;
        ekg::ui::height = sdl_event.window.data2;
        
        ekg::core->p_gpu_api->update_viewport(ekg::ui::width, ekg::ui::height);
        ekg::core->update_size_changed();

        break;
    }

    break;

  case SDL_KEYDOWN:
    serialized.is_key_down = true;
    serialized.key = static_cast<int32_t>(sdl_event.key.keysym.sym);
    ekg::poll_io_event = true;
    break;

  case SDL_KEYUP:
    serialized.is_key_up = true;
    serialized.key = static_cast<int32_t>(sdl_event.key.keysym.sym);
    ekg::poll_io_event = true;
    break;

  case SDL_TEXTINPUT:
    serialized.is_text_input = true;
    serialized.text_input = sdl_event.text.text;
    ekg::poll_io_event = true;
    break;

  case SDL_MOUSEBUTTONUP:
    serialized.is_mouse_button_up = true;
    serialized.mouse_button = sdl_event.button.button;
    ekg::poll_io_event = true;
    break;

  case SDL_MOUSEBUTTONDOWN:
    serialized.is_mouse_button_down = true;
    serialized.mouse_button = sdl_event.button.button;
    ekg::poll_io_event = true;
    break;

  case SDL_MOUSEWHEEL:
    serialized.is_mouse_wheel = true;
    serialized.mouse_wheel_x = sdl_event.wheel.x;
    serialized.mouse_wheel_y = sdl_event.wheel.y;
    serialized.mouse_wheel_precise_x = sdl_event.wheel.preciseX;
    serialized.mouse_wheel_precise_y = sdl_event.wheel.preciseY;
    ekg::poll_io_event = true;
    break;

  case SDL_FINGERUP:
    serialized.is_finger_up = true;
    serialized.finger_x = sdl_event.tfinger.x;
    serialized.finger_y = sdl_event.tfinger.y;        
    ekg::poll_io_event = true;
    break;

  case SDL_FINGERDOWN:
    serialized.is_finger_down = true;
    serialized.finger_x = sdl_event.tfinger.x;
    serialized.finger_y = sdl_event.tfinger.y;   
    ekg::poll_io_event = true;
    break;

  case SDL_FINGERMOTION:
    serialized.is_finger_motion = true;
    serialized.finger_x = sdl_event.tfinger.x;
    serialized.finger_y = sdl_event.tfinger.y;
    serialized.finger_dx = sdl_event.tfinger.dx;
    serialized.finger_dy = sdl_event.tfinger.dy;
    ekg::poll_io_event = true;
    break;
  }
}