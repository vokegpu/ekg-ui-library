#pragma once
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
        void set(const std::string &str, int32_t value);
        void set(const std::string &str, float value);
        void set2(const std::string &str, float* value);
        void set3(const std::string &str, float* value);
        void set4(const std::string &str, float* value);
        void setm2(const std::string &str, float* matrix);
        void setm3(const std::string &str, float* matrix);
        void setm4(const std::string &str, float* matrix);
    };

    void init_opengl_context();
};

#endif