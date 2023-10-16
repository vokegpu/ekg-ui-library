/*
 * MIT License
 * 
 * Copyright (c) 2022-2023 Rina Wilk / vokegpu@gmail.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef EKG_UTIL_IO_H
#define EKG_UTIL_IO_H

#include <iostream>
#include <sstream>
#include <vector>
#include <cstdint>
#include <map>

#if defined(__ANDROID__)
#include <android/log.h>
#endif

#include "geometry.hpp"

namespace ekg {
    struct item {
    public:
        std::vector<ekg::item> child_list {};
        std::vector<uint64_t> connected_child_list {};
        std::string value {};

        uint16_t attr {};
        uint16_t state {};

        ekg::component component {};
        ekg::item *p_item_parent {};
        bool *p_semaphore {};
    public:
        ekg::item &operator=(std::string_view _value);
        ekg::item &operator=(const std::vector<std::string_view> &item_value_list);
        ekg::item &operator[](uint64_t index);
    public:
        item() = default;
        item(std::string_view _value);
        
        void clear();
        void push_back(const ekg::item &item_copy);
        void push_back(std::string_view item_value);
        void insert(const std::vector<std::string_view> &item_value_list);
        void insert(uint16_t predicate, uint64_t index, const std::vector<std::string_view> &value_list);

        ekg::item &emplace_back();
        ekg::item &insert(std::string_view item_value);
        ekg::item &insert(const ekg::item &item);
        ekg::item &at(uint64_t index);
        ekg::item &insert(uint64_t index, std::string_view value);
        ekg::item &insert(uint64_t index, const ekg::item &copy_item);

        [[nodiscard]] bool empty() const;
        [[nodiscard]] uint64_t size() const;
    
        std::vector<ekg::item>::iterator begin();
        std::vector<ekg::item>::iterator end();
    
        [[nodiscard]] std::vector<ekg::item>::const_iterator cbegin() const;
        [[nodiscard]] std::vector<ekg::item>::const_iterator cend() const;
    public:
        void set_value(std::string_view _value);
        bool has_parent();
        bool has_children();
    };

    struct log {
    public:
        static std::ostringstream buffer;
        static bool buffered;
        static bool tracked;
    public:
        static void flush() {
            if (ekg::log::buffered) {
                std::string p_log {ekg::log::buffer.str()};

                #if defined(__ANDROID__)
                __android_log_print(ANDROID_LOG_VERBOSE, "EKG", "%s", p_log);
                #else
                std::cout << p_log;
                #endif

                ekg::log::buffer = {};
                ekg::log::buffered = false;
            }
        }

        template<typename t>
        static void trace(bool should, t trace, bool interrupt_runtime = false) {
            if (!should) {
                return;
            }

            std::cout << "\n[EKG-TRACE] " << trace << std::endl;
            if (interrupt_runtime) {
                std::terminate();
            }
        }

        explicit log() {
            ekg::log::buffered = true;
            ekg::log::buffer << "\n[EKG-INFO] ";
        }

        template<typename t>
        log &operator<<(const t &value) {
            ekg::log::buffer << value;
            return *this;
        }
    };

    /*
     * Attributes are used by item,
     * they can be dynamically converted by string symbols:
     * \t separator
     * \\ box
     * \1 category
     * \2 row
     * \3 row_member
     * \4 unselectable
     */
    enum attr {
        separator  = 2 << 2,
        box        = 2 << 3,
        category   = 2 << 4,
        row        = 2 << 5,
        row_member = 2 << 6,
        unselectable = 2 << 7
    };

    bool file_to_string(std::string &file_content, std::string_view path);

    struct timing {
    public:
        uint64_t elapsed_ticks {};
        uint64_t current_ticks {};
        uint64_t ticks_going_on {};
    };

    bool reach(ekg::timing&, uint64_t);
    bool reset(ekg::timing&);

    struct flag {
        bool highlight {};
        bool hovered {};
        bool activy {};
        bool focused {};
        bool state {};
        bool extra_state {};
        bool absolute {};
    };

    enum class clipboard {
        copy  = 1073741948, // SDLK_COPY
        cut   = 1073741947, // SDLK_CUT
        paste = 1073741949  // SDLK_PASTE
    };

    namespace bitwise {
        bool contains(uint16_t flags, uint16_t target);
        uint16_t &add(uint16_t &flags, uint16_t target);
        uint16_t &remove(uint16_t &flags, uint16_t target);
    }

    namespace input {
        bool action(std::string_view action_key);
        bool receive(std::string_view input_key);
        void clipboard(ekg::clipboard clipboard_op);
        void fire(std::string_view action_key);
        void bind(std::string_view action_key, std::string_view input_key);
        bool motion();
        bool released();
        bool pressed();
        bool wheel();
        bool typed();
        ekg::vec4 &interact();
    }
}

#endif