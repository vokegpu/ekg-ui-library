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

#ifndef EKG_OS_SDL_H
#define EKG_OS_SDL_H

#if defined(__ANDROID__)
#include "SDL.h"
#else
#include <SDL2/SDL.h>
#endif

#include "platform.hpp"

namespace ekg::os {
  class sdl : public ekg::os::platform {
  protected:
    SDL_Cursor loaded_system_cursor_list[12] {};
    SDL_Window *p_sdl_win {};
    SDL_Event *p_sdl_event {};
  public:
    sdl(SDL_Window *p_sdl_win, SDL_Event *p_sdl_event);
  public:
    void init() override;
    void quit() override;

    void update_monitor_resolution() override;
    void update_cursor(ekg::system_cursor system_cursor) override;
  };
}

#endif