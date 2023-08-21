/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, free to use in anything (utility, hacking, cheat, game, software).
 * 5- Malware, rat and others virus. We do not care.
 * 6- Do not modify this license under any instance.
 *
 * @VokeGpu 2023 all rights reserved.
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
    protected:
        std::ostringstream buffer {};
    public:
        static constexpr int32_t WARNING {0};
        static constexpr int32_t INFO {1};
        static constexpr int32_t ERROR {2};
        static std::string cache;

        static bool buffering() {
            return !ekg::log::cache.empty();
        }
        
        static void flush() {
            ekg::log::cache.clear();
        }

        explicit log(int32_t priority = ekg::log::INFO) {
            switch (priority) {
                case ekg::log::WARNING: {
                    this->buffer << "- [EKG-WARNING] ";
                    break;
                }

                case ekg::log::INFO: {
                    this->buffer << "[EKG-INFO] ";
                    break;
                }

                case ekg::log::ERROR: {
                    this->buffer << "[EKG-ERROR] ";
                    break;
                }

                default: {
                    break;
                }
            }

            if (ekg::log::cache.size() >= 50000) ekg::log::cache.clear();
        }

        ~log() {
            this->buffer << '\n';
            std::string logmsg {this->buffer.str()};

            #if defined(__ANDROID__)
            __android_log_print(ANDROID_LOG_VERBOSE, "EKG", "%s", logmsg.c_str());
            #else
            std::cout << logmsg;
            #endif

            ekg::log::cache += logmsg;
        }

        template<typename t>
        log &operator<<(const t &value) {
            this->buffer << value;
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