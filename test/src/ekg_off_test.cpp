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

#include <iostream>
#include <type_traits>
#include <vector>

namespace rtd {
  class my_own_vector_based : public std::vector<std::string> {
  public:
    void update() {
      std::cout << "omaga hello" << std::endl;
    }
  };
}

struct property {
public:
    uint32_t id {};
};

template<typename t>
void cout_stride(void *p_list, void *p_it_begin, void *p_it_end) {
  size_t t_sizeof {sizeof(t)};

  size_t length {sizeof(p_list) / t_sizeof};
  std::cout << "len: " << length << std::endl;

  size_t begin {sizeof(p_it_begin) / t_sizeof};
  std::cout << "begin: " << begin << std::endl;

  size_t end {length - (sizeof(p_it_end) / t_sizeof)};
  std::cout << "end: " << end << std::endl;
}

template<typename t>
struct kist {
protected:
  size_t cached_length {};
  size_t length {};
  t *p_data {};
protected:
    void resize() {
        this->cached_length += 64;
        t *p_new_size = new t[this->cached_length];

        size_t it {};
        for (size_t it {}; it < this->length; ++it) {
            p_new_size[it] = this->p_data[it];
        }

        delete this->p_data;
        this->p_data = p_new_size;
    }
public:
    kist() {
        this->cached_length = 64;
        this->p_data = new t[64];
    }

    size_t size() {
        return this->length;
    }

    t *&data() {
        return this->p_data;
    }

    t &emplace_back() {
        if (this->length >= this->cached_length) {
            this->resize();
        }

        return this->p_data[this->length++];
    }

    t *begin() {
        return this->p_data;
    }

    t *end() {
        return this->p_data + this->length;
    }
};

int32_t main(int32_t, char**) {
  rtd::my_own_vector_based text_box {};

  text_box.update();
  text_box.insert(text_box.begin(), "hello");

  return 0;
}