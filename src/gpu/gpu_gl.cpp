#include "ekg/gpu/gpu_gl.hpp"
#include "ekg/cpu/cpu_info.hpp"
#include "ekg/util/util.hpp"

void ekg::gpu::init_opengl_context() {
    switch (EKG_CPU_PLATFORM) {
        case ekg::platform::os_android: {
            // Ignore.
            break;
        }

        default: {
            glewExperimental = GL_TRUE;

            if (glewInit()) {
                ekg::log("Failed to initialise GLEW OpenGL context!");
            } else {
                ekg::log("GLEW initialised");
            }

            break;
        }
    }
}
