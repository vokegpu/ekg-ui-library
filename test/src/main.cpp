/*
 * MIT License
 * 
 * Copyright (c) 2022-2023 Rina Wilk / vokegpu@gmail.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <ekg/ekg.hpp>

#define SOUGOSTOSA

ekg::rect window_size {0, 0, 1280, 720};
bool running {true};

void create_exit_button() {
    ekg::frame("oi wuria ser linda", {window_size.w / 2, window_size.h - 50}, {100, 40});
    ekg::button("Exit", ekg::dock::fill | ekg::dock::next)->set_callback(new ekg::task {
        .p_tag = "exit-callback",
        .p_callback = &running,
        .function = [](void *p_data) {
        bool *p_running {static_cast<bool*>(p_data)};
        *p_running = false;
    }});;

    ekg::pop_group();

    ekg::log() << window_size.w / 2 << window_size.h - 50 + 20;
}

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

template<typename t>
struct linkedlist {
public:
    template<typename s>
    struct node {
    public:
        node<s> *p_next {};
        s value {};
    };
protected:
    linkedlist::node<t> *p_node_list {};
    uint64_t size {};
public:
    explicit linkedlist() {
        this->p_node_list = new linkedlist::node<t>();
    }

    ~linkedlist() {
        delete this->p_node_list;
    }

    void push_back(const t &copy) {
        this->p_node_list[this->size] = {};
        this->p_node_list[this->size].value = copy;
        this->size++;
    }

    t &operator[](uint64_t it) {
        return this->p_node_list[this->size].value;
    }
};

static uint64_t framerate {};

template<typename t, typename s>
t ref(s _ref) {
    return static_cast<s>(_ref);
}

int32_t main_example_with_no_ekg() {
    int32_t root_width {1280};
    int32_t root_height {720};

    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_Window* sdl_win {SDL_CreateWindow("Pompom Calculator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, root_width, root_height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL)};
    SDL_GLContext sdl_gl_context {SDL_GL_CreateContext(sdl_win)};

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        ekg::log() << "Error: Failed to initialise GLEW";
    } else {
        ekg::log() << "GLEW initialised";
    }

    ekg::runtime ekg_runtime {};

    ekg::log() << "OpenGL context created";
    ekg::debug = true;

    SDL_GL_SetSwapInterval(1);
    SDL_Event sdl_event {};

    while (running) {
        while (SDL_PollEvent(&sdl_event)) {
            if (sdl_event.type == SDL_QUIT) {
                running = false;
            }
        }

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glClearColor(1.0f, 0.69f, 1.0f, 1.0f);

        // Swap buffers.
        SDL_GL_SwapWindow(sdl_win);
        SDL_Delay(16);
    }

    return 0;
}


int32_t main_example() {
    int32_t root_width {1280};
    int32_t root_height {720};

    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    SDL_Window* sdl_win {SDL_CreateWindow("Pompom Calculator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, root_width, root_height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL)};
    SDL_GLContext sdl_gl_context {SDL_GL_CreateContext(sdl_win)};

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        ekg::log() << "Error: Failed to initialise GLEW";
    } else {
        ekg::log() << "GLEW initialised";
    }

    ekg::autoscale = false;
    ekg::scalebase = {1280, 720};
    ekg::pre_decode_clipboard = true;

    ekg::log() << "oi queria ser MUIUTO gostosa\t" << ((uint64_t) 0 - (uint64_t) 0);

    ekg::runtime ekg_runtime {};
    ekg::init(&ekg_runtime, sdl_win, "whitneybook.otf");

    ekg::log() << "OpenGL context created";
    ekg::debug = true;

    SDL_GL_SetSwapInterval(1);
    SDL_Event sdl_event {};

    auto frame = ekg::frame("lê rect", {20, 20}, {400, 400});

    frame->set_drag(ekg::dock::top);
    frame->set_resize(ekg::dock::left | ekg::dock::right | ekg::dock::bottom);

    ekg::button("pom pom clcik click!!", ekg::dock::fill);
    ekg::slider("pompo number!!", 0.34f, 0.11f, 0.934f, ekg::dock::fill | ekg::dock::next)->set_precision(4);

    auto p_textbox = ekg::textbox("Le textbox", "", ekg::dock::next)->set_scaled_height(6);
    p_textbox->set_text("oii");

    ekg::label("RGB:", ekg::dock::next);

    auto r = ekg::slider("red", 1.0f, 0.0f, 1.0f, ekg::dock::fill)->set_precision(2);
    auto g = ekg::slider("green", 0.69f, 0.0f, 1.0f, ekg::dock::fill)->set_precision(2);
    auto b = ekg::slider("blue", 1.0f, 0.0f, 1.0f, ekg::dock::fill)->set_precision(2);

    ekg::scroll("scroll minecraft");
    ekg::pop_group();

    create_exit_button();

    std::string oi_cachorroo = "oi cachorror";
    p_textbox->set_text(oi_cachorroo);

    while (running) {
        while (SDL_PollEvent(&sdl_event)) {
            if (sdl_event.type == SDL_QUIT) {
                running = false;
            }

            ekg::poll_event(sdl_event);

            if (ekg::input::released() && ekg::input::receive("mouse-3-up")) {
                auto main = ekg::popup("file", {
                    "\tAdd",
                    "Cut",
                    "Copy",
                    "\tPaste",
                    "\tSelect All",
                    "Actions"
                });
  
                if (main) {
                    auto three = ekg::popup("file-add", {"Cube", "Plane", "Sphere", "Hexagon", "Hexagon"});
                    auto hexagon = ekg::popup("file-add-hexagon", {"Tree D", "Plane", "Double Pairs Daggers"});
                    auto game = ekg::popup("file-actions", {"Reload Clock", "Flush"});

                    main->link("Add", three);
                    main->link("Actions", game);
                    three->link("Hexagon", hexagon);
                }
            }
        }

        ekg::display::dt = 0.016f;
        ekg::update();

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glClearColor(r->get_value(), g->get_value(), b->get_value(), 1.0f);

        ekg::render();

        // Swap buffers.
        SDL_GL_SwapWindow(sdl_win);
        SDL_Delay(16);
    }

    return 0;
}

/*
 * Created by Rina.
 */
int32_t main_calculator() {
    std::string utf8pompom {"𒁑𒁕𒈢𒋟𒈙"};
    std::string le {"宇"};

    ekg::log() << le << " utf8 size: " << ekg::utf_length(le) << " string size: " << le.size();
    ekg::log() << "EKG User interface library demo starting";

    std::string something_ascii {"qrsergostosaterumbumbumlindo"};
    std::string something_utf8 {"qrsergostósaterumbumbumlindo"};
    ekg::log() << '\n' << "(EKG utf8) " << ekg::utf_substr(something_utf8, 2, 40) << "\n substr vs \n(STL ascii) " << something_ascii.substr(2, 40);

    int32_t root_width {1280};
    int32_t root_height {720};

    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    SDL_Window* sdl_win {SDL_CreateWindow("Pompom Calculator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, root_width, root_height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL)};
    SDL_GLContext sdl_gl_context {SDL_GL_CreateContext(sdl_win)};

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        ekg::log() << "Error: Failed to initialise GLEW";
    } else {
        ekg::log() << "GLEW initialised";
    }

    SDL_GL_SetSwapInterval(1); // v-sync on
    framerate = 16;

    ekg::runtime ekg_runtime {};
    ekg::init(&ekg_runtime, sdl_win, "whitneybook.otf");
    ekg::log() << "OpenGL context created";

    ekg::autoscale = true;
    ekg::scalebase = {1920.0f, 1080.0f};
    ekg::debug = true;

    ekg::timing mainloop_timing {};
    ekg::timing fps_timing {};
    SDL_Event sdl_event {};

    uint64_t fps {60};
    uint64_t fps_ms_interval {1000 / fps};
    uint64_t display_fps {};
    uint64_t ticked_frames {};

    auto f1 = ekg::frame("sou gostosa", {700, 300}, {200, 200})->set_drag(ekg::dock::top)->set_resize(ekg::dock::bottom | ekg::dock::left | ekg::dock::right);

    ekg::button("swap locked-framerate")->set_callback(new ekg::task {"theme-switcher", nullptr, [](void *pdata) {
        framerate = framerate == 0 ? 16 : 0;
        SDL_GL_SetSwapInterval(framerate ? 1 : 0);
    }});

    ekg::label("oioioioioio", ekg::dock::fill);
    auto ff = ekg::frame("frame-debug", {200, 200}, ekg::dock::fill | ekg::dock::next);
    ff->set_resize(ekg::dock::right | ekg::dock::bottom);
    ff->set_height(300.0f);

    auto ff3 = ekg::textbox("frame-debug", "oi", ekg::dock::fill | ekg::dock::next);
    ff3->set_scaled_height(16);
    ekg::checkbox("Auto-scale", true, ekg::dock::fill | ekg::dock::next)->set_tag("base.resolution.autoscale");
    ekg::label("Aspect:", ekg::dock::fill | ekg::dock::next);
    ekg::slider("base.resolution.width", 1920.0f, 800.0f, 2560.0f, ekg::dock::fill);
    ekg::slider("base.resolution.height", 1080.0f, 600.0f, 1440.0f, ekg::dock::fill);
    ff->add_child(ekg::scroll("hello-sou-gostosa")->get_id());
    f1->add_child(ff->get_id());
    f1->add_child(ekg::scroll("hello-sou-gostosa")->get_id());

    auto framedebug = ekg::frame("frame-debug", {0, 0}, {400, root_height});
    framedebug->set_resize(ekg::dock::right | ekg::dock::bottom);
    framedebug->set_drag(ekg::dock::top);

    auto debuglabel = ekg::label("Debug:", ekg::dock::next);
    debuglabel->set_text_align(ekg::dock::left | ekg::dock::top);
    auto textboxdebug = ekg::textbox("textbox-debug", "", ekg::dock::next);
    textboxdebug->set_scaled_height(6);
    textboxdebug->set_enabled(false);
    textboxdebug->set_width(600.0f);

    ekg::ui::label *labelresult {};
    std::string previous_operator {};
    ekg::ui::frame *frame2 {};

    for (int32_t it {}; it < 1; it++) {
        auto frame1 = ekg::frame("cat", {400, 400}, ekg::dock::none);
        frame1->set_resize(ekg::dock::right | ekg::dock::bottom | ekg::dock::left);

        ekg::checkbox("meow enabled", false, ekg::dock::next);
        ekg::checkbox("meow enabled", false, ekg::dock::next);
        ekg::checkbox("meow enabled", false);
        ekg::checkbox("meow enabled", false, ekg::dock::next);

        // @TODO remove text from UI slider

        auto slider = ekg::slider("gato", 0.0f, 0.0f, 1000000.0f, ekg::dock::fill);
        slider->set_text_align(ekg::dock::center);
        slider->set_precision(23);
        ekg::checkbox("meow enabled", false, ekg::dock::fill | ekg::dock::next);
        ekg::scroll("pompom-meow");

        framedebug->add_child(frame1->get_id());
        frame2 = ekg::frame("cha", {400, 400}, ekg::dock::none);
        frame2->set_resize(ekg::dock::right | ekg::dock::bottom | ekg::dock::left);
        framedebug->add_child(frame2->get_id());

        ekg::theme().gen_default_dark_theme();
        ekg::checkbox("Light Theme", false, ekg::dock::fill | ekg::dock::next)->set_callback(new ekg::task {"theme-switcher", frame2, [](void *pdata) {
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
        ekg::pop_group();

        framedebug->add_child(ekg::frame("helko", {300, 300}, ekg::dock::next)->get_id());
        ekg::button("hello", ekg::dock::fill);
        ekg::button("hello vc é lindo", ekg::dock::next);
        ekg::pop_group();
    }

    framedebug->add_child(ekg::scroll("mewoscroll")->get_id());

    // ekg::button("Button 7", ekg::dock::right);
    // ekg::button("Button 8", ekg::dock::fill | ekg::dock::bottom);

    uint64_t cpu_now_ticks {};
    uint64_t cpu_last_ticks {};
    ekg::event event {};

    ekg::input::bind("pompom", "c-up");
    ekg::input::bind("pompom", "rshift");

    glDisable(GL_DEPTH_TEST);

    /*
     * Mainloop.
     */
    while (running) {
        if (ekg::reach(fps_timing, 1000) && ekg::reset(fps_timing)) {
            display_fps = ticked_frames;
            ticked_frames = 0;
        }

        cpu_last_ticks = cpu_now_ticks;
        cpu_now_ticks = SDL_GetPerformanceCounter();
        ekg::display::dt = static_cast<float>(cpu_now_ticks - cpu_last_ticks) / static_cast<float>(SDL_GetPerformanceFrequency());

        debuglabel->set_text("FPS: " + std::to_string(display_fps));

        while (SDL_PollEvent(&sdl_event)) {
            switch (sdl_event.type) {
                case SDL_QUIT: {
                    running = false;
                    break;
                }

                default: {
                    //framedebug->set_height(ekg::display::height);

                    if (ekg::listen(event, sdl_event)) {
                        if (event.tag == "file") {
                            if (event.value == "Copy") {
                                ekg::input::fire("clipboard-copy");
                            } else if (event.value == "Cut") {
                                ekg::input::fire("clipboard-cut");
                            } else if (event.value == "Paste") {
                                ekg::input::fire("clipboard-paste");
                            }
                        } else if (event.tag == "base.resolution.width") {
                            ekg::scalebase.x = std::stof(event.value);
                        } else if (event.tag == "base.resolution.height") {
                            ekg::scalebase.y = std::stof(event.value);
                        } else if (event.tag == "base.resolution.autoscale") {
                            ekg::autoscale = event.value == "checked";
                        }

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

                    ekg::poll_event(sdl_event);

                    if (ekg::input::released() && ekg::input::receive("mouse-3-up")) {
                        auto main = ekg::popup("file", {"---Add", "Cut", "Copy", "---Paste", "---Select All", "Actions"});
                        auto three = ekg::popup("file-add", {"Cube", "Plane", "Sphere", "Hexagon", "Hexagon"});
                        auto hexagon = ekg::popup("file-add-hexagon", {"Tree D", "Plane", "Double Pairs Daggers"});
                        auto game = ekg::popup("file-actions", {"Reload Clock", "Flush"});

                        if (main) {
                            main->link("Add", three);
                            main->link("Actions", game);
                            three->link("Hexagon", hexagon);
                        }
                    }

                    if (ekg::input::action("pompom")) {
                        ekg::log() << "VASCO GAMMA E JOAO É GOSTOSAA";
                    }

                    break;
                }
            }
        }

        // framedebug->set_size(ekg::display::width, ekg::display::height);

        if (ekg::log::buffered) {
            std::string old_log {textboxdebug->get_text()};
            if (old_log.size() > 50000) old_log = '\n';
            old_log += ekg::log::buffer.str();
            textboxdebug->set_text(old_log);
        }

        ekg::update();

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        ekg::render();

        // Count the FPS.
        ticked_frames++;

        // Swap buffers.
        if (framerate) {
            SDL_GL_SwapWindow(sdl_win);
            SDL_Delay(framerate);
        } else {
            SDL_GL_SwapWindow(sdl_win);
        }
    }

    if (!ekg::swap::tooktimeanalyzingtelemtry.empty()) {
        uint64_t sum {};
        for (uint64_t &took_time : ekg::swap::tooktimeanalyzingtelemtry) {
            sum += took_time;
        }

        ekg::log() << "(telemetry size, sum) (average timing): " << ekg::swap::tooktimeanalyzingtelemtry.size() << ", " << sum << '\t' << (sum / ekg::swap::tooktimeanalyzingtelemtry.size());
    }

    ekg::quit();

    return 0;
}

#include <thread>
#include <mutex>

uint64_t ticking_sleep {};
std::vector<std::string> task_list {};

void parallel_runtime() {
    while (running) {    
        for (std::string &tasks : task_list) {
            std::cout << tasks << std::endl;
        }

        task_list.clear();
        ticking_sleep = 100;
        std::this_thread::sleep_for(std::chrono::milliseconds(ticking_sleep));
    }
}

int32_t main_thread_system() {
    for (uint32_t it {}; it < 256; it++) {
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
        task_list.emplace_back("oiii fofa"); task_list.emplace_back("fofooo"); task_list.emplace_back("gatinho e cachorro");
    }

    ticking_sleep = 600;
    std::thread thread_fofa(parallel_runtime);

    while (running) {
        ticking_sleep = 0;

        std::this_thread::sleep_for(std::chrono::seconds(1));
        running = false;
    }

    std::flush(std::cout);
    ticking_sleep = 0;
    thread_fofa.join();
    return 0;
}

void utf_test() {
    std::string utf_text {
        "ççç ççç"
    };

    for (uint64_t it {}; it < utf_text.size(); it++) {
        uint8_t char8 {static_cast<uint8_t>(utf_text.at(it))};
        char32_t char32 {};
        std::string utf_char {};
        it += ekg::utf_check_sequence(char8, char32, utf_char, utf_text, it);
        std::cout << it << " joao(s)" << utf_char  << "\t" << std::endl;
    }
}

std::ostringstream log() {
    return {};
}

template<typename t>
std::ostringstream &operator+(std::ostringstream &os_string_stream, const t &log_content) {
    os_string_stream << log_content;
    return os_string_stream;
}

std::ostringstream &operator-(std::ostringstream &os_string_stream, std::ostringstream &os_string_stream_final) {
    std::cout << os_string_stream.str() << std::endl;
    return os_string_stream;
}

int32_t main(int32_t, char**) {
    ekg::item animals = {"animals"};

    animals.emplace_back() = "cats";
    std::cout << animals.at(0).value << std::endl;

    ekg::item &cats {animals.at(0)};
    cats.push_back("potato");
    std::cout << animals.at(0).value << '\t' << cats.at(0).value << std::endl;

    animals.push_back("dogs");
    std::cout << animals.at(1).value << std::endl;

    animals.insert({"humans", "monkeys"});
    std::cout << animals.at(2).value << std::endl;
    std::cout << animals.at(3).value << std::endl;

    std::cout << '\n' << std::endl;

    for (const ekg::item &items : animals) {
        std::cout << items.value << std::endl;
    }

    ekg::item mother {"animals"};
    mother.emplace_back() = "\t\\Select All";

    return main_example();
}