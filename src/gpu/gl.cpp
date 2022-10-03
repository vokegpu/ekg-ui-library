#include "ekg/gpu/gl.hpp"
#include "ekg/cpu/info.hpp"
#include "ekg/util/geometry.hpp"

void ekg::gpu::init_opengl_context() {
    switch (ekg::os) {
        case ekg::platform::os_android: {
            // Ignore.
            break;
        }

        default: {
            break;
        }
    }
}

bool ekg::gpu::load_basic_program(ekg::gpu::program &program, const std::string &vsh_path, const std::string &fsh_path) {
    std::string vsh_src {};
    std::string fsh_src {};

    return ekg::file_to_string(vsh_src, vsh_path) && ekg::file_to_string(fsh_src, fsh_path) && gpu::create_basic_program(program, vsh_src.c_str(), fsh_src.c_str());
}

bool ekg::gpu::create_basic_program(ekg::gpu::program &program, const char *vsh_src, const char *fsh_src) {
    GLuint vsh {};
    GLuint fsh {};

    bool flag {ekg::gpu::compile_shader(vsh, GL_VERTEX_SHADER, vsh_src) && ekg::gpu::compile_shader(fsh, GL_FRAGMENT_SHADER, fsh_src)};

    if (flag) {
        program.id = glCreateProgram();

        glAttachShader(program.id, vsh);
        glAttachShader(program.id, fsh);
        glLinkProgram(program.id);

        GLint status {GL_FALSE};
        glGetProgramiv(program.id, GL_LINK_STATUS, &status);

        if (!status) {
            char log[256];
            glGetProgramInfoLog(program.id, 256, nullptr, log);

            std::string ln {"\n"};
            ln += log;
            ekg::log(ln);

            flag = false;
        }

        glDeleteShader(vsh);
        glDeleteShader(fsh);
    }

    return flag;
}

bool ekg::gpu::compile_shader(GLuint &shader, GLuint shader_type, const char *src) {
    shader = glCreateShader(shader_type);

    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint status {GL_FALSE};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (!status) {
        char log[256];
        glGetShaderInfoLog(shader, 256, nullptr, log);

        std::string ln {"\n"};
        ln += log;
        ekg::log(ln);

        return false;
    }

    return true;
}

void ekg::gpu::invoke(ekg::gpu::program &program) {
    glUseProgram(program.id);
}

void ekg::gpu::revoke() {
    glUseProgram(0);
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