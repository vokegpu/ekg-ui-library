#pragma once
#ifndef EKG_UTIL_H
#define EKG_UTIL_H

#include <iostream>
#include <vector>

/**
 * Utilities of EKG used on the background GUI.
 **/
namespace utility {
    /*
     * Send output log.
     */
    void log(const char* log);

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
     * Resource used in EKG to store some data.
     **/
    struct resource {
        const char* path;

        void* data1;
        void* data2;
    };

    /**
     * Stack object to store ids.
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
        REFRESH   = 1 << 1,
        FIX_STACK = 1 << 2
    };
};

#endif