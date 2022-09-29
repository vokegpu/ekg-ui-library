#include "ekg/gpu/gpu_allocator.hpp"
#include "ekg/ekg.hpp"

ekg::gpu::program ekg::gpu::allocator::program {};
float ekg::gpu::allocator::orthographicm4[16] {};

void ekg::gpu::allocator::invoke() {
    this->iterate_ticked_count = 0;
    this->begin_stride_count = 0;
    this->end_stride_count = 0;

    // Set stride to 0 vertex position.
    this->clear_current_data();
    this->bind_current_data().begin_stride = this->begin_stride_count;
    this->bind_scissor(-1, -1, -1, -1);
}

void ekg::gpu::allocator::bind_texture(GLuint &texture) {
    bool should_alloc_new_texture {};
    GLuint texture_slot {};

    for (std::size_t it = 0; it < this->loaded_texture_list.size(); it++) {
        GLuint &textures = this->loaded_texture_list.at(it);
        should_alloc_new_texture = textures != texture;

        if (!should_alloc_new_texture) {
            texture_slot = (uint32_t) it;
            break;
        }
    }

    if (should_alloc_new_texture) {
        this->loaded_texture_list.push_back(texture);
        texture_slot = (GLuint) this->loaded_texture_list.size() - 1;
    }

    ekg::gpu::data &data = this->bind_current_data();

    data.texture = texture;
    data.texture_slot = texture_slot;
}

void ekg::gpu::allocator::dispatch() {
    auto &data = this->bind_current_data();

    data.scissored_area[0] = this->current_scissor_bind[0];
    data.scissored_area[1] = this->current_scissor_bind[1];
    data.scissored_area[2] = this->current_scissor_bind[2];
    data.scissored_area[3] = this->current_scissor_bind[3];
    data.end_stride = this->end_stride_count;

    this->begin_stride_count += this->end_stride_count;
    this->end_stride_count = 0;

    this->iterate_ticked_count++;
    this->clear_current_data();;
}

void ekg::gpu::allocator::revoke() {
    glBindVertexArray(this->buffer_list);
    glBindBuffer(GL_ARRAY_BUFFER, this->buffer_vertex);
    glEnableVertexAttribArray(0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->loaded_vertex_list.size(), &this->loaded_vertex_list[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, this->buffer_uv);
    glEnableVertexAttribArray(1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->loaded_uv_list.size(), &this->loaded_uv_list[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glBindVertexArray(0);

    this->loaded_texture_list.clear();
    this->loaded_vertex_list.clear();
    this->loaded_uv_list.clear();
}

void ekg::gpu::allocator::draw() {
    GLfloat viewport[4];
    glGetFloatv(GL_VIEWPORT, viewport);

    ekg::orthographic2d(ekg::gpu::allocator::orthographicm4, 0, viewport[2], viewport[3], 0);
    ekg::gpu::invoke(ekg::gpu::allocator::program);

    ekg::gpu::allocator::program.setm4("MatrixProjection", ekg::gpu::allocator::orthographicm4);
    ekg::gpu::allocator::program.set("ViewportHeight", viewport[3]);

    glBindVertexArray(this->buffer_list);

    bool scissor_enabled {};
    bool active_texture {};
    bool texture_enabled {};

    float depth_testing {this->depth_testing_preset};

    for (uint32_t data_iterations = 0; data_iterations < this->iterate_ticked_count; data_iterations++) {
        auto &data = this->cpu_allocated_data[data_iterations];
        active_texture = data.texture != 0;

        if (active_texture) {
            glActiveTexture(GL_TEXTURE0 + data.texture_slot);
            glBindTexture(GL_TEXTURE_2D, data.texture);

            ekg::gpu::allocator::program.set("ActiveTextureSlot", data.texture_slot);
            texture_enabled = true;
        }

        if (texture_enabled && !active_texture) {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        ekg::gpu::allocator::program.set("ActiveTexture", active_texture);
        ekg::gpu::allocator::program.set4("Color", data.colored_area);
        ekg::gpu::allocator::program.set4("Rect", data.rect_area);
        ekg::gpu::allocator::program.set("Depth", depth_testing);
        ekg::gpu::allocator::program.set("LineThickness", data.outline);

        switch (data.scissored_area[0]) {
            case -1: {
                if (scissor_enabled) {
                    glDisable(GL_SCISSOR_TEST);
                    scissor_enabled = false;
                }

                glDrawArrays(GL_TRIANGLES, data.begin_stride, data.end_stride);
                break;
            }

            default: {
                if (!scissor_enabled) {
                    glEnable(GL_SCISSOR_TEST);
                    glScissor(data.scissored_area[0], static_cast<GLint>(viewport[3]) - (data.scissored_area[1] + data.scissored_area[3]), data.scissored_area[2], data.scissored_area[3]);
                    scissor_enabled = true;
                }

                glDrawArrays(GL_TRIANGLES, data.begin_stride, data.end_stride);
                break;
            }
        }

        depth_testing += 0.001f;
    }

    if (scissor_enabled || texture_enabled) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_SCISSOR_TEST);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void ekg::gpu::allocator::init() {
    glGenVertexArrays(1, &this->buffer_list);
    glGenBuffers(1, &this->buffer_vertex);
    glGenBuffers(1, &this->buffer_uv);

    const std::string vsh_src {
ekg::gl_version + "\n"
"layout (location = 0) in vec2 VertexData;\n"
"layout (location = 1) in vec2 UVData;\n"

"uniform mat4 MatrixProjection;\n"
"uniform vec4 Rect;\n"
"uniform float Depth;\n"
""
"out vec2 TextureUV;\n"
"out vec4 ShapeRect;\n"
""
"void main() {"
"    vec2 ProcessedVertex = VertexData;\n"
"    bool FixedShape = Rect.z != 0 || Rect.w != 0;\n"
""
"    if (FixedShape) {"
"        ProcessedVertex *= Rect.zw;"
"    }\n"
""
"    ProcessedVertex += Rect.xy;\n"
"    gl_Position = MatrixProjection * vec4(ProcessedVertex, Depth, 1.0f);\n"
""
"    TextureUV = UVData;\n"
"    ShapeRect = Rect;\n"
"}"};

    const std::string fsh_src {
ekg::gl_version + "\n"
""
"layout (location = 0) out vec4 OutColor;\n"
""
"in vec2 TextureUV;\n"
"in vec4 ShapeRect;\n"
""
"uniform int LineThickness;\n"
"uniform float ViewportHeight;\n"
"uniform bool ActiveTexture;\n"
"uniform sampler2D ActiveTextureSlot;\n"
"uniform vec4 Color;\n"
""
"void main() {"
"    OutColor = Color;\n"
""
"    if (LineThickness != 0) {"
"        vec2 FragPos = vec2(gl_FragCoord.x, ViewportHeight - gl_FragCoord.y);\n"
"        vec4 OutlineRect = vec4(ShapeRect.x + LineThickness, ShapeRect.y + LineThickness, ShapeRect.z - (LineThickness * 2), ShapeRect.w - (LineThickness * 2));\n"
""
"        bool Collide = FragPos.x > OutlineRect.x && FragPos.x < OutlineRect.x + OutlineRect.z && FragPos.y > OutlineRect.y && FragPos.y < OutlineRect.y + OutlineRect.w;\n"
"        "
"        if (Collide) {"
"            discard;"
"        }"
"    }\n"
""
"    if (ActiveTexture) {"
"        OutColor = texture(ActiveTextureSlot, TextureUV);\n"
"        OutColor = vec4(OutColor.xyz - ((1.0f - Color.xyz) - 1.0f), OutColor.w - (1.0f - Color.w));"
"    }\n"
"}"};

    gpu::create_basic_program(ekg::gpu::allocator::program, vsh_src.c_str(), fsh_src.c_str());
}

void ekg::gpu::allocator::clear_current_data() {
    ekg::gpu::data &data = this->bind_current_data();

    data.outline = 0;
    data.factor = 0;
    data.texture = 0;
    data.texture_slot = 0;
}

ekg::gpu::data &ekg::gpu::allocator::bind_current_data() {
    return this->cpu_allocated_data[this->iterate_ticked_count];
}

void ekg::gpu::allocator::quit() {
    glDeleteTextures((GLint) this->loaded_texture_list.size(), &this->loaded_texture_list[0]);
    glDeleteBuffers(1, &this->buffer_list);
    glDeleteBuffers(1, &this->buffer_uv);
    glDeleteVertexArrays(1, &this->buffer_list);
}

void ekg::gpu::allocator::bind_scissor(int32_t x, int32_t y, int32_t w, int32_t h) {
    this->current_scissor_bind[0] = x;
    this->current_scissor_bind[1] = y;
    this->current_scissor_bind[2] = w;
    this->current_scissor_bind[3] = h;
}

void ekg::gpu::allocator::vertex2f(float x, float y) {
    this->loaded_vertex_list.push_back(x);
    this->loaded_vertex_list.push_back(y);
    this->end_stride_count++;
}

void ekg::gpu::allocator::coord2f(float x, float y) {
    this->loaded_uv_list.push_back(x);
    this->loaded_uv_list.push_back(y);
}