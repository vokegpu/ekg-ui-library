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

#include "ekg/ekg.hpp"
#include "ekg/os/info.hpp"

ekg::runtime* ekg::core {nullptr};
std::string ekg::gl_version {"#version 450"};

ekg::service::theme &ekg::theme() {
    return ekg::core->get_service_theme();
}

ekg::draw::font_renderer &ekg::f_renderer(ekg::font font_size) {
    switch (font_size) {
        case ekg::font::big: {
            return ekg::core->get_f_renderer_big();
        }

        case ekg::font::normal: {
            return ekg::core->get_f_renderer_normal();
        }

        case ekg::font::small: {
            return ekg::core->get_f_renderer_small();
        }
    }

    return ekg::core->get_f_renderer_normal();
}

void ekg::init(SDL_Window* root, std::string_view font_path) {
    ekg::log() << "Initialising EKG";
    ekg::gpu::init_opengl_context();
    ekg::listener = SDL_RegisterEvents(1);

#if defined(_WIN)
    ekg::os = {ekg::platform::os_win};
#elif defined(__ANDROID__)
    ekg::os = {ekg::platform::os_android};
#elif defined(__linux__)
    ekg::os = {ekg::platform::os_linux};
#endif

    const std::string vsh_src {
ekg::gl_version + "\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"

"#define CONVEX -256.0\n"

"uniform mat4 uOrthographicMatrix;\n"
"uniform vec4 uRect;\n"
"uniform float uDepth;\n"

"out vec2 vTexCoord;\n"
"out vec4 vRect;\n"

"void main() {"
"    vec2 vertex = aPos;\n"
"    bool modalShape = uRect.z != CONVEX || uRect.w != CONVEX;\n"

"    if (modalShape) {"
"        vertex *= uRect.zw;"
"    }\n"

"    vertex += uRect.xy;\n"
"    gl_Position = uOrthographicMatrix * vec4(vertex, 0.0f, 1.0f);\n"

"    vTexCoord = aTexCoord;\n"
"    vRect = uRect;\n"
"}"};

    const std::string fsh_src {
ekg::gl_version + "\n"
"layout (location = 0) out vec4 vFragColor;\n"
"uniform sampler2D uTexture;\n" // @TODO fix binding support in OpenGL high-performance

"in vec2 vTexCoord;\n"
"in vec4 vRect;\n"

"uniform int uLineThickness;\n"
"uniform bool uActiveTexture;\n"
"uniform vec4 uColor;\n"
"uniform vec4 uScissor;\n"
"uniform bool uEnableScissor;\n"
"uniform float uViewportHeight;\n"

"void main() {"
"    vFragColor = uColor;\n"
"    vec2 fragPos = vec2(gl_FragCoord.x, uViewportHeight - gl_FragCoord.y);\n"
"    bool shouldDiscard = uEnableScissor && (fragPos.x <= uScissor.x || fragPos.y <= uScissor.y || fragPos.x >= uScissor.x + uScissor.z || fragPos.y >= uScissor.y + uScissor.w);\n"

"    float lineThicknessf = float(uLineThickness);\n"
"    if (uLineThickness > 0) {"
"        vec4 outline = vec4(vRect.x + lineThicknessf, vRect.y + lineThicknessf, vRect.z - (lineThicknessf * 2.0f), vRect.w - (lineThicknessf * 2.0f));\n"
"        shouldDiscard = shouldDiscard || (fragPos.x > outline.x && fragPos.x < outline.x + outline.z && fragPos.y > outline.y && fragPos.y < outline.y + outline.w);\n"
"    } else if (uLineThickness < 0) {\n"
"       float radius = vRect.z / 2.0f;\n"
"       vec2 diff = vec2((vRect.x + radius) - fragPos.x, (vRect.y + radius) - fragPos.y);\n"
"       float dist = sqrt(diff.x * diff.x + diff.y * diff.y);\n"

"       vFragColor.w = (1.0f - smoothstep(0.0, radius * radius, dot(dist, dist)));\n"
"    }"

"    if (shouldDiscard) {"
"       vFragColor.w = 0.0f;\n"
"    }\n"

"    if (uActiveTexture && !shouldDiscard) {"
"        vFragColor = texture(uTexture, vTexCoord);\n"
"        vFragColor = vec4(vFragColor.xyz - ((1.0f - uColor.xyz) - 1.0f), vFragColor.w - (1.0f - uColor.w));\n"
"    }\n"
"}"};

    /* Create main shading program using two basic shaders (vertex & fragment). */
    ekg::gpu::create_basic_program(ekg::gpu::allocator::program, {
        {vsh_src, GL_VERTEX_SHADER},
        {fsh_src, GL_FRAGMENT_SHADER}
    });

    /* The runtime core, everything need to be setup before init. */
    ekg::core = new ekg::runtime();
    ekg::core->get_f_renderer_small().font_path = font_path;
    ekg::core->get_f_renderer_normal().font_path = font_path;
    ekg::core->get_f_renderer_big().font_path = font_path;
    ekg::core->set_root(root);
    ekg::core->init();

    /* First update of orthographic matrix and uniforms. */

    SDL_GetWindowSize(root, &ekg::display::width, &ekg::display::height);
    ekg::gpu::invoke(ekg::gpu::allocator::program);
    ekg::orthographic2d(ekg::gpu::allocator::mat4x4orthographic, 0, static_cast<float>(ekg::display::width), static_cast<float>(ekg::display::height), 0);
    ekg::gpu::allocator::program.setm4("uOrthographicMatrix", ekg::gpu::allocator::mat4x4orthographic);
    ekg::gpu::allocator::program.set("uViewportHeight", static_cast<float>(ekg::display::height));
    ekg::gpu::revoke();

    /* SDL info. */

    SDL_version sdl_version {};
    SDL_GetVersion(&sdl_version);

    ekg::log() << "SDL version: " << (int32_t) sdl_version.major << '.' << (int32_t) sdl_version.minor << '.' << (int32_t) (sdl_version.patch);
}

void ekg::quit() {
    ekg::core->quit();
    delete ekg::core;

    ekg::log() << "Shutdown complete - Thank you for using EKG ;) <3";
}

void ekg::event(SDL_Event &sdl_event) {
    bool phase_keep_process {
            sdl_event.type == SDL_MOUSEBUTTONDOWN || sdl_event.type == SDL_MOUSEBUTTONUP ||
            sdl_event.type == SDL_FINGERUP        || sdl_event.type == SDL_FINGERDOWN ||
            sdl_event.type == SDL_FINGERMOTION    || sdl_event.type == SDL_MOUSEMOTION ||
            sdl_event.type == SDL_KEYDOWN         || sdl_event.type == SDL_KEYUP ||
            sdl_event.type == SDL_WINDOWEVENT     || sdl_event.type == SDL_MOUSEWHEEL ||
            sdl_event.type == SDL_TEXTINPUT
    };

    if (!phase_keep_process) {
        return;
    }

    switch (sdl_event.type) {
        case SDL_WINDOWEVENT: {
            switch (sdl_event.window.event) {
                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    /* Set new display size, update orthographic matrix and pass the uniforms to the main shader. */

                    ekg::display::width = sdl_event.window.data1;
                    ekg::display::height = sdl_event.window.data2;

                    ekg::gpu::invoke(ekg::gpu::allocator::program);
                    ekg::orthographic2d(ekg::gpu::allocator::mat4x4orthographic, 0, static_cast<float>(ekg::display::width), static_cast<float>(ekg::display::height), 0);
                    ekg::gpu::allocator::program.setm4("uOrthographicMatrix", ekg::gpu::allocator::mat4x4orthographic);
                    ekg::gpu::allocator::program.set("uViewportHeight", static_cast<float>(ekg::display::height));
                    ekg::gpu::revoke();
                    ekg::core->update_size_changed();
                    break;
                }
            }

            break;
        }
    }

    ekg::core->process_event(sdl_event);
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

ekg::ui::frame *ekg::frame(std::string_view tag, const ekg::vec2 &initial_position, ekg::vec2 size) {
    auto ui {new ekg::ui::frame()};
    ui->set_tag(tag);
    ui->set_type(ekg::type::frame);
    ekg::core->gen_widget(ui);

    ui->set_pos_initial(initial_position.x, initial_position.y);
    ui->set_size_initial(size.x, size.y);

    return ui;
}

ekg::ui::button *ekg::button(std::string_view text, uint16_t dock) {
    auto ui {new ekg::ui::button()};
    ui->set_type(ekg::type::button);
    ekg::core->gen_widget(ui);

    ui->set_text(text);
    ui->set_place(dock);
    ui->set_scaled_height(1);
    ui->set_font_size(ekg::font::normal);
    ui->set_text_align(ekg::dock::left | ekg::dock::center);
    ui->set_tag(text);

    return ui;
}

ekg::ui::label *ekg::label(std::string_view text, uint16_t dock) {
    auto ui {new ekg::ui::label()};
    ui->set_type(ekg::type::label);
    ekg::core->gen_widget(ui);

    ui->set_text(text);
    ui->set_place(dock);
    ui->set_scaled_height(1);
    ui->set_font_size(ekg::font::normal);
    ui->set_text_align(ekg::dock::left | ekg::dock::center);
    ui->set_tag(text);

    return ui;
}

ekg::ui::checkbox *ekg::checkbox(std::string_view text, bool value, uint16_t dock) {
    auto ui {new ekg::ui::checkbox()};
    ui->set_type(ekg::type::checkbox);
    ekg::core->gen_widget(ui);

    ui->set_text(text);
    ui->set_place(dock);
    ui->set_scaled_height(1);
    ui->set_font_size(ekg::font::normal);
    ui->set_text_align(ekg::dock::left | ekg::dock::center);
    ui->set_box_align(ekg::dock::left | ekg::dock::center);
    ui->set_tag(text);
    ui->set_value(value);

    return ui;
}

ekg::ui::slider *ekg::slider(std::string_view tag, float val, float min, float max, uint16_t dock) {
    auto ui {new ekg::ui::slider()};
    ui->set_type(ekg::type::slider);
    ekg::core->gen_widget(ui);

    ui->set_tag(tag);
    ui->set_place(dock);
    ui->set_text_align(ekg::dock::center);
    ui->set_bar_align(ekg::dock::left | ekg::dock::center);
    ui->set_scaled_height(1);
    ui->set_font_size(ekg::font::normal);
    ui->set_value_min(min);
    ui->set_value_max(max);
    ui->set_precision(0);
    ui->set_bar_axis(ekg::axis::horizontal);
    ui->set_width(200);
    ui->set_value(val);

    return ui;
}

ekg::ui::popup *ekg::popup(std::string_view tag, const std::vector<std::string> &component_list, bool interact_position) {
    if (ekg::hovered::type == ekg::type::popup || ekg::hovered::downtype == ekg::type::popup) {
        return nullptr;
    }

    auto ui {new ekg::ui::popup()};
    ui->set_type(ekg::type::popup);
    ekg::core->gen_widget(ui);

    if (interact_position) {
        auto &interact {ekg::interact()};
        ui->set_pos(interact.x, interact.y);
    }

    ui->set_width(100);
    ui->append(component_list);
    ui->set_tag(tag);
    ui->set_scaled_height(1);
    ui->set_text_align(ekg::dock::center | ekg::dock::left);

    return ui;
}

ekg::ui::textbox *ekg::textbox(std::string_view tag, std::string_view text, uint16_t dock) {
    auto ui {new ekg::ui::textbox()};
    ui->set_type(ekg::type::textbox);
    ekg::core->gen_widget(ui);

    ui->set_tag(tag);
    ui->set_place(dock);
    ui->set_scaled_height(1);
    ui->set_font_size(ekg::font::normal);
    ui->set_width(200);
    ui->set_text(text);

    return ui;
}

ekg::ui::scroll *ekg::scroll(std::string_view tag) {
    auto ui {new ekg::ui::scroll()};
    ui->set_type(ekg::type::scroll);
    ui->set_tag(tag);
    ekg::core->gen_widget(ui);
    return ui;
}

void ekg::popgroup() {
    ekg::core->end_group_flag();
}
