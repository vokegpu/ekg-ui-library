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

#ifndef EKG_UTIL_ENV_H
#define EKG_UTIL_ENV_H

#include <iostream>
#include <sstream>
#include <vector>

namespace ekg {
    extern bool debug;
    extern float scrollsmooth;

    enum class env {
        refresh, swap, reload, synclayout, scissor, redraw, gc
    };

    enum class cursor {
        normal, arrow_axis_x, arrowleft, arrowp, arrowdown, arrow
    };

    enum class font {
        small = 0, normal = 1, big = 2
    };

    struct flag {
        bool highlight {};
        bool hovered {};
        bool activy {};
        bool focused {};
        bool state {};
        bool extra_state {};
        bool absolute {};
    };

    struct timing {
        uint64_t elapsed_ticks {};
        uint64_t current_ticks {};
        uint64_t ticks_going_on {};
    };

    struct log {
    protected:
        std::ostringstream buffer {};
    public:
        static constexpr int32_t WARNING {0};
        static constexpr int32_t INFO {1};
        static constexpr int32_t ERROR {2};

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
        }

        ~log() {
            this->buffer << '\n';
            std::cout << this->buffer.str();
        }

        template<typename t>
        log &operator<<(const t &value) {
            this->buffer << value;
            return *this;
    }
    };

    std::string utf8char32(char32_t);
    std::string utf8substr(std::string_view, size_t, size_t);
    size_t utf8length(std::string_view);
    char32_t char32str(std::string_view);
    void utf8read(std::string_view, std::vector<std::string>&);
    size_t utf8checksequence(uint8_t&, char32_t&, std::string&, std::string_view, size_t);

    float lerp(float, float, float);
    bool reach(ekg::timing&, uint64_t);
    bool reset(ekg::timing&);

    bool file_to_string(std::string &string_builder, const std::string &path);
    bool set(bool &var_mutable, bool predicate);
    int32_t &set(int32_t &var_mutable, int32_t predicate);

    std::string &set(std::string &var_mutable, const std::string &predicate);
    std::string string_float_precision(float, int32_t);

    namespace bitwise {
        bool contains(uint16_t target, uint16_t flag);
        uint16_t &add(uint16_t &target, uint16_t flags);
        uint16_t &remove(uint16_t &target, uint16_t flags);
    }

    namespace input {
        bool pressed(std::string_view);
        bool receive(std::string_view);
        void bind(std::string_view, std::string_view);
        bool motion();
        bool released();
        bool pressed();
        bool wheel();
    }
};

#endif