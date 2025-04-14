#version 430 core

layout(location = 0) in vec3 aVertex;
layout(location = 1) in vec2 aUV;

layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;



//out flat uint   InstanceIndex;
out vec3        LocalPosition;
out vec3        WorldPosition;
out vec3        Normal;

void main(){
    LocalPosition   = aVertex;
    WorldPosition   = aVertex;
    Normal          = aNormal;
    
    gl_Position = vec4(aVertex, 1.);
}