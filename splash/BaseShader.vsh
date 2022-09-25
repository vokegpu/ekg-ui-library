#version 450 core

layout (location = 0) in vec2 VertexData;
layout (location = 1) in vec2 UVData;

uniform mat4 MatrixProjection;
uniform vec4 Rect;
uniform float Depth;

void main() {
    vec2 ProcessedVertex = (VertexData + Rect.xy) + rect.zw;
    gl_Position = MatrixProjection * vec4(ProcessedVertex, Depth, 1.0f);
}