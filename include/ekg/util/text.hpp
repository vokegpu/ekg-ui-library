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

#ifndef EKG_UTIL_TEXT_H
#define EKG_UTIL_TEXT_H

#include <iostream>
#include <string>
#include <vector>

namespace ekg {
  std::string utf_char32_to_string(char32_t char32);

  std::string utf_substr(std::string_view string, uint64_t offset, uint64_t size);

  uint64_t utf_length(std::string_view string);

  uint64_t utf_check_sequence(
    uint8_t &char8,
    char32_t &char32,
    std::string &utf_string,
    std::string_view string,
    uint64_t index
  );

  char32_t utf_string_to_char32(std::string_view string);

  void utf_decode(std::string_view string, std::vector<std::string> &utf_decoded);

  std::string string_float_precision(float number, int32_t precision);

  uint8_t check_attribute_flags(std::string_view text, uint16_t &flags);
}

#endif