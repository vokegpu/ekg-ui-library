#pragma once
#ifndef EKG_UTIL_H
#define EKG_UTIL_H

#include <iostream>
#include <vector>

/**
 * EKG math utils.
 **/
namespace ekgmath {
    /*
     * Vec4 class.
     */
    struct vec4f {
        float x = 0;
        float y = 0;
        float z = 0;
        float w = 0;

        vec4f();
        vec4f(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);

        /*
         * Convert 0 - 255 RGBA to normalised value 0.0 - 1.0.
         */
        void color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);
    };

    /**
     * Store position and size AB.
     **/
    struct rect {
        float x = 0;
        float y = 0;
        float w = 0;
        float h = 0;

        /*
         * Collide point aabb with this rect.
         */
        bool collide_aabb_with_point(float px, float py);

        /*
         * Copy position and size.
         */
        void copy(ekgmath::rect &r);

        void operator += (ekgmath::rect &r) {
            this->x += r.x;
            this->y += r.y;
        }
    };

    /*
     * Interpolation linear, used to animations or calcs.
     */
    float lerpf(float a, float b, float t);

    /*
     * Clamp float min and max.
     */
    float clampf(float val, float min, float max);

    /*
     * Calc. ortho projection mat4x4.
     */
    void ortho2d(float* mat, float left, float right, float bottom, float top);
};

/**
 * Utilities of EKG used on the background GUI.
 **/
namespace ekgutil {
    /*
     * Send output log.
     */
    void log(const std::string &log);

    /*
     * Find target flag from flags.
     */
    bool contains(uint16_t &flags, uint16_t target);

    /*
     * Remove some target flag from flags.
     */
    bool remove(uint16_t &flags, uint16_t target);

    /*
     * Add flag into flags.
     */
    bool add(uint16_t &flags, uint16_t val_flag);

    /*
     * Capture the dock based on a bound.
     */
    bool find_axis_dock(uint16_t &target, float px, float py, float offset, ekgmath::rect &rect);

    /**
     * Store flags to sync GUI with UI.
     **/
    struct flag {
        bool old_over = false;
        bool old_highlight = false;
        bool old_activy = false;
        bool old_focused = false;
        bool old_dead = false;

        bool over = false;
        bool highlight = false;
        bool activy = false;
        bool focused = false;
        bool dead = false;
    };

    /**
     * Resource used in EKG to redirect_data some data.
     **/
    struct resource {
        const char* path;

        void* data1;
        void* data2;
    };

    /**
     * Stack object to redirect_data ids.
     **/
    struct stack {
        // A vector to handle the ids.
        std::vector<uint32_t> ids;

        /*
         * Find if exists an id in ids list.
         */
        bool contains(uint32_t id);

        /*
         * Remove an id from ids.
         */
        bool remove(uint32_t id);

        /*
         * Add an id at ids list.
         */
        void add(uint32_t id);

        /*
         * Add other stack.
         */
        void add(ekgutil::stack& stack);
    };

    /**
     * Actions in EKG core.
     **/
    enum action {
        SWAPBUFFERS = 1 << 0,
        FIXSTACK    = 1 << 1,
        REFRESH     = 1 << 2,
        FIXRECTS    = 1 << 3
    };

    /**
     * Category of shapes to draw.
     */
    enum shape_category {
        RECTANGLE = 0,
        CIRCLE    = 1
    };
};

#endif