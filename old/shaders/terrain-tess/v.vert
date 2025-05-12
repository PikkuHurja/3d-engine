#version 460 core

layout(location = 0) in vec3 aVertex; // 2D grid coords (x, z)

void main() {
    // Send to TCS as a 3D position with y = 0 (flat plane)
    gl_Position = vec4(aVertex, 1.0);
}
