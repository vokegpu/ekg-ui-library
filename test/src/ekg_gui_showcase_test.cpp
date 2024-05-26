#include <ekg/ekg.hpp>
#include <ekg/os/ekg_sdl.hpp>
#include <ekg/os/ekg_opengl.hpp>
#include "application.hpp"

//#define application_enable_stb_image_test
#ifdef application_enable_stb_image_test
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#endif

application app {};

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

int32_t showcase_useless_window() {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
  SDL_GL_SetSwapInterval((app.vsync = true));

  app.p_sdl_win = {
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
  SDL_GLContext sdl_gl_context {SDL_GL_CreateContext(app.p_sdl_win)};

  glewExperimental = GL_TRUE;
  glewInit();

  ekg::runtime_property ekg_runtime_property {
    .p_font_path = "JetBrainsMono-Bold.ttf",
    .p_font_path_emoji = "NotoColorEmoji.ttf",
    .p_gpu_api = new ekg::os::opengl(),
    .p_os_platform = new ekg::os::sdl(app.p_sdl_win)
  };

  ekg::runtime runtime {};
  ekg::init(
    &runtime,
    &ekg_runtime_property
  );

  ekg::ui::label *labelresult {};
  ekg::ui::label *fps {};
  std::string previous_operator {};

  #ifdef application_enable_stb_image_test
  
  ekg::gpu::sampler_allocate_info gato_all_info {
    .p_tag = "./joao_das_galaxias_cat.png",
  };

  gato_all_info.p_data = {
    stbi_load(
      gato_all_info.p_tag,
      &gato_all_info.w,
      &gato_all_info.h,
      &gato_all_info.gl_internal_format,
      0
    )
  };

  if (!gato_all_info.p_data) {
    ekg::log() << "failed to meow";
  }

  gato_all_info.gl_wrap_modes[0] = GL_CLAMP;
  gato_all_info.gl_wrap_modes[1] = GL_CLAMP;
  gato_all_info.gl_parameter_filter[0] = GL_LINEAR;
  gato_all_info.gl_parameter_filter[1] = GL_LINEAR;
  gato_all_info.gl_internal_format = GL_RGB;
  gato_all_info.gl_format = GL_RGBA;
  gato_all_info.gl_format = GL_UNSIGNED_BYTE;

  ekg::gpu::sampler_t gato {};
  auto result = ekg::allocate_sampler(
    &gato_all_info,
    &gato
  );

  ekg::log() << gato_all_info.p_tag
             << " status: " << result
             << "; w, h, c [" << gato.w << ", " << gato.h << ", " << gato_all_info.gl_internal_format << "]";

  auto p_gato_frame = ekg::frame("foto-de-gato-fofo-amo-vc", {400, 400}, ekg::dock::none)
    ->set_resize(ekg::dock::right | ekg::dock::bottom | ekg::dock::left)
    ->set_drag(ekg::dock::top)
    ->set_layer(&gato, ekg::layer::background);

  auto p_gato_layer = p_gato_frame->get_layer(ekg::layer::background);
  ekg::log() << "MEOW: " << p_gato_layer->p_tag;

  ekg::pop_group();
  #endif

  ekg::frame("mumu", {900, 300}, ekg::dock::none)
    ->set_resize(ekg::dock::right | ekg::dock::bottom | ekg::dock::left)
    ->set_drag(ekg::dock::top);

  ekg::label("meow", ekg::dock::fill);

  auto p_terminal = ekg::textbox("meow", "meow oi", ekg::dock::fill | ekg::dock::next)
    ->set_scaled_height(24);

  ekg::pop_group();

  ekg::frame("cat", {400, 700}, ekg::dock::none)
    ->set_resize(ekg::dock::right | ekg::dock::bottom | ekg::dock::left)
    ->set_drag(ekg::dock::top);

  ekg::button("Oi Quica Aqui pa Sair Mumu", ekg::dock::fill | ekg::dock::next)
    ->set_text_align(ekg::dock::center)
    ->set_font_size(ekg::font::big)
    ->set_task(
      new ekg::task {
        .info = {
          .tag = "oi bu"
        },
        .function = [](ekg::info &task_info) {
          SDL_Event sdl_event_quit {};
          sdl_event_quit.type = SDL_QUIT;
          SDL_PushEvent(&sdl_event_quit);

          ekg::log() << "task executed: " << task_info.tag;
        }
      },
      ekg::action::activity
    );

  fps = ekg::label("FPS: ", ekg::dock::fill | ekg::dock::next)
    ->set_font_size(ekg::font::big);

  auto list = ekg::listbox(
    "hello",
    {
      ekg::item(
        "Nome",
        {
          ekg::item("Anjo-Dorminhoco"),
          ekg::item("Potato"),
          ekg::item("Astah", {
            ekg::item("meow"),
            ekg::item("meow")
          }),
          ekg::item("Malboro"),
          ekg::item("Leviata")
        },
        ekg::attr::disabled | ekg::attr::locked
      ),
      ekg::item(
        "Estado",
        {
          ekg::item("No Ceu"),
          ekg::item("Brincando la fora"),
          ekg::item("Mordendo rato"),
          ekg::item("Mordendo a Astah"),
          ekg::item("Correndo")
        }
      )
    },
    ekg::dock::fill | ekg::dock::next
  )->set_scaled_height(16);

  ekg::slider("gostoso", 500.0f, 0.0f, 1000.0f, ekg::dock::fill | ekg::dock::next);

  auto p_dpi = ekg::checkbox("DPI-scale:", true, ekg::dock::next);
  ekg::textbox("DPI", "1920x1080", ekg::dock::fill)
    ->set_max_lines(1)
    ->set_task(
      new ekg::task {
        .info = {
          .tag = "DPI-scale"
        },
        .function = [](ekg::info &task_info) {
          ekg::ui::textbox *p_ui {static_cast<ekg::ui::textbox*>(task_info.p_ui)};
          std::string &text {p_ui->at(0)};

          bool found_x {};
          uint64_t begin {};
          uint64_t size {text.size()};

          for (uint64_t it {}; it < text.size(); it++) {
            char &c {text.at(it)};

            if (c == 'x') {
              begin = it;
              ekg::ui::scale.x = std::stof(text.substr(0, begin)); 
              found_x = true;
            }

            if (it == size - 1) {
              ekg::ui::scale.y = std::stof(text.substr(begin+1, it - begin));
            }
          }

          ekg::log() << "updated DPI";
        }
      },
      ekg::action::activity
    );

  ekg::theme().gen_default_dark_theme();
  ekg::checkbox("Light Theme", false, ekg::dock::fill | ekg::dock::next)
    ->set_task(
      new ekg::task {
        .info = {
          .tag = "theme-switcher",
        },
        .function = [](ekg::info &task_info) {
          auto &theme {ekg::theme()};
          std::string current_theme_name {theme.get_current_theme_name()};

          if (current_theme_name == "dark") {
            theme.gen_default_light_theme();
          } else if (current_theme_name == "light") {
            theme.gen_default_dark_theme();
          }
        }
      },
      ekg::action::activity
    );

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

  ekg::scrollbar("pompom-calc");
  ekg::pop_group();

  ekg::input::bind("hiroodrop", {"lctrl+b", "lctrl+lshift+v", "lshift+m"});
  ekg::input::bind("meow", "lctrl+mouse-1");

  bool running {true};
  uint64_t now {};
  uint64_t last {};
  uint64_t life {};

  SDL_Delay(10);
  
  float performance_frequency {};
  float dt {};

  uint64_t frame_couting {};
  uint64_t last_frame {1};
  ekg::timing fps_timing {};

  while (running) {
    last = now;
    now = SDL_GetPerformanceCounter();
    ekg::ui::dt = static_cast<float>(
      (now - last) * 1000 / SDL_GetPerformanceFrequency()
    ) * 0.01f;

    if (ekg::reach(fps_timing, 1000) && ekg::reset(fps_timing)) {
      last_frame = frame_couting;
      fps->set_text(
        "FPS: " + std::to_string(frame_couting) +
        " DT: " + std::to_string(ekg::ui::dt) +
        " GD: " + std::to_string(ekg::gpu::allocator::current_rendering_data_count)
      );

      ekg::ui::auto_scale = p_dpi->get_value();
      frame_couting = 0;
    }

    while (SDL_PollEvent(&sdl_event)) {
      ekg::os::sdl_poll_event(sdl_event);
    
      switch (sdl_event.type) {
      case SDL_QUIT: {
        running = false;
        break;
      }
      /*
      default: {
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
      */
      }
    }

    if (ekg::input::action("meow")) {
      std::cout << "mumu sou uma vakinha (" << ++life << ')' << std::endl;
    }
    
    if (ekg::log::buffered) {
      if (p_terminal->size() >= 100000) {
        p_terminal->erase(p_terminal->begin(), p_terminal->end() - 10000);
      }

      std::vector<std::string> &vec {*static_cast<std::vector<std::string>*>(p_terminal)};
      ekg::utf_decode(ekg::log::buffer.str(), vec);
    }

    ekg::update();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glViewport(0.0f, 0.0f, ekg::ui::width, ekg::ui::height);

    ekg::render();

    frame_couting++;

    SDL_GL_SwapWindow(app.p_sdl_win);
    if (app.vsync) {
      SDL_Delay(6);
    }
  }

  return 6666;
}

void test_out_of_context_uis() {
  std::vector<ekg::ui::label> label_list {
    ekg::ui::label(),
    ekg::ui::label(),
    ekg::ui::label(),
    ekg::ui::label(),
    ekg::ui::label(),
    ekg::ui::label(),
    ekg::ui::label()
  }; 

  std::string_view minecraft {"ç"};
  std::string_view listbox_can_open {"▶"};
  std::string_view listbox_can_close {"▼"};

  std::cout << "listbox: " << ekg::utf_string_to_char32(minecraft) << ", " << ekg::utf_string_to_char32(listbox_can_close) << std::endl;

  for (ekg::ui::label &label : label_list) {
    label.set_text("oi meow");
    ///std::cout << label.get_text() << std::endl;
  }

  std::string vakinha_mumu {
    "meow\nmumu\n"
    "vakinhas gostam de bombom\n"
    "gatinhos de leite"
  };

  std::string_view vacas {"字"};
  std::cout << "vacas: " << ekg::utf_string_to_char32(vacas) << std::endl;

  std::vector<std::string> oi_eu_amo_vacas {};
  ekg::utf_decode(vakinha_mumu, oi_eu_amo_vacas);

  for (std::string &mumu : oi_eu_amo_vacas) {
    std::cout << mumu << std::endl;
  }
}

int32_t font_rendering_dynamic_batching() {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
  SDL_GL_SetSwapInterval((app.vsync = true));

  app.p_sdl_win = {
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
  SDL_GLContext sdl_gl_context {SDL_GL_CreateContext(app.p_sdl_win)};

  glewExperimental = GL_TRUE;
  glewInit();

  ekg::runtime_property ekg_runtime_property {
    .p_font_path = "whitneybook.otf",
    .p_gpu_api = new ekg::os::opengl(),
    .p_os_platform = new ekg::os::sdl(app.p_sdl_win)
  };

  ekg::runtime runtime {};
  ekg::init(
    &runtime,
    &ekg_runtime_property
  );

  bool running {true};
  uint64_t now {};
  uint64_t last {};
  uint64_t life {};

  SDL_Delay(10);
  
  float performance_frequency {};
  float dt {};

  uint64_t frame_couting {};
  uint64_t last_frame {1};
  ekg::timing fps_timing {};

  while (running) {
    last = now;
    now = SDL_GetPerformanceCounter();
    ekg::ui::dt = static_cast<float>(
      (now - last) * 1000 / SDL_GetPerformanceFrequency()
    ) * 0.01f;

    if (ekg::reach(fps_timing, 1000) && ekg::reset(fps_timing)) {
      last_frame = frame_couting;
      frame_couting = 0;
    }

    while (SDL_PollEvent(&sdl_event)) {
      ekg::os::sdl_poll_event(sdl_event);
    
      switch (sdl_event.type) {
      case SDL_QUIT:
        running = false;
        break;
      }
    }

    ekg::update();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glViewport(0.0f, 0.0f, ekg::ui::width, ekg::ui::height);

    ekg::render();

    frame_couting++;

    SDL_GL_SwapWindow(app.p_sdl_win);
    if (app.vsync) {
      SDL_Delay(6);
    }
  }

  return 0;
}

int32_t main(int32_t, char**) {
  test_out_of_context_uis();
  return showcase_useless_window();
}