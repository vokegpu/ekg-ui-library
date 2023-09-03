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

#ifndef EKG_GPU_GL_H
#define EKG_GPU_GL_H

#include "ekg/os/ekg_opengl.hpp"
#include <iostream>
#include <string>
#include <unordered_map>

namespace ekg::gpu {
    struct program {
        GLuint id {};

        void set(const std::string &str, bool value);
        void set(const std::string &str, GLuint value);
        void set(const std::string &str, GLint value);
        void set(const std::string &str, GLfloat value);
        void set2(const std::string &str, GLfloat* value);
        void set3(const std::string &str, GLfloat* value);
        void set4(const std::string &str, GLfloat* value);
        void setm2(const std::string &str, GLfloat* matrix);
        void setm3(const std::string &str, GLfloat* matrix);
        void setm4(const std::string &str, float* matrix);
    };

    void invoke(ekg::gpu::program &program);
    void revoke();

    bool create_basic_program(ekg::gpu::program &program, const std::unordered_map<std::string, uint32_t> &resources);
};

#endif