#include <ekg/ekg.hpp>
#include <ekg/os/ekg_sdl.hpp>
#include <ekg/os/ekg_opengl.hpp>

int32_t showcase_useless_window() {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *p_sdl_win {
    SDL_CreateWindow(
      "Pompom",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      1280,
      720,
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
    )
  };

  SDL_Event sdl_event {};
  SDL_GLContext sdl_gl_context {SDL_GL_CreateContext(p_sdl_win)};

  glewExperimental = GL_TRUE;
  glewInit();

  ekg::runtime_property ekg_runtime_property {
    .p_font_path = "whitneybook.otf",
    .p_gpu_api = new ekg::os::opengl(),
    .p_os_platform = new ekg::os::sdl(p_sdl_win)
  };

  ekg::runtime runtime {};
  ekg::init(&runtime, &ekg_runtime_property);

  auto frame = ekg::frame("aurora-fofa", {20, 20}, {200, 200})->set_drag(ekg::dock::top);

  ekg::input::bind("hiroodrop", "finger-click");
  ekg::input::bind("hiroodrop", "mouse-1");
  ekg::input::bind("hiroodrop", "lctrl+b");
  ekg::input::bind("hiroodrop", "a-up");
  ekg::input::bind("hiroodrop", "b-up");

  bool running {true};

  while (running) {
    while (SDL_PollEvent(&sdl_event)) {
      ekg::os::sdl_poll_event(sdl_event);
    
      switch (sdl_event.type) {
      case SDL_QUIT:
        running = false;
        break;
      }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.6f, 0.0f, 0.6f, 0.993f);

    ekg::ui::dt = 0.016f;
    ekg::update();
    ekg::render();

    SDL_GL_SwapWindow(p_sdl_win);
    SDL_Delay(16);
  }

  return 666;
}

int32_t main(int32_t, char**) {
  return showcase_useless_window();
}