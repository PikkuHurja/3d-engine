#version 430 core

layout(location = 0) in vec3 aVertex;
layout(location = 1) in vec2 aUV;

layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

layout(location = 5) in vec4 aModelMatrixR0;
layout(location = 6) in vec4 aModelMatrixR1;
layout(location = 7) in vec4 aModelMatrixR2;
layout(location = 8) in vec4 aModelMatrixR3;

layout(std140, binding = 0) uniform ubCamera
{
    vec3 camera_position; // POSITION
    vec3 camera_forward; // DIRECTION_FORWARD_NORMALIZED
    vec3 camera_up; // DIRECTION_UP_NORMALIZED

    mat4 view; // VIEW
    mat4 inverse_view; // INVERSE_VIEW

    mat4 perspective; // PERSPECTIVE
    mat4 inverse_perspective; // INVERSE_PERSPECTIVE

    mat4 view_perspective; // VIEW_PERSPECTIVE
    mat4 inverse_view_perspective; // INVERSE_VIEW_PERSPECTIVE
};

layout(std430, binding = 1) buffer ssbInstanceMapping
{
    uint instance_mapping_size;   // size, if zero, then attributes are used and there should be two different vertex shad
    uint instance_mapping[];      // converts gl_InstanceID to an index, which may be used to retrive instanced data
};

layout(std430, binding = 2) buffer ssbModelMatrix
{
    mat4 model_inverse_array[]; // converts gl_InstanceID to an index, which may be used to retrive instanced data
};


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

    InstanceIndex = instance_mapping[gl_InstanceID];
    mat4 model_matrix = model_inverse_array[InstanceIndex*2];
    mat4 inverse_model_matrix = model_inverse_array[InstanceIndex*2+1];


        ////////////// POSITION //////////////

    mat4 MPV = model_matrix * view_perspective;
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