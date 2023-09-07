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

#if defined(__ANDROID__)
#include <android/log.h>
#endif

#include "geometry.hpp"

namespace ekg {
    struct log {
    public:
        static std::ostringstream buffer;
        static bool buffered;
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

    struct value {
    protected:
        void *p_value {};
    public:
    };

    namespace bitwise {
        bool contains(uint16_t flags, uint16_t target);
        uint16_t &add(uint16_t &flags, uint16_t target);
        uint16_t &remove(uint16_t &flags, uint16_t target);
    }

    namespace input {
        bool action(std::string_view action_key);
        bool receive(std::string_view input_key);
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