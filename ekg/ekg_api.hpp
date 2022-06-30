#pragma once
#ifndef EKG_API_H
#define EKG_API_H

#include <SDL2/SDL.h>
#include "ekg_opengl.hpp"

/*
 * Press event cool down/wait ms delay.
 */
static uint64_t EKG_ACTIVE_CALLBACK_MS_DELAY;

/**
 * Analyse the display touch input and store to later be use.
 **/
struct {
    float last_down_x;
    float last_down_y;
} ekg_display_touch_input;

/**
 * Make the CPU count ticks to be use in api.
 **/
struct ekg_cpu_timing {
    static uint64_t last_ticks;
    static bool clock_going_on;

    /*
     * Start count ticks.
     */
    static bool start();

    /*
     * Stop count ticks if > ms val argument.
     */
    static bool endif(uint64_t ms);
};

/**
 * Different CPUs and platforms are one thing normal in programming world. \n
 * EKG native supports ARM and x86. \n
 * \n
 * Here we will choose which to use. \n
 **/
namespace api {
    /*
     * Platform supported.
     */
    enum cpu {
        X86, ARM
    };

    /*
     * Init the api.
     */
    void init();

    /*
     * Send logs.
     */
    void send_output(const char* output);

    /*
     * Mouse left click or finger touch.
     */
    bool input1(SDL_Event &sdl_event);

    /*
     * Mouse right click.
     */
    bool input2(SDL_Event &sdl_event);

    /*
     * Mouse middle click.
     */
    bool input3(SDL_Event &sdl_event);

    /*
     * Detect scroll or calc. scroll from previous moving click.
     */
    float motion(SDL_Event &sdl_event, float x, float y);

    /**
     * The OpenGL functions.
     **/
     namespace OpenGL {
         /*
          * Init OpenGL basic stuff.
          */
         void init();

         /**
          * Shader program are mini-programs (program) that GPU runs.
          **/
         struct program {
             GLuint program;
             bool compiled;
         };

         /*
          * Returns true if shader is compiled else false.
          */
         bool compile_shader(GLuint &shader_id, GLuint mode, const char* src);

         /*
          * Compile OpenGL program for use.
          */
         void compile_program(api::OpenGL::program &program, const char* vertex_src, const char* fragment_src);
     };

     /**
      * File manager.
      **/
      namespace file {
          void init();
      };
};

/**
 * Current project platform.
 **/
static api::cpu EKG_CPU_PLATFORM;

#endif