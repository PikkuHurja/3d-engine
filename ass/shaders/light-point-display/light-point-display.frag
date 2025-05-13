#version 440 core
#extension GL_KHR_blend_equation_advanced : require


in vec2     UV;
in vec3     color;

layout(blend_support_multiply) out;
out vec4 output_color;

void main(){

    float dist_pow = dot(UV, UV);
    if(dist_pow > 1){
        discard;
        return;
    }
    float transparency = (1-sqrt(dist_pow));
    output_color.xyz = color * transparency;
        //opaque, for now
    output_color.a = transparency;
}

