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
     * Animation without delay with customizable end time.
     */
    void smoothf(float &val, float duration, uint32_t ticks);

    /*
     * Detect collision between interact point and rect.
     */
    bool collide_aabb_with_point(float x, float y, float w, float h);

    /*
     * Interpolation linear, used to make animations and change the movement of any val.
     */
    float lerpf(float a, float b, float t);

    /*
     * Clamp float min and max.
     */
    float clampf(float val, float min, float max);

    /*
     * Clamp AABB (x, y, w, h) with screen width and height.
     */
    void clamp_aabb_with_screen_size(float &minx, float &miny, float &maxx, float &maxy);

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
     * "text".contains("t"); // output: true
     */
    bool contains(const std::string &str1, const std::string &str2);

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

    /*
     * Component to store something.
     */
    struct component {
        float x = 0.0f;
        float y = 0.0f;
        float w = 0.0f;
        float h = 0.0f;

        std::string text;
        bool enabled = false;
        uint32_t data;
    };

    /**
     * Store flags to sync GUI with UI.
     **/
    struct flag {
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
        SWAPBUFFERS   = 1 << 0,
        FIXSTACK      = 1 << 1,
        REFRESH       = 1 << 2,
        FIXRECTS      = 1 << 3,
        REFRESHUPDATE = 1 << 4
    };

    /**
     * Category of shapes to draw.
     */
    enum shape_category {
        RECTANGLE = 0,
        CIRCLE    = 1,
        OUTLINE   = 2
    };
};

#endif