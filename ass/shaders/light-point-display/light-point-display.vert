#version 440 core

layout(location = 0) in vec4 aPositionRadius;
layout(location = 1) in vec4 aColorIntensity;

out vec4 color_radius;

void main(){
    color_radius = vec4(aColorIntensity.xyz, aPositionRadius.w);
    gl_Position = vec4(aPositionRadius.xyz, 1.0);
}