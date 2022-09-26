#version 450 core

layout (location = 0) in vec2 VertexData;
layout (location = 1) in vec2 UVData;

uniform mat4 MatrixProjection;
uniform vec4 Rect;
uniform float Depth;

out vec2 TextureUV;
out vec4 ShapeRect;

void main() {
    vec2 ProcessedVertex = VertexData;
    bool FixedShape = Rect.z != 0.0f && Rect.w != 0.0f;

    if (FixedShape) {
        ProcessedVertex *= Rect.zw;
    }

    ProcessedVertex += Rect.xy;
    gl_Position = MatrixProjection * vec4(ProcessedVertex, Depth, 1.0f);

    TextureUV = UVData;
    ShapeRect = Rect;
}