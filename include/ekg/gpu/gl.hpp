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

#ifndef EKG_GPU_GL_H
#define EKG_GPU_GL_H

#if defined(_WIN32)
#include "GL/glew.h"
#elif defined(__linux__)
#include "GL/glew.h"
#elif defined(__ANDROID__)
#include "SDL2/SDL_opengles2_gl2.h"
#endif

#include <iostream>
#include <string>

namespace ekg::gpu {
    struct program {
        GLuint id {};

        void set(const std::string &str, bool value);
        void set(const std::string &str, GLuint value);
        void set(const std::string &str, GLint value);
        void set(const std::string &str, GLfloat value);
        void set2(const std::string &str, GLfloat* value);
        void set3(const std::string &str, GLfloat* value);
        void set4(const std::string &str, GLfloat* value);
        void setm2(const std::string &str, GLfloat* matrix);
        void setm3(const std::string &str, GLfloat* matrix);
        void setm4(const std::string &str, GLfloat* matrix);
    };

    void init_opengl_context();
    void invoke(ekg::gpu::program &program);
    void revoke();

    bool compile_shader(GLuint &shader, GLuint shader_type, const char* src);
    bool load_basic_program(gpu::program &program, const std::string &vsh_path, const std::string &fsh_path);
    bool create_basic_program(gpu::program &program, const char* vsh_src, const char* fsh_src);
};

#endif