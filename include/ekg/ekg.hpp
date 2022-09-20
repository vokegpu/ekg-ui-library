#include <SDL2/SDL.h>

namespace ekg {
    void init();
    void quit();

    void event(SDL_Event &sdl_event);
    void update();
    void render();
};