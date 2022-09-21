#include "ekg/ekg.hpp"
#include "ekg/cpu/cpu_input.hpp"
#include "ekg/gpu/gpu_gl.hpp"

ekg_core* ekg::core {nullptr};

void ekg::init() {
    ekg::log("Initialising ekg...");

    ekg::core = new ekg_core();
    ekg::core->init();
}

void ekg::quit() {
    ekg::log("Quiting from ekg...");

    ekg::core->quit();
    delete ekg::core;
}

void ekg::event(SDL_Event &sdl_event) {
    ekg::cpu::process_input(sdl_event);
    ekg::core->process_event(sdl_event);
}

void ekg::update() {
    ekg::core->process_update();
}

void ekg::render() {
    ekg::core->process_render();
}

void ekg::demo() {
    ekg::log("Initialising demo showcase");
    SDL_Window* sdl_win {SDL_CreateWindow("ekg showcase", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 800, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL)};

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    ekg::gpu::init_opengl_context();

    SDL_GLContext sdl_gl_context {SDL_GL_CreateContext(sdl_win)};
    bool running {true};

    ekg::log("OpenGL 4 context created");

    ekg::timing mainloop_timing {};
    ekg::timing fps_timing {};

    SDL_Event sdl_event {};

    // z-depth testing.
    glEnable(GL_DEPTH_TEST);

    // Alpha channel (blended).
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    uint64_t fps = 60;
    uint64_t fps_ms_interval {1000 / fps};
    uint64_t last_ticked_frames {};
    uint64_t ticked_frames {};

    /*
     * Mainloop.
     */
    while (running) {
        /*
         * Handle ticked loop each frame (1s).
         */
        if (ekg::util::reach(mainloop_timing, fps_ms_interval)) {
            ekg::util::reset(mainloop_timing);
            ekg::dt = static_cast<float>(mainloop_timing.current_ticks) / 100;

            if (ekg::util::reach(fps_timing, 1000)) {
                ekg::util::reset(fps_timing);

                last_ticked_frames = ticked_frames;
                ticked_frames = 0;
            }

            while (SDL_PollEvent(&sdl_event)) {
                switch (sdl_event.type) {
                    case SDL_QUIT: {
                        running = false;
                        break;
                    }

                    default: {
                        ekg::event(sdl_event);
                        break;
                    }
                }
            }

            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

            // Count the FPS.
            ticked_frames++;

            // Swap buffers.
            SDL_GL_SwapWindow(sdl_win);
        }
    }
}
