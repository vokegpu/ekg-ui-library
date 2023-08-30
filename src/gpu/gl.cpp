/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, free to use in anything (utility, hacking, cheat, game, software).
 * 5- Malware, rat and others virus. We do not care.
 * 6- Do not modify this license under any instance.
 *
 * @VokeGpu 2023 all rights reserved.
 */

#include "ekg/gpu/gl.hpp"
#include "ekg/os/info.hpp"
#include "ekg/util/io.hpp"
#include <vector>

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

    for (auto &[value, key] : resources) {
        uint32_t shader {glCreateShader(key)};
        const char *p_src {value.c_str()};
        glShaderSource(shader, 1, &p_src, nullptr);
        glCompileShader(shader);

        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &status);
            std::string msg {}; msg.resize(status);
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
            std::string msg {}; msg.resize(status);
            glGetProgramInfoLog(program.id, status, nullptr, msg.data());
            ekg::log() << "Error: Failed to link program: \n" << msg;
        }
    }

    return false;
}

void ekg::gpu::program::set(const std::string &str, bool value) {
    glUniform1i(glGetUniformLocation(this->id, str.c_str()), value);
}

void ekg::gpu::program::set(const std::string &str, GLint value) {
    glUniform1i(glGetUniformLocation(this->id, str.c_str()), value);
}

void ekg::gpu::program::set(const std::string &str, GLuint value) {
    glUniform1ui(glGetUniformLocation(this->id, str.c_str()), value);
}

void ekg::gpu::program::set(const std::string &str, GLfloat value) {
    glUniform1f(glGetUniformLocation(this->id, str.c_str()), value);
}

void ekg::gpu::program::set2(const std::string &str, GLfloat *value) {
    glUniform2fv(glGetUniformLocation(this->id, str.c_str()), GL_TRUE, value);
}

void ekg::gpu::program::set3(const std::string &str, GLfloat *value) {
    glUniform3fv(glGetUniformLocation(this->id, str.c_str()), GL_TRUE, value);
}

void ekg::gpu::program::set4(const std::string &str, GLfloat *value) {
    glUniform4fv(glGetUniformLocation(this->id, str.c_str()), GL_TRUE, value);
}

void ekg::gpu::program::setm2(const std::string &str, GLfloat *matrix) {
    glUniformMatrix2fv(glGetUniformLocation(this->id, str.c_str()), GL_TRUE, GL_FALSE, matrix);
}

void ekg::gpu::program::setm3(const std::string &str, GLfloat *matrix) {
    glUniformMatrix3fv(glGetUniformLocation(this->id, str.c_str()), GL_TRUE, GL_FALSE, matrix);
}

void ekg::gpu::program::setm4(const std::string &str, GLfloat *matrix) {
    glUniformMatrix4fv(glGetUniformLocation(this->id, str.c_str()), GL_TRUE, GL_FALSE, matrix);
}