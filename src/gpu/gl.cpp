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

#include <vector>

#include "ekg/gpu/gl.hpp"
#include "ekg/os/platform.hpp"
#include "ekg/util/io.hpp"
#include "ekg/os/ekg_opengl.hpp"

void ekg::gpu::invoke(ekg::gpu::program &program) {
    glUseProgram(program.id);
}

void ekg::gpu::revoke() {
    glUseProgram(0);
}

bool ekg::gpu::create_basic_program(ekg::gpu::program &program, const std::unordered_map<std::string, uint32_t> &resources) {
    if (resources.empty()) {
        ekg::log() << "Error: Invalid shader, empty resources";
        return true;
    }

    std::string shader_src {};
    std::vector<uint32_t> compiled_shader_list {};
    int32_t status {};
    program.id = glCreateProgram();

    std::string msg  {};
    for (auto &[value, key] : resources) {
        uint32_t shader {glCreateShader(key)};
        const char *p_src {value.c_str()};
        glShaderSource(shader, 1, &p_src, nullptr);
        glCompileShader(shader);

        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &status);
            msg.resize(status);
            glGetShaderInfoLog(shader, status, nullptr, msg.data());
            ekg::log() << "Error: Failed to compile shader stage: \n" << msg;
            break;
        }

        compiled_shader_list.push_back(shader);
    }

    bool keep {compiled_shader_list.size() == resources.size()};

    for (uint32_t &shaders : compiled_shader_list) {
        if (keep) {
            glAttachShader(program.id, shaders);
        }

        glDeleteShader(shaders);
    }

    if (keep) {
        glLinkProgram(program.id);
        glGetProgramiv(program.id, GL_LINK_STATUS, &status);

        if (status == GL_FALSE) {
            glGetProgramiv(program.id, GL_INFO_LOG_LENGTH, &status);
            msg.resize(status);
            glGetProgramInfoLog(program.id, status, nullptr, msg.data());
            ekg::log() << "Error: Failed to link program: \n" << msg;
        }
    }

    return false;
}

void ekg::gpu::program::set(const std::string &str, bool value) {
    glUniform1i(glGetUniformLocation(this->id, str.c_str()), value);
}

void ekg::gpu::program::set(const std::string &str, int32_t value) {
    glUniform1i(glGetUniformLocation(this->id, str.c_str()), value);
}

void ekg::gpu::program::set(const std::string &str, uint32_t value) {
    glUniform1ui(glGetUniformLocation(this->id, str.c_str()), value);
}

void ekg::gpu::program::set(const std::string &str, float value) {
    glUniform1f(glGetUniformLocation(this->id, str.c_str()), value);
}

void ekg::gpu::program::set2(const std::string &str, float *value) {
    glUniform2fv(glGetUniformLocation(this->id, str.c_str()), GL_TRUE, value);
}

void ekg::gpu::program::set3(const std::string &str, float *value) {
    glUniform3fv(glGetUniformLocation(this->id, str.c_str()), GL_TRUE, value);
}

void ekg::gpu::program::set4(const std::string &str, float *value) {
    glUniform4fv(glGetUniformLocation(this->id, str.c_str()), GL_TRUE, value);
}

void ekg::gpu::program::setm2(const std::string &str, float *matrix) {
    glUniformMatrix2fv(glGetUniformLocation(this->id, str.c_str()), GL_TRUE, GL_FALSE, matrix);
}

void ekg::gpu::program::setm3(const std::string &str, float *matrix) {
    glUniformMatrix3fv(glGetUniformLocation(this->id, str.c_str()), GL_TRUE, GL_FALSE, matrix);
}

void ekg::gpu::program::setm4(const std::string &str, float *matrix) {
    glUniformMatrix4fv(glGetUniformLocation(this->id, str.c_str()), GL_TRUE, GL_FALSE, matrix);
}