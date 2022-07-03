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
    struct vec4 {
        float x, y, z, w;

        vec4();
        vec4(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);
    };

    /**
     * Store position and size AB.
     **/
    struct rect {
        float x, y, w, h;

        /*
         * Collide point aabb with this rect.
         */
        bool collide_aabb_with_point(float px, float py);
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
namespace utility {
    /*
     * Send output log.
     */
    void log(const std::string &log);

    /*
     * Find target flag from flags.
     */
    bool contains(uint8_t &flags, uint8_t target);

    /*
     * Remove some target flag from flags.
     */
    bool remove(uint8_t &flags, uint8_t target);

    /*
     * Add flag into flags.
     */
    bool add(uint8_t &flags, uint8_t val_flag);

    /**
     * Flag events in a unique place.
     **/
    struct flag {
        bool flag_over;
        bool flag_highlight;
        bool flag_activy;
        bool flag_focused;
        bool flag_dead;
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
    };

    /**
     * Visibility of elements in screen.
     **/
    enum visibility {
        VISIBLE, LOW_PRIORITY, NONE
    };

    /**
     * Actions in EKG core.
     **/
    enum action {
        REFRESH   = 1 << 0,
        FIX_STACK = 1 << 1
    };
};

#endif