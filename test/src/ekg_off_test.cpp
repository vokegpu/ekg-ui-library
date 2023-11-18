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

struct property {
public:
    const char *p_tag {};
};

template<typename t>
void cout_stride(t &list, t p_it_begin, t p_it_end) {
    size_t t_sizeof {sizeof(t)};

    size_t length {sizeof(list) / t_sizeof};
    std::cout << "len: " << length << std::endl;

    size_t begin {sizeof(p_it_begin) / t_sizeof};
    std::cout << "begin: " << begin << std::endl;

    size_t end {length - (sizeof(p_it_end) / t_sizeof)};
    std::cout << "end: " << end << std::endl;
}

int32_t main() {
    property *p_properties {new property[5]};

    p_properties[0].p_tag = "cat";
    p_properties[1].p_tag = "car";
    p_properties[2].p_tag = "com";
    p_properties[3].p_tag = "cur";
    p_properties[4].p_tag = "col";

    cout_stride(p_properties, p_properties + 1, p_properties + 3);
    return 0;
}