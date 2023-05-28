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

/*
 * Created by Rina.
 */
int32_t main(int32_t, char**) {
    std::string utf8pompom {"𒁑𒁕𒈢𒋟𒈙"};
    ekg::log() << utf8pompom << " utf8 size: " << ekg::utf8length(utf8pompom) << " string size: " << utf8pompom.size();
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

    std::string textcatsampler {};
    textcatsampler += "não§§È1II21II§2121§§2212121212126\r\n"
                      "[EKG-INFO] utf8 size: 5 string size: 20\n"
                      "[EKG-INFO] EKG User interface library demo starting\n"
                      "[EKG-INFO] 3\n"
                      "[EKG-INFO] GLEW initialised\n"
                      "[EKG-INFO] Initialising EKG\n"
                      "[EKG-INFO] GPU allocator initialised\n"
                      "[EKG-INFO] Initialising default theme\n"
                      "[EKG-INFO] Analysing files for themes\n"
                      "[EKG-INFO] Preparing internal EKG core\n"
                      "[EKG-INFO] Preparing internal user interface environment\n"
                      "[EKG-INFO] Registering user interface input bindings\n"
                      "[EKG-INFO] SDL version: 2.26.5\n"
                      "[EKG-INFO] OpenGL context created\n"
                      "[EKG-INFO] Shutdown complete - Thank you for using EKG ;) <3\n";

    auto framedebug = ekg::frame("frame-debug", {700, 20}, {400, 415});
    framedebug->set_drag(ekg::dock::top);
    framedebug->set_resize(ekg::dock::left | ekg::dock::right);

    ekg::label("Debug:", ekg::dock::fill | ekg::dock::next);
    auto textboxdebug = ekg::textbox("textbox-debug", "cat\ncat\ncat\r\noioi", ekg::dock::fill | ekg::dock::next);
    textboxdebug->set_scaled_height(12);
    ekg::popgroup();

    for (int32_t it {}; it < 1; it++) {
        auto frame2 {ekg::frame("text sampler", {20, 30}, {400, 400})};
        frame2->set_drag(ekg::dock::top);
        frame2->set_resize(ekg::dock::left | ekg::dock::bottom | ekg::dock::right);

        ekg::theme().gen_default_light_theme();
        ekg::checkbox("Light Theme", true, ekg::dock::fill | ekg::dock::next)->set_callback(new ekg::cpu::event {"theme-switcher", frame2, [](void *pdata) {
            ekg::ui::frame *frame {static_cast<ekg::ui::frame*>(pdata)};
            // frame->set_pos(20, 20);
            // @TODO fix random issue with set positon

            auto &theme {ekg::theme()};
            std::string current_theme_name {theme.get_current_theme_name()};

            if (current_theme_name == "dark") {
                theme.gen_default_light_theme();
            } else if (current_theme_name == "light") {
                theme.gen_default_dark_theme();
            }
        }});

        auto label = ekg::label("0", ekg::dock::fill | ekg::dock::next);
        label->set_scaled_height(4);
        label->set_text_align(ekg::dock::right | ekg::dock::bottom);
        label->set_font_size(ekg::font::big);

        auto buselesstop1 = ekg::button("", ekg::dock::fill | ekg::dock::next);
        buselesstop1->set_scaled_height(2);
        buselesstop1->set_text_align(ekg::dock::center);
        buselesstop1->set_tag("minecraft");

        auto buselesstop2 = ekg::button("", ekg::dock::fill);
        buselesstop2->set_scaled_height(2);
        buselesstop2->set_text_align(ekg::dock::center);

        auto buselesstop3 = ekg::button("", ekg::dock::fill);
        buselesstop3->set_scaled_height(2);
        buselesstop3->set_text_align(ekg::dock::center);

        auto berase = ekg::button("<=", ekg::dock::fill);
        berase->set_scaled_height(2);
        berase->set_text_align(ekg::dock::center);

        auto b7 = ekg::button("7", ekg::dock::fill | ekg::dock::next);
        b7->set_scaled_height(2);
        b7->set_text_align(ekg::dock::center);
        
        auto b8 = ekg::button("8", ekg::dock::fill);
        b8->set_scaled_height(2);
        b8->set_text_align(ekg::dock::center);
        
        auto b9 = ekg::button("9", ekg::dock::fill);
        b9->set_scaled_height(2);
        b9->set_text_align(ekg::dock::center);

        auto bmultiply = ekg::button("x", ekg::dock::fill);
        bmultiply->set_scaled_height(2);
        bmultiply->set_text_align(ekg::dock::center);

        auto b4 = ekg::button("4", ekg::dock::fill | ekg::dock::next);
        b4->set_scaled_height(2);
        b4->set_text_align(ekg::dock::center);
        
        auto b5 = ekg::button("5", ekg::dock::fill);
        b5->set_scaled_height(2);
        b5->set_text_align(ekg::dock::center);
        
        auto b6 = ekg::button("6", ekg::dock::fill);
        b6->set_scaled_height(2);
        b6->set_text_align(ekg::dock::center);

        auto bsubtract = ekg::button("-", ekg::dock::fill);
        bsubtract->set_scaled_height(2);
        bsubtract->set_text_align(ekg::dock::center);

        auto b1 = ekg::button("1", ekg::dock::fill | ekg::dock::next);
        b1->set_scaled_height(2);
        b1->set_text_align(ekg::dock::center);
        
        auto b2 = ekg::button("2", ekg::dock::fill);
        b2->set_scaled_height(2);
        b2->set_text_align(ekg::dock::center);
        
        auto b3 = ekg::button("3", ekg::dock::fill);
        b3->set_scaled_height(2);
        b3->set_text_align(ekg::dock::center);

        auto baddition = ekg::button("+", ekg::dock::fill);
        baddition->set_scaled_height(2);
        baddition->set_text_align(ekg::dock::center);

        auto buseless1 = ekg::button("", ekg::dock::fill | ekg::dock::next);
        buseless1->set_scaled_height(2);
        buseless1->set_text_align(ekg::dock::center);

        auto b0 = ekg::button("0", ekg::dock::fill);
        b0->set_scaled_height(2);
        b0->set_text_align(ekg::dock::center);

        auto buseless2 = ekg::button("", ekg::dock::fill);
        buseless2->set_scaled_height(2);
        buseless2->set_text_align(ekg::dock::center);

        auto bassign = ekg::button("=", ekg::dock::fill);
        bassign->set_scaled_height(2);
        bassign->set_text_align(ekg::dock::center);
        bassign->set_tag("calculador-assign");

        ekg::scroll("pompom");

        //auto slider = ekg::slider("Button 1", 0.0f, 0.0f, 1000000.0f, ekg::dock::fill);
        //slider->set_text_align(ekg::dock::center);
        //slider->set_precision(23);
    }

    // ekg::button("Button 7", ekg::dock::right);
    // ekg::button("Button 8", ekg::dock::fill | ekg::dock::bottom);

    uint64_t cpu_now_ticks {}, cpu_last_ticks {};
    ekg::cpu::uievent event {};

    ekg::input::bind("pompom", "c-up");
    ekg::input::bind("pompom", "lctrl+b");

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
                    ekg::event(sdl_event);
                    if (ekg::listen(event, sdl_event)) {
                        ekg::log() << event.tag + " " + event.value + " " << event.type;
                    }

                    if (ekg::input::released() && ekg::input::receive("mouse-3-up")) {
                        auto main = ekg::popup("file", {"---Add", "Cut", "Copy", "---Paste", "---Select All", "Actions"});
                        auto three = ekg::popup("file-add", {"Cube", "Plane", "Sphere", "Hexagon", "Hexagon"});
                        auto hexagon = ekg::popup("file-add-hexagon", {"Tree D", "Plane", "Double Pairs Daggers"});
                        auto game = ekg::popup("file-actions", {"Reload Clock", "Flush"});

                        if (main != nullptr && three != nullptr) {
                            main->append_linked("Add", three);
                            main->append_linked("Actions", game);
                            three->append_linked("Hexagon", hexagon);
                        }
                    }

                    if (ekg::input::pressed("pompom")) {
                        ekg::log() << "sou gostosa";
                    }

                    break;
                }
            }
        }

        ekg::update();

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

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