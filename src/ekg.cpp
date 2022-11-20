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
#include "ekg/cpu/info.hpp"

ekg::runtime* ekg::core {nullptr};
std::string ekg::gl_version {"#version 450 core"};

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
    ekg::log("initialising ekg...");
    ekg::gpu::init_opengl_context();

#if defined(_WIN)
    ekg::os = {ekg::platform::os_win};
#elif defined(__linux__)
    ekg::os = {ekg::platform::os_linux};
#elif defined(__ANDROID__)
    ekg::os = {ekg::platform::os_android};
#endif

    ekg::core = new ekg::runtime();
    ekg::core->get_f_renderer_small().font_path = font_path;
    ekg::core->get_f_renderer_normal().font_path = font_path;
    ekg::core->get_f_renderer_big().font_path = font_path;

    ekg::core->init();
    ekg::core->set_root(root);

    /* Create main shading program. */

    const std::string vsh_src {
ekg::gl_version + "\n"
"layout (location = 0) in vec2 VertexData;\n"
"layout (location = 1) in vec2 UVData;\n"

"uniform mat4 MatrixProjection;\n"
"uniform vec4 Rect;\n"
"uniform float Depth;\n"
""
"out vec2 TextureUV;\n"
"out vec4 ShapeRect;\n"
""
"void main() {"
"    vec2 ProcessedVertex = VertexData;\n"
"    bool FixedShape = Rect.z != 0 || Rect.w != 0;\n"
""
"    if (FixedShape) {"
"        ProcessedVertex *= Rect.zw;"
"    }\n"
""
"    ProcessedVertex += Rect.xy;\n"
"    gl_Position = MatrixProjection * vec4(ProcessedVertex, Depth, 1.0f);\n"
""
"    TextureUV = UVData;\n"
"    ShapeRect = Rect;\n"
"}"};

    const std::string fsh_src {
ekg::gl_version + "\n"
""
"layout (location = 0) out vec4 OutColor;\n"
""
"in vec2 TextureUV;\n"
"in vec4 ShapeRect;\n"
""
"const int PixelDifference = 1;\n"
""
"uniform int LineThickness;\n"
"uniform float ViewportHeight;\n"
"uniform bool ActiveTexture;\n"
"uniform sampler2D ActiveTextureSlot;\n"
"uniform vec4 Color;\n"
""
"void main() {"
"    OutColor = Color;\n"
"    vec2 FragPos = vec2(gl_FragCoord.x, ViewportHeight - gl_FragCoord.y);\n"
"    if (LineThickness > 0) {"
"        vec4 OutlineRect = vec4(ShapeRect.x + LineThickness, ShapeRect.y + LineThickness, ShapeRect.z - (LineThickness * 2), ShapeRect.w - (LineThickness * 2));\n"
"        bool Collide = FragPos.x > OutlineRect.x && FragPos.x < OutlineRect.x + OutlineRect.z && FragPos.y > OutlineRect.y && FragPos.y < OutlineRect.y + OutlineRect.w;\n"
"        "
"        if (Collide) {"
"            discard;"
"        }"
"    } else if (LineThickness < 0) {\n"
"        vec2 FragPos = vec2(gl_FragCoord.x, ViewportHeight - gl_FragCoord.y);\n"
"        float L = LineThickness * 4;\n"
"        \n"
"    }\n"
"    if (ActiveTexture) {"
"        OutColor = texture(ActiveTextureSlot, TextureUV);\n"
"        OutColor = vec4(OutColor.xyz - ((1.0f - Color.xyz) - 1.0f), OutColor.w - (1.0f - Color.w));"
"    }\n"
"}"};

    gpu::create_basic_program(ekg::gpu::allocator::program, vsh_src.c_str(), fsh_src.c_str());

    /* First update of orthographic matrix and uniforms. */

    SDL_GetWindowSize(root, &ekg::display::width, &ekg::display::height);
    ekg::gpu::invoke(ekg::gpu::allocator::program);
    ekg::orthographic2d(ekg::gpu::allocator::orthographicm4, 0, static_cast<float>(ekg::display::width), static_cast<float>(ekg::display::height), 0);
    ekg::gpu::allocator::program.setm4("MatrixProjection", ekg::gpu::allocator::orthographicm4);
    ekg::gpu::allocator::program.set("ViewportHeight", static_cast<float>(ekg::display::height));
    ekg::gpu::revoke();
    ekg::log("orthographic root display (" + std::to_string(ekg::display::width) + ", " + std::to_string(ekg::display::height) + ")");

    /* SDL info. */

    SDL_version sdl_version {};
    SDL_GetVersion(&sdl_version);
    ekg::log("SDL version: " + std::to_string(sdl_version.major) + "." + std::to_string(sdl_version.minor) + "." + std::to_string(sdl_version.patch));
}

void ekg::quit() {
    ekg::log("quiting from ekg...");

    ekg::core->quit();
    delete ekg::core;
}

void ekg::event(SDL_Event &sdl_event) {
    bool phase_keep_process {
            sdl_event.type == SDL_MOUSEBUTTONDOWN || sdl_event.type == SDL_MOUSEBUTTONUP ||
            sdl_event.type == SDL_FINGERUP        || sdl_event.type == SDL_FINGERDOWN ||
            sdl_event.type == SDL_FINGERMOTION    || sdl_event.type == SDL_MOUSEMOTION ||
            sdl_event.type == SDL_KEYDOWN         || sdl_event.type == SDL_KEYUP ||
            sdl_event.type == SDL_WINDOWEVENT
    };

    if (!phase_keep_process) {
        return;
    }

    switch (sdl_event.type) {
        case SDL_WINDOWEVENT: {
            switch (sdl_event.window.event) {
                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    /* Set new display size, update ortographic matrix and pass the uniforms to the main shader. */

                    ekg::display::width = sdl_event.window.data1;
                    ekg::display::height = sdl_event.window.data2;

                    ekg::gpu::invoke(ekg::gpu::allocator::program);
                    ekg::orthographic2d(ekg::gpu::allocator::orthographicm4, 0, ekg::display::width, ekg::display::height, 0);
                    ekg::gpu::allocator::program.setm4("MatrixProjection", ekg::gpu::allocator::orthographicm4);
                    ekg::gpu::allocator::program.set("ViewportHeight", static_cast<float>(ekg::display::height));
                    ekg::gpu::revoke();
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
    auto ui = new ekg::ui::frame();
    ui->set_tag(tag);
    ui->set_type(ekg::type::frame);
    ekg::core->create_ui(ui);

    ui->set_pos_initial(initial_position.x, initial_position.y);
    ui->set_size_initial(size.x, size.y);

    return ui;
}

ekg::ui::button *ekg::button(std::string_view text, uint16_t dock) {
    auto ui = new ekg::ui::button();
    ui->set_type(ekg::type::button);
    ekg::core->create_ui(ui);

    ui->set_text(ekg::dock::center);
    ui->set_text(text);
    ui->set_dock(dock);
    ui->set_scaled_height(1);
    ui->set_font_size(ekg::font::normal);

    return ui;
}

void ekg::popgroup() {
    ekg::core->reset_group_instance();
}

ekg::ui::label *ekg::label(std::string_view text, uint16_t dock) {
    auto ui = new ekg::ui::label();
    ui->set_type(ekg::type::label);
    ekg::core->create_ui(ui);

    ui->set_text(ekg::dock::center);
    ui->set_text(text);
    ui->set_dock(dock);
    ui->set_scaled_height(1);
    ui->set_font_size(ekg::font::normal);

    return ui;
}

ekg::ui::checkbox* ekg::checkbox(std::string_view text, uint16_t dock) {
    auto ui = new ekg::ui::checkbox();
    ui->set_type(ekg::type::checkbox);
    ekg::core->create_ui(ui);

    ui->set_text(ekg::dock::left | ekg::dock::center);
    ui->set_text(text);
    ui->set_dock(dock);
    ui->set_scaled_height(1);
    ui->set_font_size(ekg::font::normal);

    return ui;
}

ekg::ui::slider* ekg::slider(std::string_view tag, float val, float min, float max, uint16_t dock) {
    auto ui = new ekg::ui::slider();
    ui->set_type(ekg::type::slider);
    ekg::core->create_ui(ui);

    ui->set_bar(ekg::dock::left);
    ui->set_tag(tag);
    ui->set_dock(dock);
    ui->set_scaled_height(1);
    ui->set_font_size(ekg::font::normal);

    return ui;
}
