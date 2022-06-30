#pragma once
#ifndef EKG_OPENGL_H
#define EKG_OPENGL_H

#ifdef _WIN64
    #include <GL/glew.h>
#elif _WIN32
    #include <GL/glew.h>
#elif __ANDROID__
    #include <SDL_opengles2.h>
#elif __linux
    #include <GL/glew.h>
#endif
#endif