#include "ekg/ekg.hpp"
#include "ekg/cpu/cpu_input.hpp"
#include "ekg/gpu/gpu_gl.hpp"
#include "ekg/cpu/cpu_info.hpp"

ekg::runtime* ekg::core {nullptr};
std::string ekg::gl_version {"#version 450"};

void ekg::init(SDL_Window* root) {
    ekg::log("Initialising ekg...");

#if defined(_WIN)
    ekg::os = {ekg::platform::os_win};
#elif defined(__linux__)
    ekg::os = {ekg::platform::os_linux};
#elif defined(__ANDROID__)
    ekg::os = {ekg::platform::os_android};
#endif

    ekg::core = new ekg::runtime();
    ekg::core->init();
    ekg::core->set_root(root);

    SDL_GetWindowSize(root, &ekg::display::width, &ekg::display::height);
    ekg::log("Root display size (" + std::to_string(ekg::display::width) + ", " + std::to_string(ekg::display::height) + ")");
}

void ekg::quit() {
    ekg::log("Quiting from ekg...");

    ekg::core->quit();
    delete ekg::core;
}

void ekg::event(SDL_Event &sdl_event) {
    ekg::cpu::process_input(sdl_event);
    ekg::core->process_event(sdl_event);

    switch (sdl_event.type) {
        case SDL_WINDOWEVENT: {
            switch (sdl_event.window.type) {
                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    ekg::display::width = sdl_event.window.data1;
                    ekg::display::height = sdl_event.window.data2;
                    break;
                }
            }

            break;
        }
    }
}

void ekg::update() {
    ekg::core->process_update();
}

void ekg::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ekg::core->process_render();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void ekg::demo() {
    float root_width {1280.0f};
    float root_height {800.0f};

    SDL_Init(SDL_INIT_VIDEO);

    ekg::log("Initialising demo showcase");
    SDL_Window* sdl_win {SDL_CreateWindow("ekg showcase", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, root_width, root_height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL)};

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    SDL_GLContext sdl_gl_context {SDL_GL_CreateContext(sdl_win)};
    bool running {true};

    ekg::gpu::init_opengl_context();
    ekg::init(sdl_win);
    ekg::log("OpenGL 4 context created");

    ekg::timing mainloop_timing {};
    ekg::timing fps_timing {};

    SDL_Event sdl_event {};

    // z-depth testing.
    glEnable(GL_DEPTH_TEST);

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
        if (ekg::reach(mainloop_timing, fps_ms_interval)) {
            ekg::reset(mainloop_timing);
            ekg::display::dt = static_cast<float>(mainloop_timing.current_ticks) / 100;

            if (ekg::reach(fps_timing, 1000)) {
                ekg::reset(fps_timing);

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

            glViewport(0, 0, static_cast<int32_t>(root_width), static_cast<int32_t>(root_height));
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

            ekg::render();

            // Count the FPS.
            ticked_frames++;

            // Swap buffers.
            SDL_GL_SwapWindow(sdl_win);
        }
    }
}

void ekg::depth(float depth_preset) {

}
