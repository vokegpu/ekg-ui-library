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

#define SOUGOSTOSA

std::string checkcalc(std::string_view text, std::string_view operatortext) {
    std::string result {};
    result += text.substr(text.size() - 1, 1);
    if ((result == "*" || result == "-" || result == "+")) {
        result.clear();
        result += text.substr(0, text.size() - 1);
        result += operatortext;
    } else {
        result.clear();
        result += text;
        result += operatortext;
    }

    return result;
}

std::string resultcalc(std::string_view text) {
    std::string result {};

    int64_t lpom {};
    int64_t rpom {};
    int64_t cpom {};

    bool firstoperation {true};
    bool reset {};

    uint64_t find {};
    uint64_t size {text.size()};
    
    std::string textsubstr {text};
    std::string aliasoperator[3] {
        "*", "-", "+"
    };

    uint64_t it {};
    char prevoperator {};
    bool docalc {};
    bool kissme {};

    for (it = 0; it < size; it++) {
        if (!docalc) {
            switch (textsubstr.at(it)) {
            case '*':
                cpom = std::stoi(result);
                result.clear();
                docalc = true;
                break;
            case '-':
                cpom = std::stoi(result);
                result.clear();
                docalc = true;
                break;
            case '+':
                cpom = std::stoi(result);
                result.clear();
                docalc = true;
                break;
            default:
                result += textsubstr.at(it);
                break;
            }

            prevoperator = textsubstr.at(it);
            continue;
        }

        kissme = false;
        switch (textsubstr.at(it)) {
        case '*':
            kissme = true;
            break;
        case '-':
            kissme = true;
            break;
        case '+':
            kissme = true;
            break;
        default:
            kissme = it == size - 1;
            if (kissme) result += textsubstr.at(it);

            break;
        }

        if (!kissme) {
            result += textsubstr.at(it);
            continue;
        }

        switch (prevoperator) {
        case '*':
            cpom = cpom * std::stoi(result);
            result.clear();
            prevoperator = textsubstr.at(it);
            break;
        case '-':
            cpom = cpom - std::stoi(result);
            result.clear();
            prevoperator = textsubstr.at(it);
            break;
        case '+':
            cpom = cpom + std::stoi(result);
            result.clear();
            prevoperator = textsubstr.at(it);
            break;
        default:
            break;
        }
    }


    result = std::to_string(cpom);
    return result;
}

/*
 * Created by Rina.
 */
int32_t main(int32_t, char**) {
    std::string utf8pompom {"𒁑𒁕𒈢𒋟𒈙"};
    ekg::log() << utf8pompom << " utf8 size: " << ekg::utf8length(utf8pompom) << " string size: " << utf8pompom.size();
    ekg::log() << "EKG User interface library demo starting";

    int32_t root_width {1280};
    int32_t root_height {720};

    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    SDL_Window* sdl_win {SDL_CreateWindow("Pompom Calculator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, root_width, root_height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL)};
    SDL_GLContext sdl_gl_context {SDL_GL_CreateContext(sdl_win)};
    bool running {true};
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        ekg::log(ekg::log::ERROR) << "Failed to initialise GLEW";
    } else {
        ekg::log() << "GLEW initialised";
    }

    SDL_GL_SetSwapInterval(0); // v-sync on
    ekg::init(sdl_win, "whitneybook.otf");
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

    auto framedebug = ekg::frame("frame-debug", {0, 0}, {400, root_height});
    framedebug->set_resize(ekg::dock::right | ekg::dock::bottom);

    auto debuglabel = ekg::label("Debug:", ekg::dock::fill | ekg::dock::next);
    auto textboxdebug = ekg::textbox("textbox-debug", "", ekg::dock::fill | ekg::dock::next);
    textboxdebug->set_scaled_height(6);
    textboxdebug->set_enabled(false);

    ekg::ui::label *labelresult {};
    std::string previous_operator {};
    ekg::ui::frame *frame2 {};

    for (int32_t it {}; it < 1; it++) {
        auto frame1 = ekg::frame("cat", {400, 400}, ekg::dock::fill);
        frame1->set_resize(ekg::dock::right | ekg::dock::bottom | ekg::dock::left);

        ekg::checkbox("meow enabled", false, ekg::dock::next);
        ekg::checkbox("meow enabled", false, ekg::dock::next);
        ekg::checkbox("meow enabled", false);
        ekg::checkbox("meow enabled", false, ekg::dock::next);

        auto slider = ekg::slider("gato", 0.0f, 0.0f, 1000000.0f, ekg::dock::fill);
        slider->set_text_align(ekg::dock::center);
        slider->set_precision(23);
        ekg::checkbox("meow enabled", false, ekg::dock::fill | ekg::dock::next);
        ekg::scroll("pompom-meow");

        framedebug->add_child(frame1->get_id());

        frame2 = ekg::frame("cha", {400, 400}, ekg::dock::fill);
        frame2->set_resize(ekg::dock::right | ekg::dock::bottom | ekg::dock::left);
        framedebug->add_child(frame2->get_id());

        ekg::theme().gen_default_dark_theme();
        ekg::checkbox("Light Theme", false, ekg::dock::fill | ekg::dock::next)->set_callback(new ekg::cpu::event {"theme-switcher", frame2, [](void *pdata) {
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

        labelresult = ekg::label("0", ekg::dock::fill | ekg::dock::next);
        labelresult->set_scaled_height(4);
        labelresult->set_text_align(ekg::dock::right | ekg::dock::bottom);
        labelresult->set_font_size(ekg::font::big);
        labelresult->set_tag("calculator-result");

        auto buselesstop1 = ekg::button("", ekg::dock::fill | ekg::dock::next);
        buselesstop1->set_scaled_height(2);
        buselesstop1->set_text_align(ekg::dock::center);

        auto buselesstop2 = ekg::button("", ekg::dock::fill);
        buselesstop2->set_scaled_height(2);
        buselesstop2->set_text_align(ekg::dock::center);

        auto bcls = ekg::button("cls", ekg::dock::fill);
        bcls->set_scaled_height(2);
        bcls->set_text_align(ekg::dock::center);
        bcls->set_tag("calculator-cls");

        auto berase = ekg::button("<=", ekg::dock::fill);
        berase->set_scaled_height(2);
        berase->set_text_align(ekg::dock::center);
        berase->set_tag("calculator-erase");

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
        bmultiply->set_tag("calculator-multiply");

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
        bsubtract->set_tag("calculator-subtract");

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
        baddition->set_tag("calculator-addition");

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
        bassign->set_tag("calculator-assign");
        ekg::scroll("pompom-calc");
        ekg::popgroup();

        framedebug->add_child(ekg::frame("helko", {300, 300}, ekg::dock::next)->get_id());
        ekg::button("hello", ekg::dock::fill);
        ekg::button("hello vc é lindo", ekg::dock::next);
        ekg::popgroup();
    }

    framedebug->add_child(ekg::scroll("mewoscroll")->get_id());

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
            debuglabel->set_text("Debug FPS: " + std::to_string(ticked_frames));
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
                    //framedebug->set_height(ekg::display::height);

                    if (ekg::listen(event, sdl_event)) {
                        if (event.tag == "calculator-assign") {
                            labelresult->set_text(resultcalc(labelresult->get_text()));
                        } else if (event.tag == "calculator-cls") {
                            labelresult->set_text("0");
                        } else if (event.tag == "calculator-addition") {
                            labelresult->set_text(checkcalc(labelresult->get_text(), "+"));
                        } else if (event.tag == "calculator-subtract") {
                            labelresult->set_text(checkcalc(labelresult->get_text(), "-"));
                        } else if (event.tag == "calculator-multiply") {
                            labelresult->set_text(checkcalc(labelresult->get_text(), "*"));
                        } else if (event.tag == "calculator-erase") {
                            std::string text {labelresult->get_text()};
                            if (text.size() <= 1) {
                                text = "0";
                            } else {
                                text = text.substr(0, text.size() - 1);
                            }

                            labelresult->set_text(text);
                        } else {
                            std::string number {};
                            for (uint32_t i {}; i < 10; i++) {
                                number = std::to_string(i);
                                if (event.tag == number) {
                                    labelresult->set_text(labelresult->get_text() == "0" ? number : labelresult->get_text() + number);
                                    break;
                                }
                            }
                        }
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

        framedebug->set_width(ekg::display::width);

        if (ekg::log::buffering()) {
            std::string oldlog {textboxdebug->get_text()};
            if (oldlog.size() > 50000) oldlog = "";
            textboxdebug->set_text(oldlog + ekg::log::cache);
            ekg::log::flush();
        }

        ekg::update();

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        ekg::render();

        // Count the FPS.
        ticked_frames++;

        // Swap buffers.
        SDL_GL_SwapWindow(sdl_win);
        //SDL_Delay(16);
    }

    ekg::quit();

    return 0;
}