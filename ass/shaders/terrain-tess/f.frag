#version 450 core

uniform sampler2D heightmap;
in vec2 fragTexCoord; // passed from TES
out vec4 FragColor;
void main() {
    // Simple color based on height
    FragColor = vec4(vec3(pow(texture(heightmap, fragTexCoord).x, 2)), 1.0);
}