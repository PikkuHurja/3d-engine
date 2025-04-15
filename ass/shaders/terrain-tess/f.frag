#version 450 core

uniform sampler2D heightmap;
in vec2 fragTexCoord; // passed from TES
out vec4 FragColor;

float linearize_depth(float d,float zNear,float zFar)
{
    return zNear * zFar / (zFar + d * (zNear - zFar));
}
void main() {
    // Simple color based on height
    FragColor = vec4(vec3(pow(texture(heightmap, fragTexCoord).x, 2)), 1.0);
}