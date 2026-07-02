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
#ifndef EKG_IO_MEMORY_HPP
#define EKG_IO_MEMORY_HPP

#include <iostream>
#include <vector>
#include <cstdint>

/**
 * This is a macro because hash should enjoy of compile-time for generate valid hashes.
 * Not really an expensive job for CPU.
 **/
#define ekg_generate_hash(distance, c32, u) static_cast<ekg::hash_t>(distance + c32 + u * 100);

/**
 * A low-level assert used in risks cases where virtual-address should be warned.
 **/
#define ekg_assert_low_level(state, alarm, end) if (state) { alarm; end; }

/**
 * A dev-purpose log level untracked for EKG.
 **/
#define ekg_log_low_level(log_content) std::cout << log_content << std::endl;

namespace ekg {
  typedef size_t flags_t;
  typedef size_t id_t;
  typedef int32_t hash_t;

  enum result {
    success,
    failed,
    failed_unknown,
    failed_not_implemented
  };

  template<typename t>
  constexpr bool has(ekg::flags_t bits, t bit) {
    return (bits & bit) >= bit;
  }

  template<typename t>
  constexpr bool strip(ekg::flags_t &bits, t bit) {
    bits = bits & ~(bit);
    return ekg::has<t>(bits, bit);
  }

  template<typename t>
  constexpr ekg::flags_t &put(ekg::flags_t &bits, t bit) {
    return (bits |= bit);
  }

  constexpr void assert(bool state, const char *p_msg = "") {
    if (state) return;
    if (p_msg) std::cout << "[EKG] assert failed: " << p_msg;
    std::abort();
  }
}

/**
 * Memory-pool and virtual address.
 **/
namespace ekg {
  constexpr ekg::id_t not_found {
    UINT32_MAX + 52 + 1977
  };

  struct at_t {
  public:
    static ekg::at_t not_found;
  public:
    ekg::id_t unique_id {ekg::not_found};
    size_t index {ekg::not_found};
    ekg::flags_t flags {ekg::not_found};
  public:
    bool operator == (ekg::at_t &at) {
      return (
        this->flags == at.flags
        &&
        this->index == at.index
        &&
        this->unique_id == at.unique_id
      );
    }

    bool operator != (ekg::at_t &at) {
      return !(*this == at);
    }
  };

  template<typename t>
  class pool {
  protected:
    std::vector<t> loaded {};
    ekg::id_t highest_unique_id {};
    size_t virtual_memory_capacity {256};
  public:
    pool() {
      this->loaded.reserve(this->virtual_memory_capacity);
    };

    t &push_back(t copy) {
      this->loaded.push_back(copy);

      size_t index {this->loaded.size() - 1};
      t &descriptor {this->loaded.at(index)};

      descriptor.at.unique_id = this->highest_unique_id++;
      descriptor.at.flags = t::type;
      descriptor.at.index = index;      

      return descriptor;
    }

    t &query(ekg::at_t &at) {
      if (at.index == ekg::not_found) {
        return t::not_found;
      }

      if (
        at.index >= this->loaded.size()
        ||
        this->loaded.at(at.index).at.unique_id != at.unique_id
      ) {
        size_t size {this->loaded.size()};
        for (size_t it {}; it < size; it++) {
          t &descriptor {this->loaded.at(it)};
          descriptor.at.index = it;

          if (descriptor.at.unique_id == at.unique_id) {
            at.index = it;
            return descriptor;
          }
        }
    
        return t::not_found;
      }

      t &descriptor {this->loaded.at(at.index)};
      return descriptor;
    }
  };
}

namespace ekg::io {

  constexpr bool strictly {true};
  constexpr bool non_strictly {false};

  /**
   * @TODO: add a complete docs here please.
   **/

  template<typename t>
  constexpr t *any_static_cast_as_ptr(void *p_any) {
    return static_cast<t*>(p_any);
  }

  template<typename t>
  constexpr t &any_static_cast(void *p_any) {
    return *ekg::io::any_static_cast_as_ptr<t>(p_any);
  }
}

#define ekg_io_memory_ownership_impl(cast_type_t, p) \
  this->p = p; \
  this->changed = true; \
  this->type_info_hash = typeid(cast_type_t).hash_code();

#define ekg_io_memory_strictly_set_impl(cast_type_t, val) \
  this->get() = val; \
  this->changed = true; \
  this->type_info_hash = typeid(cast_type_t).hash_code();

#define ekg_io_memory_strictly_format_impl(type_t, cast_type_t) \
  value(cast_type_t val) { \
    ekg_io_memory_strictly_set_impl(cast_type_t, val); \
  } \
  cast_type_t &set(cast_type_t val) { \
    return (this->get() = val); \
  } \
  ekg::value<type_t> &operator = (cast_type_t val) { \
    ekg_io_memory_strictly_set_impl(cast_type_t, val); \
    return *this; \
  } \
  operator cast_type_t () { \
    return this->get(); \
  }

#define ekg_io_memory_unstrictly_set_impl(cast_type_t, val) \
  ekg::io::any_static_cast<cast_type_t>(&this->get()) = val; \
  this->changed = true; \
  this->type_info_hash = typeid(cast_type_t).hash_code();

#define ekg_io_memory_unstrictly_format_impl(type_t, cast_type_t) \
  value(cast_type_t val) { \
    ekg_io_memory_unstrictly_set_impl(cast_type_t, val); \
  } \
  cast_type_t &set(cast_type_t val) { \
    return (this->get() = val); \
  } \
  ekg::value<type_t> &operator = (cast_type_t val) { \
    ekg_io_memory_unstrictly_set_impl(cast_type_t, val); \
    return *this; \
  } \
  operator cast_type_t () { \
    return this->get(); \
  }

/**
 * Value system.
 **/
namespace ekg {
  template<typename t>
  class value {
  protected:
    t val {};
    t *p {nullptr};
    t previous {};
    bool changed {};
    size_t type_info_hash {};
  public:
    value() {};

    void ownership(t *p) {
      ekg_io_memory_ownership_impl(t, p);
    }

    template<typename s>
    void ownership(s *p) {
      ekg_io_memory_ownership_impl(s, p);
    }

    ekg_io_memory_strictly_format_impl(t, std::string);
    ekg_io_memory_strictly_format_impl(t, const char*);
    ekg_io_memory_strictly_format_impl(t, bool);
    ekg_io_memory_unstrictly_format_impl(t, double);
    ekg_io_memory_unstrictly_format_impl(t, float);
    ekg_io_memory_unstrictly_format_impl(t, uint64_t);
    ekg_io_memory_unstrictly_format_impl(t, int64_t);
    ekg_io_memory_unstrictly_format_impl(t, uint32_t);
    ekg_io_memory_unstrictly_format_impl(t, int32_t);
    ekg_io_memory_unstrictly_format_impl(t, uint16_t);
    ekg_io_memory_unstrictly_format_impl(t, int16_t);
    ekg_io_memory_unstrictly_format_impl(t, uint8_t);
    ekg_io_memory_unstrictly_format_impl(t, int8_t);
  public:  
    t &get() {
      return this->p ? *this->p : this->val;
    }
  
    bool was_changed() {
      if (this->changed) {
        this->changed = false;
        return true;
      }
  
      t &get {this->get()};
      if (this->previous != get) {
        this->previous = get;
        return true;
      }
  
      return false;
    }

    template<typename s>
    bool was_changed_as() {
      if (this->changed) {
        this->changed = false;
        return true;
      }

      s &get {ekg::io::any_static_cast<s>(this->get())};
      s &previous {ekg::io::any_static_cast<s>(this->previous)};

      if (previous != get) {
        previous = get;
        return true;
      }
  
      return false;
    }

    template<typename s>
    s &as() {
      this->type_info_hash = typeid(s).hash_code();
      return ekg::io::any_static_cast<s>(this->get());
    }

    size_t &get_type_info_hash() {
      return this->type_info_hash;
    }

    void debug() {
      ekg_log_low_level(this->p);
      ekg_log_low_level(&this->val);
    }
  };

  struct mapped_address_sign_info_t {
  public:
    std::vector<ekg::at_t> ats {};
    void *pv_address {};
  };

  extern struct signed_address_info_t {
  public:
    std::vector<ekg::mapped_address_sign_info_t> list {};
    size_t current {};
  } sign;

  void map(void *pv_address);
  void unmap(void *pv_address);
}

#endif
