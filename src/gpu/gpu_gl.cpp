#include "ekg/gpu/gpu_gl.hpp"
#include "ekg/cpu/cpu_info.hpp"
#include "ekg/util/util.hpp"

void ekg::gpu::init_opengl_context() {
    switch (ekg::os) {
        case ekg::platform::os_android: {
            // Ignore.
            break;
        }

        default: {
            glewExperimental = GL_TRUE;

            if (!glewInit()) {
                ekg::log("Failed to initialise GLEW OpenGL context!");
            } else {
                ekg::log("GLEW initialised");
            }

            break;
        }
    }
}

void ekg::gpu::program::set(const std::string &str, bool value) {
    glProgramUniform1i(this->id, glGetUniformLocation(this->id, str.c_str()), (int32_t) value);
}

void ekg::gpu::program::set(const std::string &str, int32_t value) {
    glProgramUniform1i(this->id, glGetUniformLocation(this->id, str.c_str()), (int32_t) value);
}

void ekg::gpu::program::set(const std::string &str, float value) {
    glProgramUniform1f(this->id, glGetUniformLocation(this->id, str.c_str()), value);
}

void ekg::gpu::program::set2(const std::string &str, float *value) {
    glProgramUniform2fv(this->id, glGetUniformLocation(this->id, str.c_str()), 0, value);
}

void ekg::gpu::program::set3(const std::string &str, float *value) {
    glProgramUniform3fv(this->id, glGetUniformLocation(this->id, str.c_str()), 0, value);
}

void ekg::gpu::program::set4(const std::string &str, float *value) {
    glProgramUniform4fv(this->id, glGetUniformLocation(this->id, str.c_str()), 0, value);
}

void ekg::gpu::program::setm2(const std::string &str, float *matrix) {
    glProgramUniformMatrix2fv(this->id, glGetUniformLocation(this->id, str.c_str()), 0, GL_FALSE, matrix);
}

void ekg::gpu::program::setm3(const std::string &str, float *matrix) {
    glProgramUniformMatrix3fv(this->id, glGetUniformLocation(this->id, str.c_str()), 0, GL_FALSE, matrix);
}

void ekg::gpu::program::setm4(const std::string &str, float *matrix) {
    glProgramUniformMatrix4fv(this->id, glGetUniformLocation(this->id, str.c_str()), 0, GL_FALSE, matrix);
}
