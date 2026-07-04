/**
 * MIT License
 * 
 * Copyright (c) 2022-2025 Rina Wilk / vokegpu@gmail.com
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
#include "ekg/io/utf.hpp"

#include <cstring>
#include <sstream>
#include <ostream>

bool ekg::utf8_is_last_index(
  size_t byte_index,
  size_t last_char_bytes,
  size_t string_bytes_size
) {
  return byte_index == (string_bytes_size - last_char_bytes);
}

bool ekg::utf8_align_utf_pos_by_byte_pos(
  std::string &string,
  size_t &unaligned_byte_pos,
  size_t &unaligned_utf_pos,
  size_t next_byte_pos
) {
  if (string.empty() || next_byte_pos == unaligned_byte_pos) {
    return false;
  }

  ekg::dock axis {
    next_byte_pos > unaligned_byte_pos
    ? ekg::dock::left : ekg::dock::right 
  };

  size_t byte_pos {};
  size_t utf_pos_count {};
  size_t utf_sequence_size {};

  switch (axis) {
  case ekg::dock::left:
    byte_pos = unaligned_byte_pos;
    while (byte_pos < next_byte_pos) {
      char &c8 {string.at(byte_pos)};
      utf_sequence_size = 1;
      utf_sequence_size += 1 * ((c8 & 0xE0) == 0xC0);
      utf_sequence_size += 2 * ((c8 & 0xF0) == 0xE0);
      utf_sequence_size += 3 * ((c8 & 0xF8) == 0xF0);
      byte_pos += utf_sequence_size;
      unaligned_utf_pos++;
    }

    unaligned_byte_pos = next_byte_pos;

    break;
  case ekg::dock::right:
    byte_pos = next_byte_pos;
    while (byte_pos < unaligned_byte_pos) {
      char &c8 {string.at(byte_pos)};
      utf_sequence_size = 1;
      utf_sequence_size += 1 * ((c8 & 0xE0) == 0xC0);
      utf_sequence_size += 2 * ((c8 & 0xF0) == 0xE0);
      utf_sequence_size += 3 * ((c8 & 0xF8) == 0xF0);
      byte_pos += utf_sequence_size;
      unaligned_utf_pos--;
    }

    unaligned_byte_pos = next_byte_pos;

    break;
  default:
    break;
  }

  return true;
}

bool ekg::utf8_nearest_regex_group_matched_position(
  const std::string::const_iterator &begin,
  const std::string::const_iterator &end,
  size_t &nearest_byte_pos,
  std::regex &pattern,
  const ekg::dock &dock
) {
  std::sregex_iterator sregex_it(begin, end, pattern);
  std::sregex_iterator sregex_it_end {};

  if (sregex_it == sregex_it_end) {
    return false;
  }
  
  std::smatch match {};
  size_t groups_size {};

  bool any_matched {};
  bool is_right {dock == ekg::dock::right};

  for (; sregex_it != sregex_it_end; sregex_it++) {
    match = *sregex_it;
    groups_size = match.size();
    for (size_t i {1}; i < groups_size; i++) {
      if (match[i].matched) {
        any_matched = true;
        nearest_byte_pos = match.position(i);
        if (is_right) return true;
      }
    }
  }

  return any_matched;
}

void ekg::utf8_sequence(
  uint8_t &uc8,
  char32_t &c32,
  std::string_view utf8_str,
  size_t &it
) {
  size_t size {utf8_str.size()};

  if (size >= 1 && uc8 <= 0x7F) {
    c32 = static_cast<char32_t>(uc8);
  } else if ((size >= 2 && it + 1 < size) && (uc8 & 0xE0) == 0xC0) {
    c32 = uc8 & 0x1F;
    c32 = (c32 << 6) | (utf8_str.at(++it) & 0x3F);
  } else if ((size >= 3 && it + 2 < size) && (uc8 & 0xF0) == 0xE0) {
    c32 = uc8 & 0x0F;
    c32 = (c32 << 6) | (utf8_str.at(++it) & 0x3F);
    c32 = (c32 << 6) | (utf8_str.at(++it) & 0x3F);
  } else if ((size >= 4 && it + 3 < size) && (uc8 & 0xF8) == 0xF0) {
    c32 = uc8 & 0x07;
    c32 = (c32 << 6) | (utf8_str.at(++it) & 0x3F);
    c32 = (c32 << 6) | (utf8_str.at(++it) & 0x3F);
    c32 = (c32 << 6) | (utf8_str.at(++it) & 0x3F);
  }
}

bool ekg::utf8_check_last_char_byte_sequence(
  std::string &utf8_str,
  size_t &bytes
) {
  if (utf8_str.empty()) {
    return false;
  }

  size_t byte {utf8_str.size()};

  /**
   * Reversing find by byte pos is efficient,
   * since we want to reduce the index utf8 check.
   * 
   * It cost many CPU iterations instructions for calculating
   * the string size by utf8 sequence. 
   **/

  return (
    byte
    &&
    (
      (
        byte >= (bytes = 4)
        &&
        (static_cast<uint8_t>(utf8_str.at(byte - 4)) & 0xF8) == 0xF0
      )
      ||
      (
        byte >= (bytes = 3)
        &&
        (static_cast<uint8_t>(utf8_str.at(byte - 3)) & 0xF0) == 0xE0
      )
      ||
      (
        byte >= (bytes = 2)
        &&
        (static_cast<uint8_t>(utf8_str.at(byte - 2)) & 0xE0) == 0xC0)
      ||
      (
        byte >= (bytes = 1)
        &&
        (static_cast<uint8_t>(utf8_str.at(byte - 1))) <= 0xE0
      )
    )
  );
}

bool ekg::utf8_find_utf_pos_by_byte_pos(
  std::string &string,
  size_t byte_pos,
  size_t &utf_pos
) {
  size_t utf_sequence_size {};
  size_t string_size {string.size()};
  size_t utf_pos_count {};
  for (size_t it {}; it < string_size; it++) {
    if (it == byte_pos) {
      utf_pos = utf_pos_count;
      return true;
    }

    char &c8 {string.at(it)};
    utf_sequence_size = 0;
    utf_sequence_size += ((c8 & 0xE0) == 0xC0);
    utf_sequence_size += 2 * ((c8 & 0xF0) == 0xE0);
    utf_sequence_size += 3 * ((c8 & 0xF8) == 0xF0);

    if (it > byte_pos && it + utf_sequence_size < byte_pos) {
      return false;
    }

    it += utf_sequence_size;
    utf_pos_count++;

    if (it == byte_pos) {
      utf_pos = utf_pos_count;
      return true;
    }
  }

  return false;
}

bool ekg::utf8_find_byte_pos_by_utf_pos(
  std::string &string,
  size_t utf_pos,
  size_t &byte_pos
) {
  size_t utf_sequence_size {};
  size_t string_size {string.size()};
  size_t utf_pos_count {};

  size_t it {};
  for (; it < string_size; it++) {
    if (utf_pos_count == utf_pos) {
      byte_pos = it;
      return true;
    }

    char &c8 {string.at(it)};
    utf_sequence_size = 0;
    utf_sequence_size += ((c8 & 0xE0) == 0xC0);
    utf_sequence_size += 2 * ((c8 & 0xF0) == 0xE0);
    utf_sequence_size += 3 * ((c8 & 0xF8) == 0xF0);

    it += utf_sequence_size;
    utf_pos_count++;
  }

  if (utf_pos_count == utf_pos) {
    byte_pos = it;
    return true;
  }

  return false; 
}

uint64_t ekg::utf8_check_sequence(
  uint8_t &c8,
  char32_t &char32,
  std::string &utf_string,
  std::string_view string,
  uint64_t index
) {
  if (c8 <= 0x7F) {
    utf_string = c8;
    char32 = static_cast<char32_t>(c8);
    return 0;
  } else if ((c8 & 0xE0) == 0xC0) {
    utf_string = string.substr(index, 2);
    char32 = ekg::utf8_to_utf32(utf_string);
    return 1;
  } else if ((c8 & 0xF0) == 0xE0) {
    utf_string = string.substr(index, 3);
    char32 = ekg::utf8_to_utf32(utf_string);
    return 2;
  } else if ((c8 & 0xF8) == 0xF0) {
    utf_string = string.substr(index, 4);
    char32 = ekg::utf8_to_utf32(utf_string);
    return 3;
  }

  return 0;
}

std::string ekg::utf32_to_string(char32_t char32) {
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

char32_t ekg::utf8_to_utf32(std::string_view string) {
  char32_t char32 {};

  uint64_t it {};
  uint8_t c8 {static_cast<uint8_t>(string.at(0))};

  if (c8 <= 0x7F) {
    char32 = c8;
  } else if (c8 <= 0xDF) {
    char32 = c8 & 0x1F;
    char32 = (char32 << 6) | (string.at(++it) & 0x3F);
  } else if (c8 <= 0xEF) {
    char32 = c8 & 0x0F;
    char32 = (char32 << 6) | (string.at(++it) & 0x3F);
    char32 = (char32 << 6) | (string.at(++it) & 0x3F);
  } else {
    char32 = c8 & 0x07;
    char32 = (char32 << 6) | (string.at(++it) & 0x3F);
    char32 = (char32 << 6) | (string.at(++it) & 0x3F);
    char32 = (char32 << 6) | (string.at(++it) & 0x3F);
  }

  return char32;
}

uint64_t ekg::utf8_length(std::string_view utf_string) {
  if (utf_string.empty()) {
    return 0;
  }

  uint64_t string_size {};
  uint8_t c8 {};

  for (uint64_t it {}; it < utf_string.size(); it++) {
    c8 = static_cast<uint8_t>(utf_string.at(it));
    if (c8 == '\n' || c8 == '\r') {
      continue;
    } else if (c8 <= 0x7F) {
      string_size++;
    } else if ((c8 & 0xE0) == 0xC0) {
      string_size++;
      it++;
    } else if ((c8 & 0xF0) == 0xE0) {
      string_size++;
      it += 2;
    } else if ((c8 & 0xF8) == 0xF0) {
      string_size++;
      it += 3;
    }
  }

  return string_size;
}

std::string ekg::utf8_substr(std::string_view string, uint64_t offset, uint64_t size) {
  if (string.empty() || size == 0) {
    return "";
  }

  uint64_t string_size {string.size()};
  uint64_t utf_text_size {};       

  //offset = offset > string_size ? string_size : offset;
  size += offset;

  uint64_t index {};
  uint64_t utf_sequence_size {};
  uint64_t begin {UINT64_MAX};
  uint8_t c8 {};
  bool at_last_index {};

  /*
   * This function implementation checks the amount of bytes per char for UTF-8.
   * There is no support for UTF-16 or directly UTF-32.
   */

  while (index < string_size) {
    c8 = static_cast<uint8_t>(string.at(index));

    utf_sequence_size = 1;
    utf_sequence_size += ((c8 & 0xE0) == 0xC0);
    utf_sequence_size += 2 * ((c8 & 0xF0) == 0xE0);
    utf_sequence_size += 3 * ((c8 & 0xF8) == 0xF0);

    at_last_index = index + utf_sequence_size == string_size;

    if ((at_last_index || utf_text_size >= offset) && begin == UINT64_MAX) {
      begin = ekg::clamp_max(index, string_size);
    }

    index += utf_sequence_size;
    utf_text_size++;

    if (
        /**
         * OBS:
         *  If the `offset` paramete is equals to the last UTF-8 string size,
         *  then it continue without substring process.
         **/
        (at_last_index && begin != UINT64_MAX && offset != utf_text_size) ||
        (utf_text_size >= size)
       ) {
      string = string.substr(begin, (index - begin));
      return std::string {string.begin(), string.end()};
    }
  }

  return "";
}

size_t ekg::utf8_split_endings(
  std::string_view line,
  std::vector<std::string> &splitted
) {
  if (line.empty()) {
    return 0;
  }

  size_t new_lines_count {};
  size_t str_size {line.size()};
  size_t start_pos {};
  size_t pos {};

  while ((pos = line.find('\n', start_pos)) != std::string::npos) {
    std::string &split {splitted.emplace_back()};
    split = line.substr(start_pos, pos - start_pos);
    if (split.back() == '\r') split.pop_back();
    start_pos = pos + 1;
    new_lines_count++;
  }
  
  if (start_pos < str_size) {
    splitted.emplace_back() = line.substr(start_pos, str_size);
    new_lines_count++;
  }

  return new_lines_count;
}

void ekg::utf8_concat(
  std::string &string,
  const ekg::vec4_t<std::size_t> &stride,
  std::string &concated
) {
  std::string a {
    ekg::utf8_substr(string, stride.x, stride.y)
  };

  std::string b {
    ekg::utf8_substr(string, stride.z, stride.w)
  };

  concated.clear();
  if (!a.empty() || !b.empty()) {
    concated = a + b;
  }
}

void ekg::text::swizzle(
  std::list<ekg::io::chunk_t>::iterator chunk_it,
  size_t line_index,
  std::vector<std::string> &to_swizzle,
  bool skip_first_line
) {
  ekg_log_low_level("swizzle...")

  ekg::io::chunk_t &chunk {*chunk_it};

  this->was_audited = true;
  bool is_empty {to_swizzle.empty()};

  if (skip_first_line) {
    chunk.at(line_index) = is_empty ? "" : to_swizzle.at(0);
  }

  if (is_empty) {
    to_swizzle.emplace_back();
  }

  chunk.insert(
    chunk.begin() + line_index + 1,
    to_swizzle.begin() + skip_first_line,
    to_swizzle.end()
  );

  if (chunk.size() <= this->lines_per_chunk_limit) {
    return;
  }

  std::vector<std::string> to_swizzle_chunk {
    chunk.begin() + this->lines_per_chunk_limit,
    chunk.end()
  };

  chunk.erase(
    chunk.begin() + this->lines_per_chunk_limit,
    chunk.end()
  );

  size_t to_swizzle_chunk_size {to_swizzle_chunk.size()};

  if (++chunk_it == this->loaded_chunks.end()) {
    size_t newly_chunks {
      to_swizzle_chunk_size / this->lines_per_chunk_limit          
    };

    ekg_log_low_level("insert at end if necessary")

    for (size_t jt {}; jt < newly_chunks; jt++) {
      chunk_it = this->loaded_chunks.insert(
        chunk_it,
        ekg::io::chunk_t {
          to_swizzle_chunk.begin() + (this->lines_per_chunk_limit * (jt + 0)),
          to_swizzle_chunk.begin() + (this->lines_per_chunk_limit * (jt + 1))
        }
      );
    }

    size_t rest {to_swizzle_chunk_size - (this->lines_per_chunk_limit * newly_chunks)};
    if (rest > 0) {
      chunk_it = this->loaded_chunks.insert(
        chunk_it,
        ekg::io::chunk_t {
          to_swizzle_chunk.begin() + (this->lines_per_chunk_limit * (newly_chunks + 0)),
          to_swizzle_chunk.end()
        }
      );
    }

    return;
  }

  ekg::io::chunk_t &next_chunk {
    *chunk_it
  };

  if (to_swizzle_chunk_size + next_chunk.size() > this->lines_per_chunk_limit) {
    chunk_it = this->loaded_chunks.insert(chunk_it, {});
    ekg_log_low_level("insert if next chunk too large for this")
    this->swizzle(
      chunk_it,
      0,
      to_swizzle_chunk,
      false
    );
    return;
  }

  next_chunk.insert(
    next_chunk.begin(),
    to_swizzle_chunk.begin(),
    to_swizzle_chunk.end()
  );

  ekg_log_low_level("insert if next chunk allows")
}

size_t ekg::text::set(size_t index, std::string_view line) {
  ekg::io::chunk_t split_endings {};
  return this->set(index, line, split_endings);
}

size_t ekg::text::set(size_t index, std::string_view line, ekg::io::chunk_t &split_endings) {
  size_t current_lines {};
  size_t previous_lines {};
  size_t chunk_size {};

  ekg::utf8_split_endings(line, split_endings);

  bool ok {};
  for (std::list<ekg::io::chunk_t>::iterator it {this->loaded_chunks.begin()}; it != this->loaded_chunks.end(); it++) {
    ekg::io::chunk_t &chunk {*it};

    previous_lines = current_lines;
    current_lines += (chunk_size = chunk.size());

    if (
      !ok
      &&
      index < current_lines
      &&
      (index - previous_lines) < chunk_size
    ) {
      this->swizzle(it, (index - previous_lines), split_endings, true);
      this->was_audited = true;
      ok = true;
    }
  }

  if (!ok) throw std::out_of_range("ekg::text::set -> lines length: " + std::to_string(current_lines));
  this->total_lines = current_lines;
  return split_endings.size();
}

std::string ekg::text::at(size_t index) {
  size_t chunks_size {this->loaded_chunks.size()};
  size_t current_lines {};
  size_t previous_lines {};
  size_t chunk_size {};

  for (std::list<ekg::io::chunk_t>::iterator it {this->loaded_chunks.begin()}; it != this->loaded_chunks.end(); it++) {
    ekg::io::chunk_t &chunk {*it};
    previous_lines = current_lines;
    current_lines += (chunk_size = chunk.size());

    if (
      index < current_lines
      &&
      (index - previous_lines) < chunk_size
    ) {
      return chunk.at(index - previous_lines);
    }
  }

  throw std::out_of_range("ekg::text::at -> lines length: " + std::to_string(current_lines));
}

void ekg::text::insert(
  size_t index,
  const ekg::io::chunk_t &to_insert_chunk
) {
  if (this->loaded_chunks.empty()) {
    if (index == 0) {
      this->loaded_chunks.emplace_back();
      this->insert(index, to_insert_chunk);
    }
    return;
  }

  size_t previous_lines {};
  size_t chunk_size {};
  size_t current_lines {};
  size_t to_insert_chunk_size {to_insert_chunk.size()};

  ekg::io::chunk_t splitted {};
  for (size_t it {}; it < to_insert_chunk_size; it++) {
    const std::string &lines {to_insert_chunk.at(it)};
    ekg::utf8_split_endings(lines, splitted);
  }

  size_t b {};

  size_t total_of_chunks {this->loaded_chunks.size()};
  for (std::list<ekg::io::chunk_t>::iterator it {this->loaded_chunks.begin()}; it != this->loaded_chunks.end(); it++) {
    ekg::io::chunk_t &chunk {*it};

    previous_lines = current_lines;
    current_lines += (chunk_size = chunk.size());

    if (
      index < current_lines
      &&
      (index - previous_lines) < chunk_size
    ) {
      this->swizzle(it, (index - previous_lines), splitted, false);
      this->was_audited = true;
      this->should_count = true;
      this->total_lines += splitted.size();
      return;
    }

    b++;
  }

  throw std::out_of_range("ekg::text::insert -> lines length: " + std::to_string(current_lines));
}

void ekg::text::insert(
  size_t index,
  std::string_view line
) {
  ekg::io::chunk_t chunk {};
  chunk.push_back(std::string(line));
  this->insert(index, chunk);
}

std::string ekg::text::read(
  ekg::vec2_t<size_t> &begin,
  ekg::vec2_t<size_t> &end
) {
  size_t previous_lines {};
  size_t chunk_size {};
  size_t current_lines {};
  size_t total_of_chunks {this->loaded_chunks.size()};
  size_t line_index {};
  size_t cut_length {end.y - begin.y};
  size_t cut_count {};
  size_t lines {};

  size_t i {};
  bool oka_begin {};
  bool oka_end {};

  std::string builder {};
  for (std::list<ekg::io::chunk_t>::iterator it {this->loaded_chunks.begin()}; it != this->loaded_chunks.end(); it++) {
    ekg::io::chunk_t &chunk {*it};

    previous_lines = current_lines;
    current_lines += (chunk_size = chunk.size());

    if (
      begin.y > current_lines
    ) {
      continue;
    }

    i = 0;
    if (!oka_begin) {
      i = begin.y - previous_lines;
      oka_begin = true;
    }

    if (end.y < previous_lines) {
      return builder;
    }

    previous_lines += i;

    for (; i < chunk_size; i++) {
      std::string &line {chunk.at(i)};

      if (
        previous_lines == begin.y
        &&
        previous_lines == end.y
      ) {
        builder += ekg::utf8_substr(line, begin.x, end.x - begin.x);
        return builder;
      }

      if (
        previous_lines == begin.y
      ) {
        builder += ekg::utf8_substr(line, begin.x, UINT32_MAX) + EKG_EOF_SYSTEM;
      } else if (
        previous_lines == end.y
      ) {
        builder += ekg::utf8_substr(line, 0, end.x);
        return builder;
      } else {
        builder += line + EKG_EOF_SYSTEM;
      }

      previous_lines++;
    }
  }
}

void ekg::text::erase(
  size_t begin,
  size_t end
) {
  if (this->loaded_chunks.empty()) {
    throw std::out_of_range("ekg::text::erase -> text empty");
    return;
  }

  if (begin > this->total_lines) {
    throw std::out_of_range("ekg::text::erase: " + std::to_string(begin) + ", " + std::to_string(this->total_lines));
    return;
  }

  if (end < begin) {
    throw std::out_of_range("ekg::text::erase: " + std::to_string(end) + " end is > than " + std::to_string(begin) + " begin");
    return;
  }

  this->total_lines -= end - begin;

  this->was_audited = true;
  this->should_count = true;

  size_t previous_lines {};
  size_t chunk_size {};
  size_t lines {};
  size_t remains_lines {};

  bool empty_chunk {};
  bool goto_next_chunk {};

  for (std::list<ekg::io::chunk_t>::iterator it {this->loaded_chunks.begin()}; it != this->loaded_chunks.end(); it++) {
    ekg::io::chunk_t &chunk {*it};

    previous_lines = lines;
    lines += (chunk_size = chunk.size());

    if (begin < lines) {
      remains_lines = end - begin;
      begin = begin - previous_lines;

      ekg_log_low_level(remains_lines << " x " << begin)

      while (remains_lines != 0 && it != this->loaded_chunks.end()) {
        ekg::io::chunk_t &chunk {*it};
        chunk_size = chunk.size();

        if (chunk_size != 0) {
          lines = (remains_lines + begin) >= (chunk_size) ? (chunk_size - begin) : (remains_lines);

          ekg_log_low_level(remains_lines << " " << begin << " | " << lines)

          chunk.erase(
            chunk.begin() + begin, // 5
            chunk.begin() + begin + remains_lines // 5 + 1 = 6
          );

          ekg_log_low_level(remains_lines << " vv " << lines)
          remains_lines *= lines != 0;
          remains_lines -= lines;

          begin = 0;
        }

        if (chunk.empty()) {
          it = this->loaded_chunks.erase(it);
        }

        it++;
      }

      break;
    }
  }
}

void ekg::text::push_back(std::string_view line) {
  ekg::io::chunk_t splitted {};
  ekg::utf8_split_endings(line, splitted);

  this->should_count = true;
  this->total_lines += splitted.size();

  if (this->loaded_chunks.empty()) {
    this->loaded_chunks.emplace_back().emplace_back();
    this->swizzle(this->loaded_chunks.begin(), 0, splitted, true);
    return;
  }

  std::list<ekg::io::chunk_t>::iterator last_it {--this->loaded_chunks.end()};
  ekg::io::chunk_t &last_chunk {
    *last_it
  };

  this->swizzle(
    last_it,
    last_chunk.size() - !last_chunk.empty(),
    splitted,
    false
  );
}

std::list<ekg::io::chunk_t> &ekg::text::chunks_data() {
  return this->loaded_chunks;
}

size_t ekg::text::length_of_chunks() {
  return this->loaded_chunks.size();
}

size_t ekg::text::length_of_lines(bool force) {
  if (force) {
    this->total_lines = 0;
    for (ekg::io::chunk_t &chunk : this->loaded_chunks) {
      this->total_lines += chunk.size();
    }
  }

  return this->total_lines;
}

size_t ekg::text::length_of_chars() {
  if (this->should_count) {
    this->total_chars = 0;
    for (ekg::io::chunk_t &chunk : this->loaded_chunks) {
      for (std::string &lines : chunk) {
        this->total_chars += ekg::utf8_length(lines);
      }
    }

    this->should_count = false;
  }

  return this->total_chars;
}

bool ekg::text::audited() {
  this->was_audited = this->was_audited || (this->prev_lines != this->total_lines);
  this->prev_lines = this->total_lines;
  return this->was_audited;
}

void ekg::text::unset_audited() {
  this->was_audited = false;
}

void ekg::text::set_lines_per_chunk_limit(size_t limit) {
  this->lines_per_chunk_limit = limit;
}

size_t ekg::text::get_lines_per_chunk_limit() {
  return this->lines_per_chunk_limit;
}

void ekg::text::sanitize() {
  if (this->loaded_chunks.empty()) {
    this->loaded_chunks.emplace_back();
  }

  ekg::io::chunk_t &chunk {*this->loaded_chunks.begin()};

  if (chunk.empty()) {
    chunk.emplace_back();
  }
}
