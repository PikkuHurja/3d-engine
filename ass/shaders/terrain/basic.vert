#version 430 core

layout(location = 0) in vec3 aVertex;
layout(location = 1) in vec2 aUV;

layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;


uniform sampler2D height_map;


layout(std140, binding = 0) uniform ubCamera
{
    vec3 camera_position; // POSITION
    vec3 camera_forward; // DIRECTION_FORWARD_NORMALIZED
    vec3 camera_up; // DIRECTION_UP_NORMALIZED
    vec3 camera_right; // DIRECTION_RIGHT_NORMALIZED

    mat4 view; // VIEW
    mat4 inverse_view; // INVERSE_VIEW

    mat4 perspective; // PERSPECTIVE
    mat4 inverse_perspective; // INVERSE_PERSPECTIVE

    mat4 perspective_view; // VIEW_PERSPECTIVE
    mat4 inverse_perspective_view; // INVERSE_VIEW_PERSPECTIVE
};


uniform mat4 model_matrix = mat4(1.f);
uniform vec2 size = vec2(32);
uniform vec2 height_minmax = vec2(-4, 4);

out flat uint   InstanceIndex;
out vec3        LocalPosition;
out vec3        WorldPosition;
out vec3        Normal;


out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

out mat3 TBN; // TBN Matrix for normal mapping


void main(){
        ////////////// POSITION //////////////
    float norm_offset = texture(height_map, (aVertex.xz+0.5)/size).x;
    float offset = (norm_offset*(height_minmax.y-height_minmax.x))+ height_minmax.x;
    LocalPosition = aVertex+vec3(0,offset, 0);

    mat4 MPV = model_matrix * perspective_view;
    vec4 world_position = MPV * vec4(LocalPosition, 1.f);
    WorldPosition   = world_position.xyz;
    gl_Position     = world_position;


        ////////////// NORMAL MAPPING //////////////

    vec3 T = normalize(vec3(model_matrix * vec4(aTangent, 0.0)));
    Normal = normalize(vec3(model_matrix * vec4(aNormal, 0.0)));
    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, Normal) * Normal);
    // then retrieve perpendicular vector B with the cross product of T and N
    vec3 B = cross(Normal, T);
    mat3 TBN = mat3(T, B, Normal);  

    // TangentLightPos = ;
    TangentViewPos =  TBN * camera_position;
    TangentFragPos =  TBN * WorldPosition;
    
}