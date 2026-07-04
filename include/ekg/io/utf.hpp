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
#ifndef EKG_IO_UTF_HPP
#define EKG_IO_UTF_HPP

#include "ekg/math/geometry.hpp"

#include <stdint.h>
#include <string>
#include <vector>
#include <regex>
#include <list>

namespace ekg {
  /**
   * Check utf8 sequence and add to `it` index; checking for the first byte of a char utf8 encoded:
   * 
   * * - If first char fit less than 127; 1 byte utf8 char.
   * * - If first char fit equals 192~224; 2 byte utf8 char.
   * * - If first char fit equals 224~240; 3 byte utf8 char.
   * * - If first char fit equals 240~248; 4 byte utf8 char.
   *
   * By the amount of bytes per-char it is added to `it` e.g:
   * * - If 224~240; then `it + 2`  (`it` already counts as 1 byte)
   * 
   * @param `uc8` the current char to be checked.
   * @param `c32` the post utf32 char converted.
   * @param `utf8_str` the utf8 encoded text.
   * @param `it` the current `utf8_str` index.
   *
   **/
  void utf8_sequence(
    uint8_t &uc8,
    char32_t &c32,
    std::string_view utf8_str,
    size_t &it
  );

  /**
   * Find for an aligned utf position by byte position:
   *
   * * - If no utf position was found then `utf_pos` keeps unchanged.
   * * - If an invalid byte position is passed, e.g a position inside a
   *   utf sequence, it return atuomatically `false`.
   * 
   * @param `string` string for find utf position
   * @param `byte_pos` byte pos used for find utf position
   * @param `utf_pos` utf position relative to byte position
   * 
   * @return `true` if a valid utf position was found else `false` if was not found
   **/
  bool utf8_find_utf_pos_by_byte_pos(
    std::string &string,
    size_t byte_pos,
    size_t &utf_pos
  );

  /**
   * Find for a byte pos from an aligned utf position:
   * * - If no byte position was found then `utf_byte` keeps unchanged.
   * 
   * @param `string` string for find utf position
   * @param `utf_pos` utf pos used for find byte pos
   * @param `byte_pos` byte pos relative to utf position
   * 
   * @return `true` if a valid byte position was found else `false` if was not found
   **/
  bool utf8_find_byte_pos_by_utf_pos(
    std::string &string,
    size_t utf_pos,
    size_t &byte_pos
  );

  /**
   * Align utf position and byte-position by the next byte-position:
   * * - If next byte pos is greater than unaligned byte pos, then move to left; if not move to right.
   * 
   * May output be corrupted if current unaligned byte-pos is an invalid utf position.
   * 
   * @param `string` string for be used in alignment
   * @param `unaligned_byte_pos` byte pos to be aligned
   * @param `unaligned_utf_pos` utf pos to be aligned
   * @param `next_byte_pos` next byte pos to align byte-pos and utf-pos
   * 
   * @return true if could align, else false if could not
   **/
  bool utf8_align_utf_pos_by_byte_pos(
    std::string &string,
    size_t &unaligned_byte_pos,
    size_t &unaligned_utf_pos,
    size_t next_byte_pos
  );

  /**
   * Get the first nearest group matched byte-position:
   * * - For left dock the first matched group position is assigned;
   * * - For right dock the last matched group position is assigned.
   * 
   * @param `begin` const string iterator to begin regex matching
   * @param `end` const string iterator to end regex matching
   * @param `nearest_byte_pos` first group matched bidirectional byte-position
   *
   * @return true if matched else false if not matched
   **/
  bool utf8_nearest_regex_group_matched_position(
    const std::string::const_iterator &begin,
    const std::string::const_iterator &end,
    size_t &nearest_byte_pos,
    std::regex &pattern,
    const ekg::dock &dock
  );

  /**
   * Check if current index is last index by the last utf char size.
   * 
   * @param `byte_index` the current index in byte to be checked
   * @param `last_char_bytes` the utf size (1 | 2 | 3 | 4 bytes per utf char)
   * @param `string_bytes_size` the string size in bytes
   * 
   * @return true if is last index else false if not last index
   **/
  bool utf8_is_last_index(
    size_t byte_index,
    size_t last_char_bytes,
    size_t string_bytes_size
  );

  /**
   * Check for last char utf8 bytes sequence.
   * 
   * @param `utf8_str` the utf8 valid string to find
   * @param `bytes` the last char utf8 bytes output
   * 
   * @return true if found-valid else false if not found
   **/
  bool utf8_check_last_char_byte_sequence(
    std::string &utf8_str,
    size_t &bytes
  );

  /**
   * Returns a UTF string by `char32` converting
   * the UTF-32 unique char into a sequence of UTF-8
   * chars.
   */
  std::string utf32_to_string(
    char32_t char32
  );

  /**
   * Returns a string subtracted by stride begin `offset`
   * and end index `size`. If string is empty, return
   * empty.
   */
  std::string utf8_substr(
    std::string_view string,
    uint64_t offset,
    uint64_t size
  );

  /**   * Returns the `string` length considering UTF chars.
   */
  uint64_t utf8_length(
    std::string_view string
  );

  /**
   * Returns index size that represent an UTF-8 char.
   * Possibles:
   * 3, 2, 1, and 0.
   */
  uint64_t utf8_check_sequence(
    uint8_t &char8,
    char32_t &char32,
    std::string &utf_string,
    std::string_view string,
    uint64_t index
  );

  /**
   * Returns a UTF-32 char32 based on UTF-8 sequence.
   */
  char32_t utf8_to_utf32(
    std::string_view string
  );

  size_t utf8_split_endings(
    std::string_view line,
    std::vector<std::string> &splitted
  );

  template<typename t>
  void utf8_number_precision(
    std::string &number_to_string,
    t number,
    size_t precision
  ) {
    number_to_string = std::to_string(number);
    number_to_string = (
      number_to_string.substr(
        0,
        ekg::clamp_max<t>(
          number_to_string.find('.') + precision + (1 * precision),
          number_to_string.size()
        )
      )
    );  
  }

  void utf8_concat(
    std::string &string,
    const ekg::vec4_t<size_t> &stride,
    std::string &concated
  );
}

namespace ekg::io {
  typedef std::vector<std::string> chunk_t;
}

namespace ekg {
  class text {
  protected:
    std::list<ekg::io::chunk_t> loaded_chunks {};
    size_t lines_per_chunk_limit {100000};
    size_t total_lines {};
    size_t total_chars {};
    size_t prev_lines {};
    size_t prev_total_chars {};

    bool was_audited {};
    bool should_count {};
  protected:
    void swizzle(
      std::list<ekg::io::chunk_t>::iterator chunk_it,
      size_t line_index,
      std::vector<std::string> &to_swizzle,
      bool skip_first_line
    );
  public:
    void push_back(std::string_view line);
    size_t set(size_t index, std::string_view line, ekg::io::chunk_t &split_endings);
    size_t set(size_t index, std::string_view line);

    std::string read(
      ekg::vec2_t<size_t> &begin,
      ekg::vec2_t<size_t> &end
    );

    void insert(
      size_t index,
      const ekg::io::chunk_t &to_insert_chunk
    );
    
    void insert(
      size_t index,
      std::string_view line
    );

    void erase(
      size_t begin,
      size_t end
    );

    std::list<ekg::io::chunk_t> &chunks_data();
    size_t length_of_chunks();

    std::string at(size_t index);
    size_t length_of_lines(bool force = false);
    size_t length_of_chars();

    bool audited();
    void unset_audited();
    void gc();
    void sanitize();
  
    void set_lines_per_chunk_limit(size_t limit);
    size_t get_lines_per_chunk_limit();
  };
}

#endif
