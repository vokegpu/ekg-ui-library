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

#ifndef EKG_UTIL_TEXT_H
#define EKG_UTIL_TEXT_H

#include <iostream>
#include <string>
#include <vector>

namespace ekg {
    std::string utf_ui32_to_string(char32_t ui32_char);
    std::string utf_substr(std::string_view string, size_t a, size_t b);

    size_t utf_length(std::string_view string);
    size_t utf_check_sequence(uint8_t &ui8_char, char32_t &ui32_char, std::string &utf_string, std::string_view string, size_t index);

    char32_t utf_string_to_ui32(std::string_view string);
    void utf_decode(std::string_view string, std::vector<std::string> &utf_decoded);

    std::string string_float_precision(float number, int32_t precision);
}

#endif