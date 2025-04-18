#version 430 core

in flat uint   InstanceIndex;

in vec2        UV;

in vec3        LocalPosition;
in vec3        WorldPosition;
in vec3        Normal;

in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

out vec4 output_color;

void main(){

    float fl = floor(LocalPosition.y);
    float fr = LocalPosition.y-fl;
    int f = int(abs(fl))%3;
    output_color = vec4(vec3(fr), 1);
    //output_color = vec4(vec3(fr), 1);
}