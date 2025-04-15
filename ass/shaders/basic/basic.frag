#version 430 core

in flat uint   InstanceIndex;

in vec3        LocalPosition;
in vec3        WorldPosition;
in vec3        Normal;

in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

out vec4 output_color;

void main(){
    output_color = vec4(LocalPosition, 1);
}