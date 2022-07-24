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
#ifndef EKG_API_H
#define EKG_API_H

#include <SDL2/SDL.h>
#include "ekg_opengl.hpp"
#include <string>

/*
 * Press event cool down/wait ms delay.
 */
static uint64_t EKG_ACTIVE_CALLBACK_MS_DELAY;

/**
 * Analyse the display touch input and redirect_data to later be use.
 **/
struct {
    float last_down_x;
    float last_down_y;
} ekg_display_touch_input;

/**
 * Make the CPU count ticks to be use in ekgapi.
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
namespace ekgapi {
    /*
     * The actual x relative position of a point interacting in screen (mouse or finger input).
     */
    extern float display_interact_x;

    /*
     * The actual y relative position of a point interacting in screen (mouse or finger input).
     */
    extern float display_interact_y;

    /*
     * Platform supported.
     */
    enum cpu {
        X86, ARM
    };

    /*
     * Init the ekgapi.
     */
    void init();

    /*
     * Send logs.
     */
    void send_output(const char* output);

    /*
     * Set synchronized flag value.
     */
    bool set(bool &current, bool value);

    /*
     * Set asynchronized flag value.
     */
    bool set_direct(bool &current, bool value);

    /*
     * Set synchronized string value.
     */
    void set(std::string &current, const std::string &value);

    /*
     * Set asynchronized string value.
     */
    void set_direct(std::string &current, const std::string &value);

    /* Start of input down. */
    bool input_down_right(SDL_Event &sdl_event, float &x, float &y);
    bool input_down_left(SDL_Event &sdl_event, float &x, float &y);
    bool input_down_middle(SDL_Event &sdl_event, float &x, float &y);
    bool any_input_down(SDL_Event &sdl_event, float &x, float &y);
    bool any_input_down(SDL_Event &sdl_event);
    /* End of input down. */

    /* Start of input up. */
    bool input_up_right(SDL_Event &sdl_event, float &x, float &y);
    bool input_up_left(SDL_Event &sdl_event, float &x, float &y);
    bool input_up_middle(SDL_Event &sdl_event, float &x, float &y);
    bool any_input_up(SDL_Event &sdl_event, float &x, float &y);
    bool any_input_up(SDL_Event &sdl_event);
    /* End of input up. */

    /*
     * Detect scroll or calc. scroll from previous moving click.
     */
    void scroll(SDL_Event &sdl_event, float &y);

    /*
     * Get pos input.
     */
    bool motion(SDL_Event &sdl_event, float &x, float &y);

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

            /*
             * Use the shader program.
             */
            void use();

            /*
             * Pass uniform float arr with size of 16 floats.
             */
            void set_mat4x4(const std::string &uniform_name, float* mat4x4);

            /*
             * Pass uniform float arr with size of 4 floats.
             */
            void set_vec4f(const std::string &uniform_name, const float* vec4);


            /*
             * Pass uniform float.
             */
            void set_float(const std::string &uniform_name, float val);

            /*
             * Pass uniform float arr with size of 2 floats.
             */
            void set_vec2f(const std::string &uniform_name, const float* vec2);

            /*
             * Pass uniform int32.
             */
            void set_int(const std::string &uniform_name, int32_t val);
        };

        /*
         * Returns true if shader is compiled else false.
         */
        bool compile_shader(GLuint &shader_id, GLuint mode, const char* src);

        /*
         * Compile OpenGL program for use.
         */
        void compile_program(ekgapi::OpenGL::program &program, const char* vertex_src, const char* fragment_src);
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
static ekgapi::cpu EKG_CPU_PLATFORM;

#endif