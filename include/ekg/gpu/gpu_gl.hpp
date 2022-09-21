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

namespace ekg::gpu {
    void init_opengl_context();
};

#endif