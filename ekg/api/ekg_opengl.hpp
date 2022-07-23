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
#pragma once
#ifndef EKG_OPENGL_H
#define EKG_OPENGL_H

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