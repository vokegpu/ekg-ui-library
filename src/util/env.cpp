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

#include "ekg/util/util_event.hpp"
#include "ekg/ekg.hpp"

#include <SDL2/SDL.h>
#include <fstream>

bool ekg::debug {};
int32_t ekg::hovered::id {};
ekg::type ekg::hovered::type {};

std::string ekg::utf8char32(char32_t ui32char) {
    std::string result {};

    if (ui32char < 0x80) {
        result += static_cast<char>(ui32char);
    } else if (ui32char < 0x800) {
        result += static_cast<char>(0xC0 | (ui32char >> 6));
        result += static_cast<char>(0x80 | (ui32char & 0x3F));
    } else if (ui32char < 0x10000) {
        result += static_cast<char>(0xE0 | (ui32char >> 12));
        result += static_cast<char>(0x80 | ((ui32char >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (ui32char & 0x3F));
    } else if (ui32char < 0x110000) {
        result += static_cast<char>(0xF0 | (ui32char >> 18));
        result += static_cast<char>(0x80 | ((ui32char >> 12) & 0x3F));
        result += static_cast<char>(0x80 | ((ui32char >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (ui32char & 0x3F));
    }

    return result;
}

char32_t ekg::char32str(std::string_view string) {
    char32_t value {};
    size_t bytesread {};
    uint8_t ui8char {static_cast<uint8_t>(string.at(0))};

    if (ui8char <= 0x7F) {
        value = ui8char;
        bytesread = 1;
    } else if (ui8char <= 0xDF) {
        value = ui8char & 0x1F;
        bytesread = 2;
    } else if (ui8char <= 0xEF) {
        value = ui8char & 0x0F;
        bytesread = 3;
    } else {
        value = ui8char & 0x07;
        bytesread = 4;
    }

    for (size_t it {1}; it < bytesread; ++it) {
        ui8char = static_cast<uint8_t>(string.at(it));
        value = (value << 6) | (ui8char & 0x3F);
    }

    return value > 512 ? 32 : value;
}

size_t ekg::utf8length(std::string_view utf8tstring) {
    if (utf8tstring.empty()) {
        return 0;
    }

    size_t stringsize {};
    uint8_t ui8char {};

    for (size_t it {}; it < utf8tstring.size(); it++) {
        ui8char = static_cast<uint8_t>(utf8tstring.at(it));
        if (ui8char <= 0x7F) {
            stringsize++;
        } else if ((ui8char & 0xE0) == 0xC0) {
            stringsize++;
            it++;
        } else if ((ui8char & 0xF0) == 0xE0) {
            stringsize++;
            it += 2;
        } else if ((ui8char & 0xF8) == 0xF0) {
            stringsize++;
            it += 3;
        }
    }

    return stringsize;
}

std::string ekg::utf8substr(std::string_view string, size_t a, size_t b) {
    std::string substred {};
    if (string.empty()) {
        return substred;
    }

    size_t stringsize {ekg::utf8length(string)};

    if (stringsize == string.size()) {
        substred = string.substr(a, b);
    } else {
        if (b <= a) {
            b += a;
        }

        uint8_t ui8char {};
        bool indexafilled {};
        bool indexbfilled {};

        size_t index {};
        size_t sumindex {};
        size_t indexa {};
        size_t indexb {};
        size_t itsub {};

        std::string astring {};
        std::string bstring {};

        for (size_t it {}; it < string.size(); it++) {
            ui8char = static_cast<uint8_t>(string.at(it));

            if (ui8char <= 0x7F) {
                sumindex = 0;
            } else if ((ui8char & 0xE0) == 0xC0) {
                sumindex = 1;
            } else if ((ui8char & 0xF0) == 0xE0) {
                sumindex = 2;
            } else if ((ui8char & 0xF8) == 0xF0) {
                sumindex = 3;
            }

            if (!indexafilled && index == a) {
                indexa = it;
                indexafilled = true;
            }

            if (!indexbfilled && index == b) {
                indexb = it;
                indexbfilled = true;
                break;
            }

            if (indexafilled) {
                itsub = 0;
                while ((itsub < (sumindex + 1) || itsub == 0)) {
                    substred += string.at(it + (itsub++));
                }
            }

            index++;
            it += sumindex;
        }

    }

    return substred;
}

/*
 * This function have a potentially memory leak issue,
 * and is very dangerous.
 */
void ekg::utf8read(std::string_view string, std::vector<std::string> &utf8_read) {
    if (string.empty()) {
        return;
    }

    std::string fragment {};
    fragment += string;

    uint64_t index {};
    while ((index = fragment.find('\n')) < fragment.size()) {
        if (index > 0 && fragment.at(index - 1) == '\r') {
            utf8_read.emplace_back(fragment.substr(0, index - 1));
        } else {
            utf8_read.emplace_back(fragment.substr(0, index));
        }

        fragment = fragment.substr(index + 1, fragment.size());
    }

    if (!fragment.empty()) utf8_read.emplace_back(fragment);
}

bool ekg::bitwise::contains(uint16_t target, uint16_t flags) {
    return target & (flags);
}

uint16_t &ekg::bitwise::remove(uint16_t &target, uint16_t flags) {
    target &= ~(flags);
    return target;
}

uint16_t &ekg::bitwise::add(uint16_t &target, uint16_t flags) {
    target |= flags;
    return target;
}

bool ekg::file_to_string(std::string &string_builder, const std::string &path) {
    std::ifstream ifs {path};

    if (ifs.is_open()) {
        std::string string_buffer {};

        while (getline(ifs, string_buffer)) {
            string_builder += "\n" + string_buffer;
        }

        ifs.close();
        return true;
    }

    return false;
}

bool ekg::reach(ekg::timing &timing, uint64_t ms) {
    timing.ticks_going_on = SDL_GetTicks64();
    timing.current_ticks = timing.ticks_going_on - timing.elapsed_ticks;
    return timing.current_ticks > ms;
}

bool ekg::reset(ekg::timing &timing) {
    timing.elapsed_ticks = timing.ticks_going_on;
    return true;
}

float ekg::lerp(float a, float b, float dt) {
    if (dt < 0 || dt >= 1) {
        return b;
    }

    return a + (b - a) * dt;
}

bool ekg::set(bool &var_mutable, bool predicate) {
    if (var_mutable != predicate) {
        ekg::dispatch(ekg::env::redraw);
    }

    return (var_mutable = predicate);
}

std::string &ekg::set(std::string &var_mutable, const std::string &predicate) {
    if (var_mutable != predicate) {
        ekg::dispatch(ekg::env::redraw);
    }

    return (var_mutable = predicate);
}

int32_t &ekg::set(int32_t &var_mutable, int32_t predicate) {
    if (var_mutable != predicate) {
        ekg::dispatch(ekg::env::redraw);
    }

    return (var_mutable = predicate);
}

std::string ekg::string_float_precision(float n, int32_t precision) {
    const std::string string {std::to_string(n)};
    return string.substr(0, ekg::max((int32_t) (string.find('.') + precision + (1 * precision)), (int32_t) string.size()));
}

bool ekg::input::pressed(std::string_view key_input) {
    return ekg::core->get_service_input().pressed(key_input);
}

void ekg::input::bind(std::string_view key_input, std::string_view value_input) {
    return ekg::core->get_service_input().bind(key_input, value_input);
}

bool ekg::input::motion() {
    return ekg::core->get_service_input().was_motion();
}

bool ekg::input::released() {
    return ekg::core->get_service_input().was_released();
}

bool ekg::input::pressed() {
    return ekg::core->get_service_input().was_pressed();
}

bool ekg::input::wheel() {
    return ekg::core->get_service_input().was_wheel();
}

bool ekg::input::receive(std::string_view tag) {
    return ekg::core->get_service_input().receive(tag);
}
