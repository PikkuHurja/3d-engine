#version 450 core

in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D heightmap;

void main() {
    // Simple color based on height
    float height = texture(heightmap, texCoord).r;
    FragColor = vec4(height, height, height, 1.0);
}