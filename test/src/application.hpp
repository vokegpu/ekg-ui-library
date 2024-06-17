#ifndef EKG_TEST_APPLICATION_HPP
#define EKG_TEST_APPLICATION_HPP

#include <ekg/ekg.hpp>
#include <ekg/os/ekg_sdl.hpp>

extern struct application {
  SDL_Window *p_sdl_win {};
  bool vsync {true};
} app;

namespace laboratory {
  struct shape {
  protected:
    float mat4x4_projection[16] {};
    uint32_t program {};
    uint32_t gbuffer {};
    uint32_t buffer_collection {};
    uint32_t uniform_matrix_projection {};
    uint32_t uniform_rect {};
    uint32_t uniform_color {};
  public:
    shape();
  public:
    void on_resize();
    void invoke();
    void draw(const ekg::rect &rect, const ekg::vec4 &color);
    void revoke();
  };
}

#endif