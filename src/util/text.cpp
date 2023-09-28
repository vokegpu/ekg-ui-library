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

#include <cstring>

#include "ekg/util/text.hpp"
#include "ekg/util/geometry.hpp"
#include "ekg/util/io.hpp"
#include "ekg/ui/abstract/ui_abstract.hpp"

uint64_t ekg::utf_check_sequence(uint8_t &char8, char32_t &char32, std::string &utf_string, std::string_view string, uint64_t index) {
    if (char8 <= 0x7F) {
        utf_string = char8;
        char32 = static_cast<char32_t>(char8);
        return 0;
    } else if ((char8 & 0xE0) == 0xC0) {
        utf_string = string.substr(index, 2);
        char32 = ekg::utf_string_to_char32(utf_string);
        return 1;
    } else if ((char8 & 0xF0) == 0xE0) {
        utf_string = string.substr(index, 3);
        char32 = ekg::utf_string_to_char32(utf_string);
        return 2;
    } else if ((char8 & 0xF8) == 0xF0) {
        utf_string = string.substr(index, 4);
        char32 = ekg::utf_string_to_char32(utf_string);
        return 3;
    }

    return 0;
}

std::string ekg::utf_char32_to_string(char32_t char32) {
    std::string result {};

    if (char32 < 0x80) {
        result += static_cast<char>(char32);
    } else if (char32 < 0x800) {
        result += static_cast<char>(0xC0 | (char32 >> 6));
        result += static_cast<char>(0x80 | (char32 & 0x3F));
    } else if (char32 < 0x10000) {
        result += static_cast<char>(0xE0 | (char32 >> 12));
        result += static_cast<char>(0x80 | ((char32 >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (char32 & 0x3F));
    } else if (char32 < 0x110000) {
        result += static_cast<char>(0xF0 | (char32 >> 18));
        result += static_cast<char>(0x80 | ((char32 >> 12) & 0x3F));
        result += static_cast<char>(0x80 | ((char32 >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (char32 & 0x3F));
    }

    return result;
}

char32_t ekg::utf_string_to_char32(std::string_view string) {
    char32_t char32 {};
    uint8_t bytes_read {};
    uint8_t char8 {static_cast<uint8_t>(string.at(0))};

    if (char8 <= 0x7F) {
        char32 = char8;
        bytes_read = 1;
    } else if (char8 <= 0xDF) {
        char32 = char8 & 0x1F;
        bytes_read = 2;
    } else if (char8 <= 0xEF) {
        char32 = char8 & 0x0F;
        bytes_read = 3;
    } else {
        char32 = char8 & 0x07;
        bytes_read = 4;
    }

    for (uint8_t it {1}; it < bytes_read; ++it) {
        char8 = static_cast<uint8_t>(string.at(it));
        char32 = (char32 << 6) | (char8 & 0x3F);
    }

    return char32 > 512 ? 32 : char32;
}

uint64_t ekg::utf_length(std::string_view utf_string) {
    if (utf_string.empty()) {
        return 0;
    }

    uint64_t string_size {};
    uint8_t char8 {};

    for (uint64_t it {}; it < utf_string.size(); it++) {
        char8 = static_cast<uint8_t>(utf_string.at(it));
        if (char8 == '\n' || char8 == '\r') {
            continue;
        } else if (char8 <= 0x7F) {
            string_size++;
        } else if ((char8 & 0xE0) == 0xC0) {
            string_size++;
            it++;
        } else if ((char8 & 0xF0) == 0xE0) {
            string_size++;
            it += 2;
        } else if ((char8 & 0xF8) == 0xF0) {
            string_size++;
            it += 3;
        }
    }

    return string_size;
}

std::string ekg::utf_substr(std::string_view string, uint64_t offset, uint64_t size) {
    if (string.empty()) {
        return "";
    }

    uint64_t string_size {string.size()};
    uint64_t utf_text_size {};

    offset = offset > string_size ? string_size : offset;
    size += offset;

    std::string result {};
    bool entered {};

    uint64_t insert_size {};
    uint64_t index {};
    uint8_t utf_sequence_size {};
    uint8_t char8 {};

    /*
     * This function implementation checks the amount of bytes per char for UTF-8.
     * There is no support for UTF-16 or UTF-32 still.
     *
     * The resize is function is not the best approach, not in this case, considering
     * that UTF chars can be represented by a maximum of only 4 chars.
     */

    while (index < string_size) {
        char8 = static_cast<uint8_t>(string.at(index));
        entered = utf_text_size >= offset && utf_text_size < size;

        if (char8 <= 0x7F) {
            utf_sequence_size = 1;
            if (entered) result += char8;
        } else if ((char8 & 0xE0) == 0xC0) {
            utf_sequence_size = 2;
            if (entered) {
                result += char8;
                result += string.at(index + 1);
            }
        } else if ((char8 & 0xF0) == 0xE0) {
            utf_sequence_size = 3;
            if (entered) {
                result += char8;
                result += string.at(index + 1);
                result += string.at(index + 2);
            }
        } else if ((char8 & 0xF8) == 0xF0) {
            utf_sequence_size = 4;
            if (entered) {
                result += char8;
                result += string.at(index + 1);
                result += string.at(index + 2);
                result += string.at(index + 4);
            }
        }

        // I will let this code here <3.
        //if (utf_text_size >= offset && utf_text_size < size) {
        //    insert_size = result.size();
        //    result.resize(insert_size + utf_sequence_size);
        //    memcpy(&result.at(insert_size), &string.at(index), utf_sequence_size);

        if (utf_text_size > size) {
            break;
        }

        index += utf_sequence_size;
        utf_text_size++;
    }

    return result;
}

/*
 * This function have a potential memory leak issue,
 * and is very dangerous.
 */
void ekg::utf_decode(std::string_view string, std::vector<std::string> &utf8_read) {
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

uint8_t ekg::check_item_flags(std::string_view name, uint16_t &flags) {
    for (uint8_t it {}; it < 3 && !name.empty(); it++) {
        switch (name.at(it)) {
        case '\t':
            flags |= ekg::attribute::separator;
            break;
        case '\\':
            flags |= ekg::attribute::box;
            break;
        default:
            return it;
        }
    }
}