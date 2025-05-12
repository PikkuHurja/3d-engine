#version 460 core
#extension GL_KHR_blend_equation_advanced : require
/*
    remember to disable depth writing :3
*/

uniform vec4 albedo = vec4(1,0,1,1);


layout(blend_support_multiply) out;
layout(location = 0) out vec4 output_color;

void main(){
    vec4 o = albedo;

    output_color.xyz = o.xyz;
    output_color.a = 1;
}
