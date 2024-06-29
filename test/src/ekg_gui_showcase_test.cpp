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

laboratory::shape::shape() {
  std::string_view vsh {
    "#version 450\n"

    "layout (location = 0) in vec2 aPos;\n"
    "uniform mat4 uMatrixProjection;\n"
    "uniform vec4 uRect;\n"

    "void main() {\n"
      "gl_Position = uMatrixProjection * vec4((aPos * uRect.zw) + uRect.xy, 0.0f, 1.0f);\n"
    "}\n"
  };

  std::string_view fsh {
    "#version 450\n"

    "layout (location = 0) out vec4 aFragColor;\n"
    "uniform vec4 uColor;\n"

    "void main() {\n"
      "aFragColor = uColor;\n"
    "}\n"
  };

  this->program = glCreateProgram();
  ekg::os::opengl *p_opengl_api {ekg::os::get_opengl()};

  p_opengl_api->create_pipeline_program(
    this->program,
    {
      {vsh, GL_VERTEX_SHADER},
      {fsh, GL_FRAGMENT_SHADER}
    }
  );

  glGenVertexArrays(1, &this->buffer_collection);
  glGenBuffers(1, &this->gbuffer);

  glBindVertexArray(this->buffer_collection);
  glBindBuffer(GL_ARRAY_BUFFER, this->gbuffer);

  float geometry[12] {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f
  };

  glBufferData(GL_ARRAY_BUFFER, sizeof(geometry), geometry, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // end the collection
  glBindVertexArray(0);

  this->uniform_matrix_projection = glGetUniformLocation(this->program, "uMatrixProjection");
  this->uniform_rect = glGetUniformLocation(this->program, "uRect");
  this->uniform_color = glGetUniformLocation(this->program, "uColor");

  std::cout << "[laboratory] initialized arbitrary shapee meow ><" << std::endl;
}

void laboratory::shape::invoke() {
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glUseProgram(this->program);
  glBindVertexArray(this->buffer_collection);
}

void laboratory::shape::revoke() {
  glDisable(GL_BLEND);
  glBindVertexArray(0);
  glUseProgram(0);
}

void laboratory::shape::on_resize() {
  ekg::ortho(
    this->mat4x4_projection,
    0.0f,
    static_cast<float>(ekg::ui::width),
    static_cast<float>(ekg::ui::height),
    0.0f
  );

  glProgramUniformMatrix4fv(
    this->program,
    this->uniform_matrix_projection,
    1,
    GL_FALSE,
    this->mat4x4_projection
  );

  std::cout << "[laboratory] arbitrary on_resize called meow >< " << std::endl;
}

void laboratory::shape::draw(const ekg::rect &rect, const ekg::vec4 &color) {
  glUniform4f(this->uniform_rect, rect.x, rect.y, rect.w, rect.h);
  glUniform4f(this->uniform_color, color.x, color.y, color.z, color.w);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

template<typename t>
bool create_sampler(
  std::string_view sampler_name,
  std::unordered_map<std::string_view, ekg::gpu::sampler_t> &loaded_sampler_map
) {
  #ifndef application_enable_stb_image_test
  return false;
  #else
  ekg::gpu::sampler_allocate_info sampler_alloc_info {
    .p_tag = sampler_name.data()
  };

  sampler_alloc_info.p_data = {
    static_cast<t>(
      stbi_load(
        sampler_name.data(),
        &sampler_alloc_info.w,
        &sampler_alloc_info.h,
        &sampler_alloc_info.gl_internal_format,
        0
      )
    )
  };

  if (!sampler_alloc_info.p_data) {
    ekg::log() << "Failed to load '" << sampler_name << "' sampler ><!";
    return false;
  }

  int32_t truly_png_or_no {GL_RGB};
  if (sampler_alloc_info.gl_internal_format == 4) {
    truly_png_or_no = GL_RGBA;
  }

  sampler_alloc_info.gl_wrap_modes[0] = GL_REPEAT;
  sampler_alloc_info.gl_wrap_modes[1] = GL_REPEAT;
  sampler_alloc_info.gl_parameter_filter[0] = GL_LINEAR;
  sampler_alloc_info.gl_parameter_filter[1] = GL_LINEAR;
  sampler_alloc_info.gl_internal_format = truly_png_or_no;
  sampler_alloc_info.gl_format = truly_png_or_no;
  sampler_alloc_info.gl_type = GL_UNSIGNED_BYTE;
  sampler_alloc_info.gl_generate_mipmap = GL_TRUE;

  ekg::gpu::sampler_t &sampler {loaded_sampler_map[sampler_name]};
  ekg::allocate_sampler(
    &sampler_alloc_info,
    &sampler
  );

  return true;
  #endif
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

bool load_ttf_emoji(ekg::gpu::sampler_t *p_sampler) {
  ekg::gpu::sampler_allocate_info sampler_alloc_info {
    .p_tag = "meow"
  };

  ekg::draw::font_renderer f_renderer {ekg::f_renderer(ekg::font::normal)};
  ekg::draw::font_face_t &typography_font_face {f_renderer.font_face_emoji};

  // 🏳️‍⚧️
  // 🐈
  // 🐮
  // 🤓

  FT_Load_Char(
    typography_font_face.ft_face,
    ekg::utf_string_to_char32("🐈"),
    FT_LOAD_RENDER | FT_LOAD_COLOR | FT_LOAD_DEFAULT
  );

  sampler_alloc_info.w = static_cast<int32_t>(typography_font_face.ft_face->glyph->bitmap.width);
  sampler_alloc_info.h = static_cast<int32_t>(typography_font_face.ft_face->glyph->bitmap.rows);

  sampler_alloc_info.gl_wrap_modes[0] = GL_REPEAT;
  sampler_alloc_info.gl_wrap_modes[1] = GL_REPEAT;
  sampler_alloc_info.gl_parameter_filter[0] = GL_LINEAR;
  sampler_alloc_info.gl_parameter_filter[1] = GL_LINEAR;
  sampler_alloc_info.gl_internal_format = GL_RGBA;
  sampler_alloc_info.gl_format = GL_BGRA;
  sampler_alloc_info.gl_type = GL_UNSIGNED_BYTE;
  //sampler_alloc_info.gl_generate_mipmap = GL_TRUE;
  sampler_alloc_info.p_data = typography_font_face.ft_face->glyph->bitmap.buffer;

  return ekg::allocate_sampler(
    &sampler_alloc_info,
    p_sampler
  );
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

  /**
   * For somee reason noto emoji does work :c
   **/

  ekg::runtime_property ekg_runtime_property {
    .p_font_path = "Arial.ttf",
    .p_font_path_emoji = "twemoji.ttf",
    .p_gpu_api = new ekg::os::opengl(),
    .p_os_platform = new ekg::os::sdl(app.p_sdl_win)
  };

  ekg::runtime runtime {};
  ekg::init(
    &runtime,
    &ekg_runtime_property
  );

  ekg::frame("tweaks-window", ekg::vec2(700, 200), ekg::vec2(200, 200))
    ->set_drag(ekg::dock::top)
    ->set_resize(ekg::dock::left | ekg::dock::bottom | ekg::dock::right);

  auto p_terminal = ekg::textbox("meow", "meow oi", ekg::dock::fill | ekg::dock::next)
    ->set_scaled_height(12);

  ekg::frame("tweaks-tob-bar", ekg::vec2(10.0f, 50.0f), ekg::dock::fill | ekg::dock::next)
    ->set_drag(ekg::dock::full)
    ->make_parent_top_level();

  ekg::button("-", ekg::dock::none);
  ekg::button("[]", ekg::dock::none);
  ekg::button("x", ekg::dock::none);
  ekg::pop_group_parent();

  ekg::frame("tweaks-content", ekg::vec2(700, 200), ekg::dock::fill | ekg::dock::next);
  ekg::label("UI internal tweaks", ekg::dock::fill | ekg::dock::next);

  auto p_app_vsync = ekg::checkbox("Application Vsync", app.vsync, ekg::dock::fill | ekg::next);

  p_app_vsync->transfer_ownership(&app.vsync);
  p_app_vsync->set_task(
    new ekg::task {
      .info = {
        .tag = "turn vsync"
      },
      .function = [](ekg::info &task_info) {
        auto p_ui = static_cast<ekg::ui::checkbox*>(task_info.p_ui);
        SDL_GL_SetSwapInterval(p_ui->get_value());
      }
    },
    ekg::action::activity
  );

  ekg::label("DPI tiling:", ekg::dock::next);
  auto p_dpi_tiling = ekg::slider("DPI tiling", 4.0f, 1.0f, 50.0f, ekg::dock::fill);
  p_dpi_tiling->transfer_ownership(&ekg::ui::dpi_tiling);

  ekg::scrollbar("scrollbar omg");
  ekg::pop_group();

  ekg::ui::label *labelresult {};
  ekg::ui::label *fps {};
  std::string previous_operator {};

  auto p_calculator_frame = ekg::frame("frame-cat", {400, 900}, ekg::dock::none)
    ->set_resize(ekg::dock::right | ekg::dock::bottom | ekg::dock::left)
    ->set_drag(ekg::dock::top);

  // 🏳️‍⚧️
  // 🐈
  // 🐮

  ekg::button("🏳️‍⚧️ 🐈 🐮 Oi Quica Aqui pa Sair Mumu", ekg::dock::fill | ekg::dock::next)
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

  ekg::item content = ekg::item {
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
        ekg::item("Mordendo rato", {
          ekg::item("meow"),
          ekg::item("meow")
        }),
        ekg::item("Mordendo a Astah"),
        ekg::item("Correndo")
      }
    ),
    ekg::item(
      "Cor",
      {
        ekg::item("Azul"),
        ekg::item("Azul"),
        ekg::item("Azul", {
          ekg::item("meow?"),
          ekg::item("meow?")
        }),
        ekg::item("Azul"),
        ekg::item("Azul")
      }
    ),
    ekg::item(
      "Cachorro",
      {
        ekg::item("Au"),
      ekg::item("A"),
        ekg::item("J", {
          ekg::item("K?"),
          ekg::item("L?")
        }),
        ekg::item("Meow"),
        ekg::item("Oi")
      }
    )
  };

  //content.erase(content.begin() + 1, content.end());

  auto list = ekg::listbox(
    "hello",
    content,
    ekg::dock::fill | ekg::dock::next
  )
  ->set_scaled_height(16)
  ->set_mode(ekg::mode::multicolumn);

  ekg::slider("meow1", 0.5f, 0.0f, 1.0f, ekg::dock::fill | ekg::dock::next)
    ->transfer_ownership(&ekg::theme().frame_background.w);

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
          std::string &text {p_ui->p_value->at(0)};

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

  ekg::gpu::sampler_t ttf_cow_sampler {};
  load_ttf_emoji(&ttf_cow_sampler);

  auto p_cow_ttf_frame = ekg::frame("meow", {200, 200}, {400, 400})
    ->set_drag(ekg::dock::full)
    ->set_resize(ekg::dock::left | ekg::dock::bottom | ekg::dock::right)
    ->set_layer(&ttf_cow_sampler, ekg::layer::background);

  p_terminal->set_layer(&ttf_cow_sampler, ekg::layer::background);
  p_terminal->set_layer(&ttf_cow_sampler, ekg::layer::highlight);

  #ifdef application_enable_stb_image_test

  std::unordered_map<std::string_view, ekg::gpu::sampler_t> loaded_sampler_map {};
  create_sampler<unsigned char*>("./cow.png", loaded_sampler_map);
  create_sampler<unsigned char*>("./joao_das_galaxias_cat.png", loaded_sampler_map);

  auto p_gato_frame = ekg::frame("foto-de-gato-fofo-amo-vc", {400, 400}, ekg::dock::none)
    ->set_resize(ekg::dock::right | ekg::dock::bottom | ekg::dock::left)
    ->set_drag(ekg::dock::top)
    ->set_layer(&loaded_sampler_map["./joao_das_galaxias_cat.png"], ekg::layer::background);

  p_calculator_frame->set_layer(&loaded_sampler_map["./cow.png"], ekg::layer::background);
  p_terminal->set_layer(&loaded_sampler_map["./joao_das_galaxias_cat.png"], ekg::layer::background);

  auto p_gato_layer = p_gato_frame->get_layer(ekg::layer::background);
  ekg::log() << "MEOW: " << p_gato_layer->p_tag;

  ekg::pop_group();
  #endif

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

  ekg::input::bind("amovc", "mouse-3");

  while (running) {
    last = now;
    now = SDL_GetPerformanceCounter();
    ekg::ui::dt = static_cast<float>(
      (now - last) * 1000 / SDL_GetPerformanceFrequency()
    ) * 0.01f;

    if (ekg::reach(fps_timing, 1000) && ekg::reset(fps_timing)) {
      last_frame = frame_couting;
      fps->set_value(
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
      case SDL_QUIT:
        running = false;
        break;
      }
    }

    if (ekg::input::action("amovc")) {
      std::cout << "mumu sou uma vakinha (" << ++life << ')' << std::endl;
    }
    
    if (ekg::log::buffered) {
      if (p_terminal->p_value->size() >= 100000) {
        p_terminal->p_value->erase(
          p_terminal->p_value->begin(),
          p_terminal->p_value->end() - 10000
        );
      }

      ekg::utf_decode(ekg::log::buffer.str(), p_terminal->get_value());
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
    //label.set_value("oi meow");
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

int32_t laboratory_testing() {
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

  ekg::theme().gen_default_dark_theme();

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

  float prev_pos {10.2f};
  float scale {2.0f};

  ekg::frame("m", {400, 400})
    ->set_drag(ekg::dock::full)
    ->set_resize(ekg::dock::left | ekg::dock::bottom | ekg::dock::right);
  
  ekg::button("Sample", ekg::dock::fill | ekg::dock::next);
  ekg::button("Sample", ekg::dock::fill | ekg::dock::next);
  ekg::button("Z", ekg::dock::fill | ekg::dock::next);
  ekg::button("A", ekg::dock::none)->set_width(50.0f);
  ekg::button("B", ekg::dock::fill);
  ekg::button("C", ekg::dock::fill);

  ekg::pop_group();

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

      if (sdl_event.type == SDL_QUIT) {
        running = false;
      }
    }

    ekg::update();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glViewport(0.0f, 0.0f, ekg::ui::width, ekg::ui::height);

    ekg::render();

    SDL_GL_SwapWindow(app.p_sdl_win);
    if (app.vsync) {
      SDL_Delay(6);
    }
  }

  return 0;
}

int32_t main(int32_t, char**) {
  return showcase_useless_window();
}