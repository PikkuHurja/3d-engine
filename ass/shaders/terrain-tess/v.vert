#version 450 core

layout(location = 0) in vec3 aVertex;
layout(location = 1) in vec2 aUV;

out vec2 texCoord;

void main() {
    texCoord = aUV;
    gl_Position = vec4(aVertex, 1.0);
}