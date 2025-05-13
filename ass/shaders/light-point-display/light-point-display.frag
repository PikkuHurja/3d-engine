#version 440 core


in vec2     UV;
in vec3     color;

out vec4 output_color;

void main(){

    float dist_pow = dot(UV, UV);
    if(dist_pow > 1){
        discard;
        return;
    }
    output_color.xyz = color * (1-sqrt(dist_pow));
        //opaque, for now
    output_color.a = 1;
}

