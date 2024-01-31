/**
 * MIT License
 * 
 * Copyright (c) 2022-2024 Rina Wilk / vokegpu@gmail.com
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

#include "ekg/ekg.hpp"
#include "ekg/os/platform.hpp"
#include "ekg/os/ekg_opengl.hpp"
#include "ekg/util/geometry.hpp"

ekg::runtime *ekg::core {};
std::string ekg::glsl_version {"#version 450"};
ekg::vec2 ekg::scalebase {1920.0f, 1080.0f};

bool ekg::autoscale {true};
float ekg::scaleinterval {25.0f};
bool ekg::pre_decode_clipboard {};

ekg::service::theme &ekg::theme() {
  return ekg::core->service_theme;
}

ekg::draw::font_renderer &ekg::f_renderer(ekg::font font_size) {
  switch (font_size) {
    case ekg::font::big: {
      return ekg::core->f_renderer_big;
    }

    case ekg::font::normal: {
      return ekg::core->f_renderer_normal;
    }

    case ekg::font::small: {
      return ekg::core->f_renderer_small;
    }
  }

  return ekg::core->f_renderer_normal;
}

void ekg::init(
  ekg::runtime *p_ekg_runtime,
  ekg::runtime_property *p_ekg_runtime_property
) {
  ekg::log() << "Initialising EKG";

  if (p_ekg_runtime_property->p_sdl_win != nullptr) {
    ekg::listener = SDL_RegisterEvents(1);
  
    /* Init OS cursor and check mouse icons. */

    ekg::init_cursor();
    ekg::log() << "Initialising OS cursor";

    SDL_GetWindowSize(p_root, &ekg::display::width, &ekg::display::height);
  }

  /* The runtime core, everything should be setup before the initialization process. */
  ekg::core = p_ekg_runtime;
  ekg::core->f_renderer_small.font_path = p_ekg_runtime_property->p_font_path;
  ekg::core->f_renderer_normal.font_path = p_ekg_runtime_property->p_font_path;
  ekg::core->f_renderer_big.font_path = p_ekg_runtime_property->p_font_path;
  ekg::core->root = p_ekg_runtime_property->p_sdl_win;
  ekg::core->init();

  /* First update of orthographic matrix and uniforms. */

  ekg::orthographic2d(
    ekg::gpu::allocator::mat4x4orthographic,
    0, 
    static_cast<float>(ekg::display::width),
    static_cast<float>(ekg::display::height),
    0
  );

  /* Output SDL info. */

  SDL_version sdl_version {};
  SDL_GetVersion(&sdl_version);

  ekg::log() << "SDL version: "
             << static_cast<int32_t>(sdl_version.major)
             << '.'
             << static_cast<int32_t>(sdl_version.minor)
             << '.'
             << static_cast<int32_t>(sdl_version.patch);

  ekg::log() << "Vendor details: ";
}

void ekg::quit() {
  ekg::core->quit();
  ekg::log() << "Shutdown complete - Thank you for using EKG ;) <3";
}

void ekg::poll_event(SDL_Event &sdl_event) {
  if (!(
      /*
       * Not all events should be processed, only the requireds.
       */
      sdl_event.type == SDL_MOUSEBUTTONDOWN || sdl_event.type == SDL_MOUSEBUTTONUP ||
      sdl_event.type == SDL_FINGERUP || sdl_event.type == SDL_FINGERDOWN ||
      sdl_event.type == SDL_FINGERMOTION || sdl_event.type == SDL_MOUSEMOTION ||
      sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_KEYUP ||
      sdl_event.type == SDL_WINDOWEVENT || sdl_event.type == SDL_MOUSEWHEEL ||
      sdl_event.type == SDL_TEXTINPUT
  )) {
    return;
  }

  switch (sdl_event.type) {
    case SDL_WINDOWEVENT: {
      switch (sdl_event.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED: {
          ekg::display::width = sdl_event.window.data1;
          ekg::display::height = sdl_event.window.data2;

          ekg::orthographic2d(
            ekg::gpu::allocator::mat4x4orthographic,
            0,
            static_cast<float>(ekg::display::width),
            static_cast<float>(ekg::display::height),
            0
          );

          ekg::core->p_gpu_api->update_viewport(ekg::display::width, ekg::display::height);
          ekg::core->update_size_changed();
          break;
        }
      }

      break;
    }
  }

  // Reset the cursor to the default type, then it is never glitch.
  ekg::cursor = ekg::system_cursor::arrow;
  ekg::core->process_event(sdl_event);
}

void ekg::update() {
  ekg::core->process_update();
  ekg::set_cursor(ekg::cursor);
}

void ekg::render() {
  // TODO: Fast multi-platform renderer allocator

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  ekg::core->process_render();
  glDisable(GL_BLEND);
}

ekg::ui::frame *ekg::frame(std::string_view tag, const ekg::vec2 &initial_position, const ekg::vec2 &size) {
  auto p_ui {new ekg::ui::frame()};
  p_ui->set_tag(tag);
  p_ui->set_type(ekg::type::frame);
  ekg::core->gen_widget(p_ui);

  p_ui->set_pos_initial(initial_position.x, initial_position.y);
  p_ui->set_size_initial(size.x, size.y);
  p_ui->ui() = {initial_position.x, initial_position.y, size.x, size.y};
  p_ui->set_place(ekg::dock::none);

  return p_ui;
}

ekg::ui::frame *ekg::frame(std::string_view tag, const ekg::vec2 &size, uint16_t dock) {
  auto p_ui {new ekg::ui::frame()};

  p_ui->set_tag(tag);
  p_ui->set_type(ekg::type::frame);
  ekg::core->gen_widget(p_ui);

  p_ui->set_size_initial(size.x, size.y);
  p_ui->ui() = {0.0f, 0.0f, size.x, size.y};
  p_ui->set_place(dock);

  return p_ui;
}

ekg::ui::button *ekg::button(std::string_view text, uint16_t dock) {
  auto p_ui {new ekg::ui::button()};
  p_ui->set_type(ekg::type::button);
  ekg::core->gen_widget(p_ui);

  p_ui->set_text(text);
  p_ui->set_place(dock);
  p_ui->set_scaled_height(1);
  p_ui->set_font_size(ekg::font::normal);
  p_ui->set_text_align(ekg::dock::left | ekg::dock::center);
  p_ui->set_tag(text);

  return p_ui;
}

ekg::ui::label *ekg::label(std::string_view text, uint16_t dock) {
  auto p_ui {new ekg::ui::label()};
  p_ui->set_type(ekg::type::label);
  ekg::core->gen_widget(p_ui);

  p_ui->set_text(text);
  p_ui->set_place(dock);
  p_ui->set_scaled_height(1);
  p_ui->set_font_size(ekg::font::normal);
  p_ui->set_text_align(ekg::dock::left | ekg::dock::center);
  p_ui->set_tag(text);

  return p_ui;
}

ekg::ui::checkbox *ekg::checkbox(std::string_view text, bool value, uint16_t dock) {
  auto p_ui {new ekg::ui::checkbox()};
  p_ui->set_type(ekg::type::checkbox);
  ekg::core->gen_widget(p_ui);

  p_ui->set_text(text);
  p_ui->set_place(dock);
  p_ui->set_scaled_height(1);
  p_ui->set_font_size(ekg::font::normal);
  p_ui->set_text_align(ekg::dock::left | ekg::dock::center);
  p_ui->set_box_align(ekg::dock::left | ekg::dock::center);
  p_ui->set_tag(text);
  p_ui->set_value(value);

  return p_ui;
}

ekg::ui::slider *ekg::slider(std::string_view tag, float val, float min, float max, uint16_t dock) {
  auto p_ui {new ekg::ui::slider()};
  p_ui->set_type(ekg::type::slider);
  ekg::core->gen_widget(p_ui);

  p_ui->set_tag(tag);
  p_ui->set_place(dock);
  p_ui->set_text_align(ekg::dock::center);
  p_ui->set_bar_align(ekg::dock::left | ekg::dock::center);
  p_ui->set_scaled_height(1);
  p_ui->set_font_size(ekg::font::normal);
  p_ui->set_value_min(min);
  p_ui->set_value_max(max);
  p_ui->set_precision(0);
  p_ui->set_bar_axis(ekg::axis::horizontal);
  p_ui->set_width(200);
  p_ui->set_value(val);

  return p_ui;
}

ekg::ui::popup *
ekg::popup(std::string_view tag, const std::vector<std::string> &component_list, bool interact_position) {
  if (ekg::hovered::type == ekg::type::popup || ekg::hovered::downtype == ekg::type::popup) {
    return nullptr;
  }

  auto p_ui {new ekg::ui::popup()};
  p_ui->set_type(ekg::type::popup);
  ekg::core->gen_widget(p_ui);

  if (interact_position) {
    auto &interact {ekg::input::interact()};
    p_ui->set_pos(interact.x, interact.y);
  }

  p_ui->set_width(100);
  p_ui->insert(component_list);
  p_ui->set_tag(tag);
  p_ui->set_scaled_height(1);
  p_ui->set_text_align(ekg::dock::center | ekg::dock::left);
  p_ui->set_level(ekg::level::top_level);

  return p_ui;
}

ekg::ui::textbox *ekg::textbox(std::string_view tag, std::string_view text, uint16_t dock) {
  auto p_ui {new ekg::ui::textbox()};
  p_ui->set_type(ekg::type::textbox);
  ekg::core->gen_widget(p_ui);

  p_ui->set_tag(tag);
  p_ui->set_place(dock);
  p_ui->set_scaled_height(1);
  p_ui->set_font_size(ekg::font::normal);
  p_ui->set_width(200);
  p_ui->emplace_back() = text;

  return p_ui;
}

ekg::ui::listbox *ekg::listbox(std::string_view tag, const ekg::item &item, uint16_t dock) {
  auto p_ui {new ekg::ui::listbox()};
  p_ui->set_type(ekg::type::listbox);
  ekg::core->gen_widget(p_ui);

  p_ui->set_tag(tag);
  p_ui->set_place(dock);
  p_ui->item() = item;
  p_ui->set_scaled_height(6);
  p_ui->set_item_font_size(ekg::font::normal);
  p_ui->set_category_font_size(ekg::font::normal);

  return p_ui;
}

ekg::ui::scroll *ekg::scroll(std::string_view tag) {
  auto p_ui {new ekg::ui::scroll()};

  p_ui->set_type(ekg::type::scroll);
  ekg::core->gen_widget(p_ui);
  p_ui->set_tag(tag);

  return p_ui;
}

void ekg::pop_group() {
  ekg::core->end_group_flag();
}