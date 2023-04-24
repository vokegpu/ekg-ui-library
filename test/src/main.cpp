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

    auto frame {ekg::frame("text sampler", {20, 20}, {300, 200})};
    frame->set_drag(ekg::dock::top);
    frame->set_resize(ekg::dock::left | ekg::dock::bottom | ekg::dock::right);

    auto fillnext = ekg::dock::fill | ekg::dock::next;

    auto slider = ekg::slider("Button 1", 0.0f, 0.0f, 1000000.0f, ekg::dock::fill);
    slider->set_text_align(ekg::dock::center);
    slider->set_precision(23);

    ekg::checkbox("", ekg::dock::fill)->set_box_align(ekg::dock::center);
    ekg::slider("Button 3", 34.0f, 0.0f, 200.0f, fillnext)->set_text_align(ekg::dock::center);
    ekg::button("Button 4.1", ekg::dock::next)->set_text_align(ekg::dock::center | ekg::dock::right);
    ekg::button("Button 4.2")->set_text_align(ekg::dock::center);
    ekg::button("Button 4.3", ekg::dock::fill)->set_text_align(ekg::dock::center);
    ekg::button("Button 3", ekg::dock::next);
    ekg::label("Text Cat Sampler", ekg::dock::fill)->set_text_align(ekg::dock::center);
    ekg::button("Button 3.66", ekg::dock::fill);
    ekg::button("Button .", ekg::dock::next);
    
    ekg::button("Button 4", fillnext);
    ekg::button("Button 5");
    ekg::button("Button 6", ekg::dock::next);

    auto frame2 {ekg::frame("text sampler", {20, 20}, {200, 200})};
    frame2->set_drag(ekg::dock::top);
    frame2->set_resize(ekg::dock::left | ekg::dock::bottom | ekg::dock::right);

    ekg::button("Button 1", ekg::dock::fill)->set_text_align(ekg::dock::center);
    ekg::button("Button 2", fillnext);
    ekg::button("Button 2.1", ekg::dock::fill);
    ekg::button("Button 2.2", fillnext);
    ekg::button("Button 2.3");
    ekg::button("Button 3", ekg::dock::next);
    ekg::button("Button 4");

    // ekg::button("Button 7", ekg::dock::right);
    // ekg::button("Button 8", ekg::dock::fill | ekg::dock::bottom);

    uint64_t cpu_now_ticks {}, cpu_last_ticks {};
    ekg::cpu::uievent event {};

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

                default: {
                    if (ekg::listen(event, sdl_event)) {
                        ekg::log() << event.tag + " " + event.value + " " << event.type;
                    }

                    ekg::event(sdl_event);

                    if (ekg::input::released() && ekg::input::receive("mouse-3-up")) {
                        auto main = ekg::popup("hello", {"Element One", "---Element Two", "---Element Three", "Element Four"});
                        auto three = ekg::popup("buu", {"Buu", "AAA", " oi MinecFT"});
                        auto game = ekg::popup("game", {"cat1", "---cat2", "cat3", "cat43"});

                        if (main != nullptr && three != nullptr) {
                            main->append_linked("Element Three", three);
                            three->append_linked("AAA", game);
                        }
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