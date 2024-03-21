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

#ifndef EKG_UTIL_IO_H
#define EKG_UTIL_IO_H

#include <iostream>
#include <sstream>
#include <vector>
#include <cstdint>
#include <map>

#if defined(__ANDROID__)
#include <android/log.h>
#endif

#include "geometry.hpp"

#define ekg_sign_item_sempahore() if (this->p_semaphore) *this->p_semaphore = true

#define ekg_bitwise_contains(flags, flag) (flags & flag)
#define ekg_bitwise_remove(flags, flag)   (flags = ~(flags & flag))
#define ekg_bitwise_add(flags, flag)      (flags |= flag)

namespace ekg {
  struct item {
  private:
    static ekg::item *p_parent_instance;
    static bool *p_semaphore_instance;
    static bool default_semaphore;
  protected:
    std::vector<ekg::item> child_list {};
    ekg::component geometry_component {};
    std::string value {};

    uint16_t attributes {};
    uint16_t states {};
    bool *p_semaphore {};
  public:
    ekg::item &at(size_t index);

    ekg::item &operator[](size_t index);

    ekg::item &emplace_back();

    ekg::item &insert(std::string_view item_value);

    ekg::item &insert(std::string_view item_value, size_t index);

    ekg::item &insert(ekg::item insert_item);

    ekg::item &insert(ekg::item insert_item, size_t index);

    ekg::item &insert(const std::vector<ekg::item> &item_vector);

    ekg::item &insert(const std::vector<ekg::item> &item_vector, size_t index);

    ekg::item &set_value(size_t index, std::string_view item_value);

    ekg::item &set_attr(size_t index, uint16_t attr_bits);

    ekg::item &set_state(size_t index, uint16_t state_bits);

    ekg::item &erase(size_t begin, size_t end);

    ekg::item &erase(size_t index);

    ekg::item &clear();

    [[nodiscard]] bool empty() const;

    [[nodiscard]] size_t size() const;

    std::vector<ekg::item>::iterator begin();

    std::vector<ekg::item>::iterator end();

    [[nodiscard]] std::vector<ekg::item>::const_iterator cbegin() const;

    [[nodiscard]] std::vector<ekg::item>::const_iterator cend() const;

  public:
    item *p_parent {};
  public:
    std::vector<ekg::item> &vector();

    ekg::component &component();

    ekg::item &set_value(std::string_view item_value);

    std::string_view get_value();

    ekg::item &set_attr(uint16_t attr_bits);

    uint16_t get_attr();

    ekg::item &set_state(uint16_t state_bits);

    uint16_t get_state();

    ekg::item &set_semaphore_address(bool *p_semaphore_address);

    ekg::item &set_semaphore(bool signal);

    bool &get_semaphore();

    bool has_parent();

    bool has_children();

  public:
    item();

    item(std::string_view item_value);

    item(std::string_view item_value, uint16_t attr_bits);

  public:
    ~item();
  };

  struct log {
  public:
    static std::ostringstream buffer;
    static bool buffered;
    static bool tracked;
  public:
    static void flush() {
      if (ekg::log::buffered) {
        std::string p_log {ekg::log::buffer.str()};

        #if defined(__ANDROID__)
        __android_log_print(ANDROID_LOG_VERBOSE, "EKG", "%s", p_log.c_str());
        #else
        std::cout << p_log;
        #endif

        ekg::log::buffer = std::ostringstream {};
        ekg::log::buffered = false;
      }
    }

    template<typename t>
    static void trace(bool should, t trace, bool interrupt_runtime = false) {
      if (!should) {
        return;
      }

      std::cout << "\n[EKG-TRACE] " << trace << std::endl;
      if (interrupt_runtime) {
        std::terminate();
      }
    }

    explicit log() {
      ekg::log::buffered = true;
      ekg::log::buffer << "\n[EKG-INFO] ";
    }

    template<typename t>
    log &operator<<(const t &value) {
      ekg::log::buffer << value;
      return *this;
    }
  };

  /*
   * Attributes are used by item,
   * they can be dynamically converted by string symbols:
   * \t separator
   * \\ box
   * \1 category
   * \2 row
   * \3 row_member
   * \4 unselectable
   */
  enum attr {
    separator = 2 << 2,
    box = 2 << 3,
    category = 2 << 4,
    row = 2 << 5,
    row_member = 2 << 6,
    unselectable = 2 << 7
  };

  struct timing {
  public:
    /**
     * The 1 second counter in ms.
     */
    static int64_t second;

    /**
     * The total running ticks since the application was started.
     */
    static int64_t ticks;
  public:
    int64_t elapsed_ticks {};
    int64_t current_ticks {};
    int64_t ticks_going_on {};
  };

  struct flag {
    bool highlight {};
    bool hovered {};
    bool activity {};
    bool focused {};
    bool state {};
    bool extra_state {};
    bool absolute {};
  };

  enum class clipboard {
    copy = 1073741948, // SDLK_COPY
    cut = 1073741947, // SDLK_CUT
    paste = 1073741949  // SDLK_PASTE
  };
}

namespace ekg {
  bool file_to_string(std::string &file_content, std::string_view path);

  bool reach(ekg::timing &, uint64_t);

  bool reset(ekg::timing &);
}

namespace ekg::input {
  bool action(std::string_view action_key);

  bool receive(std::string_view input_key);

  void clipboard(ekg::clipboard clipboard_op);

  void fire(std::string_view action_key);

  void bind(std::string_view action_key, std::string_view input_key);

  bool motion();

  bool released();

  bool pressed();

  bool wheel();

  bool typed();

  ekg::vec4 &interact();
}

#endif