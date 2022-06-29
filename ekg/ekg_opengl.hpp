#pragma once
#ifndef EKG_OPENGL_H
#define EKG_OPENGL_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #include <GL/glew.h>
#elif __ANDROID__
    #include <SDL_opengles2.h>
#endif
#endif