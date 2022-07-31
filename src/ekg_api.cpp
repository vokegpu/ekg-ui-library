/**
 * EKG-LICENSE - this software/library LICENSE can not be modified in any instance.
 *
 * --
 * ANY NON-OFFICIAL MODIFICATION IS A CRIME.
 * DO NOT SELL THIS CODE SOFTWARE, FOR USE EKG IN A COMMERCIAL PRODUCT ADD EKG-LICENSE TO PROJECT,
 * RESPECT THE COPYRIGHT TERMS OF EKG, NO SELL WITHOUT EKG-LICENSE (IT IS A CRIME).
 * DO NOT FORK THE PROJECT SOURCE WITHOUT EKG-LICENSE.
 *
 * END OF EKG-LICENSE.
 **/
#include <ekg/ekg.hpp>
#include "ekg/api/ekg_api.hpp"


uint64_t ekg_cpu_timing::last_ticks = 0;
bool ekg_cpu_timing::clock_going_on = false;

float ekgapi::display_interact_x;
float ekgapi::display_interact_y;

bool ekg_cpu_timing::start() {
    last_ticks = SDL_GetTicks64();
    clock_going_on = true;

    return clock_going_on;
}

bool ekg_cpu_timing::endif(uint64_t ms) {
    clock_going_on = false;
    return SDL_GetTicks64() - last_ticks > ms;
}

void ekgapi::init() {
    EKG_ACTIVE_CALLBACK_MS_DELAY = 500;
    EKG_CPU_PLATFORM = ekgapi::cpu::X86;
}

void ekgapi::scroll(SDL_Event &sdl_event, float &y) {
    if (!ekg_cpu_timing::clock_going_on) {
        return;
    }

    switch (sdl_event.type) {
        case SDL_FINGERMOTION: {
            y = sdl_event.tfinger.y - ekg_display_touch_input.last_down_y;
        }

        case SDL_MOUSEWHEEL: {
            y = sdl_event.wheel.preciseY;
        }
    }
}

void ekgapi::send_output(const char *output) {
    switch (EKG_CPU_PLATFORM) {
        case ekgapi::cpu::X86: {
            std::string str = "[EKG] " + std::string(output);
            std::cout << str.c_str() << "\n";
            break;
        }

        case ekgapi::cpu::ARM: {
            //SDL_Log("[EKG] %s", output);
            break;
        }
    }
}

bool ekgapi::input_down_right(SDL_Event &sdl_event, float &x, float &y) {
    switch (sdl_event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            x = static_cast<float>(sdl_event.button.x);
            y = static_cast<float>(sdl_event.button.y);

            ekgapi::display_interact_x = x;
            ekgapi::display_interact_y = y;

            return sdl_event.button.button == SDL_BUTTON_RIGHT;
        }

        case SDL_FINGERDOWN: {
            x = sdl_event.tfinger.x * the_ekg_core->get_screen_width();
            y = sdl_event.tfinger.y * the_ekg_core->get_screen_height();

            ekg_display_touch_input.last_down_x = x;
            ekg_display_touch_input.last_down_y = y;
            ekg_cpu_timing::start();

            ekgapi::display_interact_x = x;
            ekgapi::display_interact_y = y;

            return true;
        }
    }

    return false;
}

bool ekgapi::input_down_left(SDL_Event &sdl_event, float &x, float &y) {
    switch (sdl_event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            x = static_cast<float>(sdl_event.button.x);
            y = static_cast<float>(sdl_event.button.y);

            ekgapi::display_interact_x = x;
            ekgapi::display_interact_y = y;

            return sdl_event.button.button == SDL_BUTTON_LEFT;
        }

        case SDL_FINGERDOWN: {
            x = sdl_event.tfinger.x * the_ekg_core->get_screen_width();
            y = sdl_event.tfinger.y * the_ekg_core->get_screen_height();

            ekgapi::display_interact_x = x;
            ekgapi::display_interact_y = y;

            ekg_display_touch_input.last_down_x = sdl_event.tfinger.x;
            ekg_display_touch_input.last_down_y = sdl_event.tfinger.y;

            return true;
        }
    }

    return false;
}

bool ekgapi::input_down_middle(SDL_Event &sdl_event, float &x, float &y) {
    switch (sdl_event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            x = static_cast<float>(sdl_event.button.x);
            y = static_cast<float>(sdl_event.button.y);

            ekgapi::display_interact_x = x;
            ekgapi::display_interact_y = y;

            return sdl_event.button.button == SDL_BUTTON_MIDDLE;
        }
    }

    return false;
}

bool ekgapi::any_input_down(SDL_Event &sdl_event, float &x, float &y) {
    switch (sdl_event.type) {
        case SDL_FINGERDOWN: {
            x = sdl_event.tfinger.x * the_ekg_core->get_screen_width();
            y = sdl_event.tfinger.y * the_ekg_core->get_screen_height();

            ekgapi::display_interact_x = x;
            ekgapi::display_interact_y = y;

            return true;
        }

        case SDL_MOUSEBUTTONDOWN: {
            x = static_cast<float>(sdl_event.button.x);
            y = static_cast<float>(sdl_event.button.y);

            ekgapi::display_interact_x = x;
            ekgapi::display_interact_y = y;

            return true;
        }
    }


    return false;
}

bool ekgapi::any_input_down(SDL_Event &sdl_event) {
    return sdl_event.type == SDL_FINGERDOWN || sdl_event.type == SDL_MOUSEBUTTONDOWN;
}

bool ekgapi::input_up_right(SDL_Event &sdl_event, float &x, float &y) {
    switch (sdl_event.type) {
        case SDL_MOUSEBUTTONUP: {
            x = static_cast<float>(sdl_event.button.x);
            y = static_cast<float>(sdl_event.button.y);

            ekgapi::display_interact_x = x;
            ekgapi::display_interact_y = y;

            return sdl_event.button.button == SDL_BUTTON_RIGHT;
        }

        case SDL_FINGERUP: {
            x = sdl_event.tfinger.x;
            y = sdl_event.tfinger.y;

            ekgapi::display_interact_x = x;
            ekgapi::display_interact_y = y;

            return ekg_cpu_timing::endif(EKG_ACTIVE_CALLBACK_MS_DELAY);
        }
    }

    return false;
}

bool ekgapi::input_up_left(SDL_Event &sdl_event, float &x, float &y) {
    switch (sdl_event.type) {
        case SDL_MOUSEBUTTONUP: {
            x = static_cast<float>(sdl_event.button.x);
            y = static_cast<float>(sdl_event.button.y);

            ekgapi::display_interact_x = x;
            ekgapi::display_interact_y = y;

            return sdl_event.button.button == SDL_BUTTON_LEFT;
        }

        case SDL_FINGERUP: {
            x = sdl_event.tfinger.x * the_ekg_core->get_screen_width();
            y = sdl_event.tfinger.y * the_ekg_core->get_screen_height();

            ekgapi::display_interact_x = x;
            ekgapi::display_interact_y = y;

            return true;
        }
    }

    return false;
}

bool ekgapi::input_up_middle(SDL_Event &sdl_event, float &x, float &y) {
    switch (sdl_event.type) {
        case SDL_MOUSEBUTTONUP: {
            x = static_cast<float>(sdl_event.motion.x);
            y = static_cast<float>(sdl_event.motion.y);

            ekgapi::display_interact_x = x;
            ekgapi::display_interact_y = y;

            return sdl_event.button.button == SDL_BUTTON_MIDDLE;
        }
    }

    return false;
}

bool ekgapi::any_input_up(SDL_Event &sdl_event, float &x, float &y) {
    switch (sdl_event.type) {
        case SDL_FINGERUP: {
            x = sdl_event.tfinger.x * the_ekg_core->get_screen_width();
            y = sdl_event.tfinger.y * the_ekg_core->get_screen_height();

            ekgapi::display_interact_x = x;
            ekgapi::display_interact_y = y;

            return true;
        }

        case SDL_MOUSEBUTTONUP: {
            x = static_cast<float>(sdl_event.button.x);
            y = static_cast<float>(sdl_event.button.y);

            ekgapi::display_interact_x = x;
            ekgapi::display_interact_y = y;

            return true;
        }
    }

    return false;
}

bool ekgapi::any_input_up(SDL_Event &sdl_event) {
    return sdl_event.type == SDL_FINGERUP || sdl_event.type == SDL_MOUSEBUTTONUP;
}

bool ekgapi::motion(SDL_Event &sdl_event, float &x, float &y) {
    switch (sdl_event.type) {
        case SDL_MOUSEMOTION: {
            x = static_cast<float>(sdl_event.motion.x);
            y = static_cast<float>(sdl_event.motion.y);

            ekgapi::display_interact_x = x;
            ekgapi::display_interact_y = y;

            return true;
        }

        case SDL_FINGERMOTION: {
            x = sdl_event.tfinger.x * the_ekg_core->get_screen_width();
            y = sdl_event.tfinger.y * the_ekg_core->get_screen_height();

            ekgapi::display_interact_x = x;
            ekgapi::display_interact_y = y;

            return true;
        }
    }

    return false;
}

bool ekgapi::set(bool &current, bool value) {
    if (current != value) {
        current = value;
        the_ekg_core->dispatch_todo_event(ekgutil::action::REFRESH);
    }

    return current;
}

bool ekgapi::set_direct(bool &current, bool value) {
    current = value;
    return current;
}

void ekgapi::set(std::string &current, const std::string &value) {
    if (current != value) {
        current = value;
        the_ekg_core->dispatch_todo_event(ekgutil::action::REFRESH);
    }
}

void ekgapi::set_direct(std::string &current, const std::string &value) {
    current = value;
}

void ekgapi::callback_popup(uint32_t id, const std::string &path) {
    SDL_Event sdl_custom_event;
    auto ekg_custom_event = new ekg_event();

    ekg_custom_event->id = id;
    ekg_custom_event->type = ekg::ui::POPUP;
    ekg_custom_event->text = path;

    sdl_custom_event.type = SDL_USEREVENT;
    sdl_custom_event.user.type = SDL_USEREVENT;
    sdl_custom_event.user.code = static_cast<int32_t>(EKG_EVENT);
    sdl_custom_event.user.data1 = static_cast<void*>(ekg_custom_event);

    the_ekg_core->dispatch_event(sdl_custom_event);
}

void ekgapi::callback_check_box(uint32_t id, const std::string &text, bool value) {
    SDL_Event sdl_custom_event;
    auto ekg_custom_event = new ekg_event();

    ekg_custom_event->id = id;
    ekg_custom_event->type = ekg::ui::CHECKBOX;
    ekg_custom_event->boolean = value;
    ekg_custom_event->text = text;

    sdl_custom_event.type = SDL_USEREVENT;
    sdl_custom_event.user.type = SDL_USEREVENT;
    sdl_custom_event.user.code = static_cast<int32_t>(EKG_EVENT);
    sdl_custom_event.user.data1 = static_cast<void*>(ekg_custom_event);

    the_ekg_core->dispatch_event(sdl_custom_event);
}

void ekgapi::callback_button(uint32_t id, const std::string &text) {
    SDL_Event sdl_custom_event;
    auto ekg_custom_event = new ekg_event();

    ekg_custom_event->id = id;
    ekg_custom_event->type = ekg::ui::BUTTON;
    ekg_custom_event->text = text;

    sdl_custom_event.type = SDL_USEREVENT;
    sdl_custom_event.user.type = SDL_USEREVENT;
    sdl_custom_event.user.code = static_cast<int32_t>(EKG_EVENT);
    sdl_custom_event.user.data1 = static_cast<void*>(ekg_custom_event);

    the_ekg_core->dispatch_event(sdl_custom_event);
}

void ekgapi::OpenGL::init() {
    ekgutil::log("API OpenGL initialised.");
}

void ekgapi::OpenGL::compile_program(ekgapi::OpenGL::program &program, const char *vertex_src, const char *fragment_src) {
    GLuint vertex_shader, fragment_shader;
    bool flag = true;

    if (!ekgapi::OpenGL::compile_shader(vertex_shader, GL_VERTEX_SHADER, vertex_src)) {
        ekgutil::log("Could not compile vertex shader.");
        flag = false;
    }

    if (!ekgapi::OpenGL::compile_shader(fragment_shader, GL_FRAGMENT_SHADER, fragment_src)) {
        ekgutil::log("Could not compile fragment shader.");
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

            ekgutil::log("Could not link program.");
        }

        program.compiled = program_compile_status;
    }
}

bool ekgapi::OpenGL::compile_shader(GLuint &shader_id, GLuint mode, const char *src) {
    shader_id = glCreateShader(mode);

    glShaderSource(shader_id, 1, &src, NULL);
    glCompileShader(shader_id);

    GLint shader_compile_status = GL_FALSE;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &shader_compile_status);

    if (shader_compile_status != GL_TRUE) {
        char log[256];
        glGetShaderInfoLog(shader_id, 256, NULL, log);
        ekgutil::log(log);
        return false;
    }

    return true;
}

void ekgapi::OpenGL::program::use() {
    glUseProgram(this->program);
}

void ekgapi::OpenGL::program::set_mat4x4(const std::string &uniform_name, float *mat4x4) {
    glUniformMatrix4fv(glGetUniformLocation(this->program, uniform_name.c_str()), 1, GL_FALSE, mat4x4);
}

void ekgapi::OpenGL::program::set_int(const std::string &uniform_name, int32_t val) {
    glUniform1i(glGetUniformLocation(this->program, uniform_name.c_str()), val);
}

void ekgapi::OpenGL::program::set_float(const std::string &uniform_name, float val) {
    glUniform1f(glGetUniformLocation(this->program, uniform_name.c_str()), val);
}

void ekgapi::OpenGL::program::set_vec4f(const std::string &uniform_name, const float *vec4) {
    glUniform4fv(glGetUniformLocation(this->program, uniform_name.c_str()), 1, vec4);
}

void ekgapi::OpenGL::program::set_vec2f(const std::string &uniform_name, const float* vec2) {
    glUniform2fv(glGetUniformLocation(this->program, uniform_name.c_str()), 1, vec2);
}
