#pragma once
#ifndef EKG_OPENGL_H
#define EKG_OPENGL_H

#define GLEW_STATIC
#include <GL/glew.h>

#ifdef _WIN64
    #define GLEW_STATIC
    #include <GL/glew.h>
#elif _WIN32
    #define GLEW_STATIC
    #include <GL/glew.h>
#elif __ANDROID__
    #include <SDL_opengles2.h>
#elif __linux
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif
#endif