#version 430 core

#extension GL_KHR_blend_equation_advanced : enable

/*
    remember to disable depth writing :3
*/

in vec2        UV;

uniform vec4 albedo = vec4(1,0,1,1);


out vec4 output_color;
void main(){
    vec4 o = albedo;

    output_color.xyz =  o.xyz;
    output_color.a = 1;
}
