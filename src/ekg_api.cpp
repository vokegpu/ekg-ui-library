#include <ekg/ekg_api.hpp>
#include <ekg/ekg_utility.hpp>

uint64_t ekg_cpu_timing::last_ticks = 0;
bool ekg_cpu_timing::clock_going_on = false;

bool ekg_cpu_timing::start() {
    last_ticks = SDL_GetTicks64();
    clock_going_on = true;

    return clock_going_on;
}

bool ekg_cpu_timing::endif(uint64_t ms) {
    clock_going_on = false;
    return SDL_GetTicks64() - last_ticks > ms;
}

bool api::input1(SDL_Event &sdl_event) {
    switch (sdl_event.type) {
        case SDL_FINGERUP: {
            ekg_cpu_timing::clock_going_on = false;
            return true;
        }

        case SDL_FINGERDOWN: {
            return true;
        }

        case SDL_MOUSEBUTTONDOWN: {
            return sdl_event.motion.type == SDL_BUTTON_LEFT;
        }

        case SDL_MOUSEBUTTONUP: {
            ekg_cpu_timing::clock_going_on = false;
            return sdl_event.motion.type == SDL_BUTTON_LEFT;
        }
    }

    return false;
}

bool api::input2(SDL_Event &sdl_event) {
    switch (sdl_event.type) {
        case SDL_FINGERUP: {
            return ekg_cpu_timing::endif(EKG_ACTIVE_CALLBACK_MS_DELAY);
        }

        case SDL_FINGERDOWN: {
            ekg_display_touch_input.last_down_x = sdl_event.tfinger.x;
            ekg_display_touch_input.last_down_y = sdl_event.tfinger.y;

            return true;
        }

        case SDL_MOUSEBUTTONDOWN: {
            return sdl_event.motion.type == SDL_BUTTON_RIGHT;
        }

        case SDL_MOUSEBUTTONUP: {
            ekg_cpu_timing::clock_going_on = false;
            return sdl_event.motion.type == SDL_BUTTON_RIGHT;
        }
    }

    return false;
}

bool api::input3(SDL_Event &sdl_event) {
    switch (sdl_event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            return sdl_event.motion.type == SDL_BUTTON_MIDDLE;
        }

        case SDL_MOUSEBUTTONUP: {
            ekg_cpu_timing::clock_going_on = false;
            return sdl_event.motion.type == SDL_BUTTON_MIDDLE;
        }
    }

    return false;
}

void api::init() {
    EKG_ACTIVE_CALLBACK_MS_DELAY = 500;
    EKG_CPU_PLATFORM = api::cpu::X86;
}

float api::motion(SDL_Event &sdl_event, float x, float y) {
    if (!ekg_cpu_timing::clock_going_on) {
        return 0.0f;
    }

    switch (sdl_event.type) {
        case SDL_FINGERMOTION: {
            return y - ekg_display_touch_input.last_down_y;
        }

        case SDL_MOUSEWHEEL: {
            return sdl_event.wheel.preciseY;
        }
    }

    return 0.0f;
}

void api::send_output(const char *output) {
    switch (EKG_CPU_PLATFORM) {
        case api::cpu::X86: {
            std::string str = "[EKG] " + std::string(output);
            std::cout << str.c_str() << "\n";
            break;
        }

        case api::cpu::ARM: {
            //SDL_Log("[EKG] %s", output);
            break;
        }
    }
}

void api::OpenGL::init() {
    utility::log("API OpenGL initialised.");
}

void api::OpenGL::compile_program(api::OpenGL::program &program, const char *vertex_src, const char *fragment_src) {
    GLuint vertex_shader, fragment_shader;
    bool flag = true;

    if (!api::OpenGL::compile_shader(vertex_shader, GL_VERTEX_SHADER, vertex_src)) {
        utility::log("Could not compile vertex shader.");
        flag = false;
    }

    if (!api::OpenGL::compile_shader(fragment_shader, GL_FRAGMENT_SHADER, fragment_src)) {
        utility::log("Could not compile fragment shader.");
        flag = false;
    }

    if (flag) {
        program.program = glCreateProgram();

        glAttachShader(program.program, vertex_shader);
        glAttachShader(program.program, fragment_shader);
        glLinkProgram(program.program);

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        GLint program_compile_status = GL_FALSE;
        glGetProgramiv(program.program, GL_LINK_STATUS, &program_compile_status);

        if (program_compile_status != GL_TRUE) {
            char log[256];
            glGetProgramInfoLog(program.program, 256, NULL, log);

            utility::log("Could not link program.");
        }

        program.compiled = program_compile_status;
    }
}

bool api::OpenGL::compile_shader(GLuint &shader_id, GLuint mode, const char *src) {
    shader_id = glCreateShader(mode);

    glShaderSource(shader_id, 1, &src, NULL);
    glCompileShader(shader_id);

    GLint shader_compile_status = GL_FALSE;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &shader_compile_status);

    if (shader_compile_status != GL_TRUE) {
        char log[256];
        glGetShaderInfoLog(shader_id, 256, NULL, log);
        return false;
    }

    return true;
}

void api::OpenGL::program::use() {
    glUseProgram(this->program);
}

void api::OpenGL::program::set_mat4x4(const std::string &uniform_name, float *mat4x4) {
    glUniformMatrix4fv(glGetUniformLocation(this->program, uniform_name.c_str()), 1, GL_FALSE, mat4x4);
}
