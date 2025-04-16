#version 430 core

layout(location = 0) in vec3 aVertex;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;


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


out flat uint   InstanceIndex;
out vec3        LocalPosition;
out vec3        WorldPosition;
out vec3        Normal;


out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

out mat3 TBN; // TBN Matrix for normal mapping


void main(){
        //basic passthoughs
    LocalPosition = aVertex;


        ////////////// INSTANCE MAPPING //////////////

        ////////////// POSITION //////////////

    mat4 MPV = model_matrix * perspective_view;
    vec4 wpos = MPV * vec4(aVertex, 1.f);
    WorldPosition = wpos.xyz;
    gl_Position = wpos;


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