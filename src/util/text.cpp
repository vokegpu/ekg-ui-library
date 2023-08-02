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

#include "ekg/util/text.hpp"
#include "ekg/util/geometry.hpp"

size_t ekg::utf8checksequence(uint8_t &ui8_char, char32_t &ui32_char, std::string &utf_string, std::string_view string, size_t index) {
    if (ui8_char <= 0x7F) {
        ui32_char = static_cast<char32_t>(ui8_char);
        utf_string = ui8_char;
        return 0;
    } else if ((ui8_char & 0xE0) == 0xC0) {
        utf_string = utf_string.substr(index, 2);
        ui32_char = ekg::char32str(utf_string);
        return 1;
    } else if ((ui8_char & 0xF0) == 0xE0) {
        utf_string = utf_string.substr(index, 3);
        ui32_char = ekg::char32str(utf_string);
        return 2;
    } else if ((ui8_char & 0xF8) == 0xF0) {
        utf_string = utf_string.substr(index, 4);
        ui32_char = ekg::char32str(utf_string);
        return 3;
    }

    return 0;
}

std::string ekg::utf8char32(char32_t ui32_char) {
    std::string result {};

    if (ui32_char < 0x80) {
        result += static_cast<char>(ui32_char);
    } else if (ui32_char < 0x800) {
        result += static_cast<char>(0xC0 | (ui32_char >> 6));
        result += static_cast<char>(0x80 | (ui32_char & 0x3F));
    } else if (ui32_char < 0x10000) {
        result += static_cast<char>(0xE0 | (ui32_char >> 12));
        result += static_cast<char>(0x80 | ((ui32_char >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (ui32_char & 0x3F));
    } else if (ui32_char < 0x110000) {
        result += static_cast<char>(0xF0 | (ui32_char >> 18));
        result += static_cast<char>(0x80 | ((ui32_char >> 12) & 0x3F));
        result += static_cast<char>(0x80 | ((ui32_char >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (ui32_char & 0x3F));
    }

    return result;
}

char32_t ekg::char32str(std::string_view string) {
    char32_t value {};
    size_t bytesread {};
    uint8_t ui8_char {static_cast<uint8_t>(string.at(0))};

    if (ui8_char <= 0x7F) {
        value = ui8_char;
        bytesread = 1;
    } else if (ui8_char <= 0xDF) {
        value = ui8_char & 0x1F;
        bytesread = 2;
    } else if (ui8_char <= 0xEF) {
        value = ui8_char & 0x0F;
        bytesread = 3;
    } else {
        value = ui8_char & 0x07;
        bytesread = 4;
    }

    for (size_t it {1}; it < bytesread; ++it) {
        ui8_char = static_cast<uint8_t>(string.at(it));
        value = (value << 6) | (ui8_char & 0x3F);
    }

    return value > 512 ? 32 : value;
}

size_t ekg::utf8length(std::string_view utf_string) {
    if (utf_string.empty()) {
        return 0;
    }

    size_t stringsize {};
    uint8_t ui8_char {};

    for (size_t it {}; it < utf_string.size(); it++) {
        ui8_char = static_cast<uint8_t>(utf_string.at(it));

        if (ui8_char == '\n' || ui8_char == '\r') {
            continue;
        } else if (ui8_char <= 0x7F) {
            stringsize++;
        } else if ((ui8_char & 0xE0) == 0xC0) {
            stringsize++;
            it++;
        } else if ((ui8_char & 0xF0) == 0xE0) {
            stringsize++;
            it += 2;
        } else if ((ui8_char & 0xF8) == 0xF0) {
            stringsize++;
            it += 3;
        }
    }

    return stringsize;
}

std::string ekg::utf8substr(std::string_view string, size_t a, size_t b) {
    if (string.empty()) {
        return "";
    }

    size_t stringsize {ekg::utf8length(string)};
    std::string substred {};

    if (stringsize == string.size()) {
        substred = string.substr(a, b);
    } else {
        b += a;

        uint8_t ui8_char {};
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
            ui8_char = static_cast<uint8_t>(string.at(it));

            if (ui8_char <= 0x7F) {
                sumindex = 0;
            } else if ((ui8_char & 0xE0) == 0xC0) {
                sumindex = 1;
            } else if ((ui8_char & 0xF0) == 0xE0) {
                sumindex = 2;
            } else if ((ui8_char & 0xF8) == 0xF0) {
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

std::string ekg::string_float_precision(float n, int32_t precision) {
    const std::string string {std::to_string(n)};
    return string.substr(0, ekg::max((int32_t) (string.find('.') + precision + (1 * precision)), (int32_t) string.size()));
}