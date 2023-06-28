#include "ekg/os/systemcursor.hpp"
#include "ekg/util/env.hpp"

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
}

void ekg::set_cursor(ekg::systemcursor system_cursor) {
    SDL_SetCursor(ekg::loadedsystemcursorlist[static_cast<uint32_t>(system_cursor)]);
}