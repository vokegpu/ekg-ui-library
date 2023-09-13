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

#include <cinttypes>
#include <iostream>
#include <fstream>

#include "ekg/os/platform.hpp"
#include "ekg/os/ekg_sdl.hpp"

ekg::system_cursor ekg::cursor {};

void ekg::init_cursor() {
    ekg::loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::arrow)]      = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    ekg::loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::ibeam)]      = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
    ekg::loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::wait)]       = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
    ekg::loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::crosshair)]  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
    ekg::loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::wait_arrow)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAITARROW);
    ekg::loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::size_nwse)]  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
    ekg::loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::size_nesw)]  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
    ekg::loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::size_we)]    = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
    ekg::loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::size_ns)]    = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
    ekg::loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::size_all)]   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
    ekg::loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::no)]         = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);
    ekg::loaded_system_cursor_list[static_cast<uint32_t>(ekg::system_cursor::hand)]       = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

    ekg::set_cursor(ekg::system_cursor::arrow);
    ekg::cursor = ekg::system_cursor::arrow;
}

void ekg::set_cursor(ekg::system_cursor sys_cursor) {
    SDL_SetCursor(ekg::loaded_system_cursor_list[static_cast<uint32_t>(sys_cursor)]);
}

void ekg::os_get_monitor_resolution(float &w, float &h) {
    SDL_DisplayMode sdl_display_mode {};
    SDL_GetDisplayMode(0, 0, &sdl_display_mode);
    w = sdl_display_mode.w;
    h = sdl_display_mode.h;
}