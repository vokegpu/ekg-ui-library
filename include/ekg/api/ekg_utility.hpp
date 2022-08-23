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
#include <SDL2/SDL.h>

/**
 * EKG math utils.
 **/
namespace ekgmath {
    /*
     * Vec of 2 values.
     */
    struct vec2f {
        float x = 0.0f;
        float y = 0.0f;

        vec2f() = default;
        vec2f(float x, float y);
    };

    /*
     * Vec of 4 values.
     */
    struct vec4f {
        float x = 0;
        float y = 0;
        float z = 0;
        float w = 0;

        vec4f() = default;
        vec4f(float x, float y, float z, float w);
        vec4f(int32_t red, int32_t green, int32_t blue, int32_t alpha = 255);

        /*
         * Convert 0 - 255 RGBA to normalised value 0.0 - 1.0.
         */
        void color(int32_t red, int32_t green, int32_t blue, int32_t alpha = 255);
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
         * Collide rect aabb with this rect.
         */
        bool collide_aabb_with_rect(const ekgmath::rect &r);

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
     * Clamp int min and max.
     */
    int32_t clampi(int32_t val, int32_t min, int32_t max);

    /*
     * Clamp AABB (x, y, w, h) with screen width and height.
     */
    void clamp_aabb_with_screen_size(float &minx, float &miny, float maxx, float maxy);

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
        bool extra = false;
        bool state = false;
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

/**
 * The EKG text api for process inputs from virtual keyboard or real keyboard.
 **/
namespace ekgtext {
    enum action {
        INSERT          = 0,
        INSERT_LINE     = 1,
        REMOVE          = 2,
        REMOVE_OPPOSITE = 3
    };

    /**
     * Store text position, cursor position and visual details.
     **/
    struct box {
        ekgmath::vec2f bounds;
        std::vector<std::string> loaded_text_chunk_list;

        int32_t min_chunk_visible;
        int32_t max_chunk_visible;

        int32_t cursor[4];
        int32_t max_chunk_size;
        int32_t max_text_amount;
        bool final_flag;
    };

    /*
     * Process the amount of rows per columns.
     */
    void process_text_rows(ekgtext::box &box, std::string &raw_text);

    /*
     * Verify if box should sync cursor index with render offset.
     */
    void should_sync_ui(ekgtext::box &box, const std::string &text, bool &should);

    /*
     * Sync cursor index with render offset using lerp animation.
     */
    void sync_ui(ekgtext::box &box, const std::string &text);

    /*
     * Process cursor position by relative interact position.
     */
    void process_cursor_pos_relative(ekgtext::box &box, const std::string &text, float &x, float &y);

    /*
     * Process cursor position by matrix index position.
     */
    void process_cursor_pos_index(ekgtext::box &box, int32_t row, int32_t column, int32_t max_row, int32_t max_column);

    /*
     * Get text chunks by box index.
     */
    void get_chunks_index_from_box(ekgtext::box &box, std::vector<int32_t> &chunks_index, int32_t index_box[4]);

    /*
     * Process if text is different.
     */
    void process_new_text(ekgtext::box &box, const std::string& new_text, int32_t action);

    /*
     * Process the events of box.
     */
    void process_event(ekgtext::box &box, const ekgmath::rect &rect, std::string &raw_text, bool &flag, SDL_Event &sdl_event);

    /*
     * As you can see it resets the cursor draw.
     */
    void reset_cursor_loop();

    /*
     * Process draw cursor loop.
     */
    void process_cursor_loop(bool &flag);

    /*
     * Process the GPU data be is draw.
     */
    void process_render_box(ekgtext::box &box, ekgmath::rect &rect, int32_t &scissor_id, bool &draw_cursor);
};

#endif