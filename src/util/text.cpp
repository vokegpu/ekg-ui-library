/**
 * MIT License
 * 
 * Copyright (c) 2022-2024 Rina Wilk / vokegpu@gmail.com
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
#include <stdint.h>
#include <string>

#include "ekg/util/text.hpp"
#include "ekg/util/geometry.hpp"
#include "ekg/util/io.hpp"
#include "ekg/ui/abstract/ui_abstract.hpp"

uint64_t ekg::utf_check_sequence(
  uint8_t &char8,
  char32_t &char32,
  std::string &utf_string,
  std::string_view string,
  uint64_t index
) {
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
    result.reserve(1);
    result[0] = static_cast<char>(char32);
  } else if (char32 < 0x800) {
    result.reserve(2);
    result[0] += static_cast<char>(0xC0 | (char32 >> 6));
    result[1] += static_cast<char>(0x80 | (char32 & 0x3F));
  } else if (char32 < 0x10000) {
    result.reserve(3);
    result[0] += static_cast<char>(0xE0 | (char32 >> 12));
    result[1] += static_cast<char>(0x80 | ((char32 >> 6) & 0x3F));
    result[2] += static_cast<char>(0x80 | (char32 & 0x3F));
  } else if (char32 < 0x110000) {
    result.reserve(4);
    result[0] = static_cast<char>(0xF0 | (char32 >> 18));
    result[1] = static_cast<char>(0x80 | ((char32 >> 12) & 0x3F));
    result[2] = static_cast<char>(0x80 | ((char32 >> 6) & 0x3F));
    result[3] = static_cast<char>(0x80 | (char32 & 0x3F));
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
  if (string.empty() || size == 0) {
    return "";
  }

  uint64_t string_size {string.size()};
  uint64_t utf_text_size {};       

  //offset = offset > string_size ? string_size : offset;
  size += offset;

  uint64_t index {};
  uint64_t utf_sequence_size {};
  uint64_t substr_size {};
  uint64_t begin {UINT64_MAX};
  uint8_t char8 {};
  bool at_last_index {};

  /*
   * This function implementation checks the amount of bytes per char for UTF-8.
   * There is no support for UTF-16 or directly UTF-32.
   */

  while (index < string_size) {
    char8 = static_cast<uint8_t>(string.at(index));

    utf_sequence_size = 1;
    utf_sequence_size += ((char8 & 0xE0) == 0xC0);
    utf_sequence_size += 2 * ((char8 & 0xF0) == 0xE0);
    utf_sequence_size += 3 * ((char8 & 0xF8) == 0xF0);

    at_last_index = index + utf_sequence_size == string_size;

    if ((at_last_index || utf_text_size >= offset) && begin == UINT64_MAX) {
      begin = ekg_max(index, string_size);
    }

    index += utf_sequence_size;
    utf_text_size++;

    if (
        /*
         * OBS:
         *  If the `offset` parameter is equals to the last UTF-8 string size,
         *  then it continue without substring process.
         */
        (at_last_index && begin != UINT64_MAX && offset != utf_text_size) ||
        (utf_text_size >= size)
       ) {
      string = string.substr(begin, (index - begin));
      return std::string {string.begin(), string.end()};
    }
  }

  return "";
}

/*
 * This function has a potential memory leak issue,
 * and it is very dangerous.
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
  return string.substr(
    0,
    ekg_max(
      static_cast<int32_t>(string.find('.') + precision + (1 * precision)),
      static_cast<int32_t>(string.size())
    )
  );
}

uint8_t ekg::check_attribute_flags(std::string_view text, uint16_t &flags) {
  for (uint8_t it {}; it < text.size(); it++) {
    switch (text.at(it)) {
      case '\t':
        flags |= ekg::attr::separator;
        break;
      case '\\':
        flags |= ekg::attr::box;
        break;
      case '\1':
        flags |= ekg::attr::category;
        break;
      case '\2':
        flags |= ekg::attr::row;
        break;
      case '\3':
        flags |= ekg::attr::row_member;
        break;
      case '\4':
        flags |= ekg::attr::unselectable;
        break;
      default:
        return it;
    }
  }

  return 0;
}

bool ekg::split(
  std::vector<std::string> *p_string_split_list,
  const std::string &string,
  char find_char
) {
  if (p_string_split_list == nullptr) {
    return false;
  }

  std::stringstream ss(string);
  std::string find_string {};

  bool found_flag {};

  while (std::getline(ss, find_string, find_char)) {
    p_string_split_list->push_back(find_string);
    found_flag = true;
  }

  return found_flag;
}