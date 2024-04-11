#ifndef EKG_TEST_APPLICATION_HPP
#define EKG_TEST_APPLICATION_HPP

#include <SDL2/SDL.h>

extern struct application {
  SDL_Window *p_sdl_win {};
  bool vsync {true};
} app;

#endif