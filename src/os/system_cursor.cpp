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

#include "ekg/os/system_cursor.hpp"

#include <cinttypes>
#include <iostream>
#include <fstream>

#include "ekg/os/ekg_sdl.hpp"

ekg::systemcursor ekg::cursor {};

void ekg::init_cursor() {
    ekg::loadedsystemcursorlist[static_cast<uint32_t>(ekg::systemcursor::arrow)]     = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    ekg::loadedsystemcursorlist[static_cast<uint32_t>(ekg::systemcursor::ibeam)]     = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
    ekg::loadedsystemcursorlist[static_cast<uint32_t>(ekg::systemcursor::wait)]      = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
    ekg::loadedsystemcursorlist[static_cast<uint32_t>(ekg::systemcursor::crosshair)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
    ekg::loadedsystemcursorlist[static_cast<uint32_t>(ekg::systemcursor::waitarrow)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAITARROW);
    ekg::loadedsystemcursorlist[static_cast<uint32_t>(ekg::systemcursor::sizenwse)]  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
    ekg::loadedsystemcursorlist[static_cast<uint32_t>(ekg::systemcursor::sizenesw)]  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
    ekg::loadedsystemcursorlist[static_cast<uint32_t>(ekg::systemcursor::sizewe)]    = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
    ekg::loadedsystemcursorlist[static_cast<uint32_t>(ekg::systemcursor::sizens)]    = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
    ekg::loadedsystemcursorlist[static_cast<uint32_t>(ekg::systemcursor::sizeall)]   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
    ekg::loadedsystemcursorlist[static_cast<uint32_t>(ekg::systemcursor::no)]        = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);
    ekg::loadedsystemcursorlist[static_cast<uint32_t>(ekg::systemcursor::hand)]      = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

    ekg::set_cursor(ekg::systemcursor::arrow);
    ekg::cursor = ekg::systemcursor::arrow;
}

void ekg::set_cursor(ekg::systemcursor sys_cursor) {
    SDL_SetCursor(ekg::loadedsystemcursorlist[static_cast<uint32_t>(sys_cursor)]);
}