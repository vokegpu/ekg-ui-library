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
 * @VokeGpu 2023 all rights reserved.
 */

#include <ekg/ekg.hpp>

/**
 * Created by Rina.
 **/
int32_t main(int, char**) {
    ekg::log() << "EKG User interface library demo starting";

    int32_t root_width {1280};
    int32_t root_height {768};

    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    SDL_Window* sdl_win {SDL_CreateWindow("ekg showcase", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, root_width, root_height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL)};
    SDL_GLContext sdl_gl_context {SDL_GL_CreateContext(sdl_win)};
    bool running {true};
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        ekg::log(ekg::log::ERROR) << "Failed to initialise GLEW";
    } else {
        ekg::log() << "GLEW initialised";
    }

    SDL_GL_SetSwapInterval(0); // v-sync on
    ekg::init(sdl_win, "JetBrainsMono-Bold.ttf");
    ekg::log() << "OpenGL context created";
    ekg::debug = true;
    SDL_SetWindowOpacity(sdl_win, 1.0f);

    ekg::timing mainloop_timing {};
    ekg::timing fps_timing {};
    SDL_Event sdl_event {};

    // z-depth testing.
    glEnable(GL_DEPTH_TEST);

    uint64_t fps {60};
    uint64_t fps_ms_interval {1000 / fps};
    uint64_t display_fps {};
    uint64_t ticked_frames {};

    auto frame {ekg::frame("text sampler", {20, 20}, {200, 200})};
    frame->set_drag(ekg::dock::top);
    frame->set_resize(ekg::dock::left | ekg::dock::bottom | ekg::dock::right);

    auto fillnext = ekg::dock::fill | ekg::dock::next;

    ekg::button("Button 1", ekg::dock::fill);

    ekg::button("Button 2", fillnext);
    ekg::button("Button 3", ekg::dock::fill);
    
    ekg::button("Button 4", fillnext);
    ekg::button("Button 5");
    ekg::button("Button 6", ekg::dock::next);
    // ekg::button("Button 7", ekg::dock::right);
    // ekg::button("Button 8", ekg::dock::fill | ekg::dock::bottom);

    uint64_t cpu_now_ticks {}, cpu_last_ticks {};

    /*
     * Mainloop.
     */
    while (running) {
        cpu_last_ticks = cpu_now_ticks;
        cpu_now_ticks = SDL_GetPerformanceCounter();
        ekg::display::dt = static_cast<float>(cpu_now_ticks - cpu_last_ticks) / static_cast<float>(SDL_GetPerformanceFrequency());

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

                case SDL_USEREVENT: {
                    ekg::cpu::uievent event {};
                    if (ekg::listen(event, sdl_event)) {
                    }
                    break;
                }

                default: {
                    ekg::event(sdl_event);

                    if (ekg::input::released() && ekg::input::receive("mouse-3-up")) {
                        auto popup = ekg::popup("hello", {"Element One", "Element Two", "Element Three", "Element Four"});
                        if (popup != nullptr) popup->set_tag(popup->get_tag() + std::to_string(popup->get_id()));
                    }

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
        SDL_Delay(fps_ms_interval);
    }

    ekg::quit();

    return 0;
}