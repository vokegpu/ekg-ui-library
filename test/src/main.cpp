/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, free to use in anything (utility, hacking, cheat, game, software).
 * 5- Malware, rat and others virus. We do not care.
 * 6- Do not modify this license under any instance.
 *
 * @VokeGpu 2022 all rights reserved.
 */

#include <ekg/ekg.hpp>

/**
 * Created by Rina.
 **/
int32_t main(int, char**) {
    ekg::log("Initialising demo showcase");

    int32_t root_width {1280};
    int32_t root_height {768};

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* sdl_win {SDL_CreateWindow("ekg showcase", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, root_width, root_height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL)};

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    SDL_GLContext sdl_gl_context {SDL_GL_CreateContext(sdl_win)};
    bool running {true};
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        ekg::log("Failed to initialise GLEW OpenGL context!");
    } else {
        ekg::log("GLEW initialised");
    }

    SDL_GL_SetSwapInterval(1); // v-sync on
    ekg::init(sdl_win, "JetBrainsMono-Bold.ttf");
    ekg::log("OpenGL 4 context created!");

    ekg::timing mainloop_timing {};
    ekg::timing fps_timing {};
    SDL_Event sdl_event {};

    // z-depth testing.
    glEnable(GL_DEPTH_TEST);

    uint64_t fps {60};
    uint64_t fps_ms_interval {1000 / fps};
    uint64_t display_fps {};
    uint64_t ticked_frames {};

    for (int32_t i {0}; i < 1; i++) {
        auto frame {ekg::frame("tag", {20, 20}, {200, 200})};
        frame->set_drag(ekg::dock::top);
        frame->set_resize(ekg::dock::left | ekg::dock::bottom);
        ekg::label("Hi, the label:"); // dock automatically set to ekg::dock::left | ekg::dock::top
        ekg::button("Button Press Me!", ekg::dock::top | ekg::dock::left | ekg::dock::next)->set_callback(
                new ekg::cpu::event {"button press", nullptr, [](void *data) {
                    SDL_Event sdl_event {};
                    sdl_event.type = SDL_QUIT;
                    SDL_PushEvent(&sdl_event);
                }}); // next does the "break line" dock (bottom to up and top to bottom).
        ekg::checkbox("hi", ekg::dock::top | ekg::dock::left | ekg::dock::next)->set_width(200);
        ekg::checkbox("hello", ekg::dock::top | ekg::dock::left)->set_width(200);
        ekg::slider("slider", 20, 20, 200);
        ekg::popgroup();
    }

    /*
     * Mainloop.
     */
    while (running) {
        /*
         * Handle ticked loop each frame (1s).
         */
        if (ekg::reach(mainloop_timing, fps_ms_interval) && ekg::reset(mainloop_timing)) {
            ekg::display::dt = static_cast<float>(mainloop_timing.current_ticks) / 100;

            if (ekg::reach(fps_timing, 1000) && ekg::reset(fps_timing)) {
                display_fps = ticked_frames;
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

            ekg::update();

            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

            ekg::render();

            // Count the FPS.
            ticked_frames++;

            // Swap buffers.
            SDL_GL_SwapWindow(sdl_win);
        }
    }

    ekg::quit();

    return 0;
}