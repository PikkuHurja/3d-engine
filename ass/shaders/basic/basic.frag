#version 430 core

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



uniform samplerCube cube_map;
uniform vec3 cube_map_position = vec3(233.559, 32.0134, 267.112);
uniform float near_z  = 0.1;
uniform float far_z   = 500;

float linearize_depth(float d)
{
    return near_z * far_z / (far_z + d * (near_z - far_z));
}



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
    output_color.a = 1;

    vec3 frag_to_light = WorldPosition - cube_map_position;
    float current_depth = length(frag_to_light); // actual distance from light to fragment

    if(current_depth >= far_z){
        output_color.xyz = vec3(0);
        return;
    }

    // Sample depth stored in cubemap in that direction
    float sampled_depth = texture(cube_map, frag_to_light).r*(far_z-near_z) + near_z;

    // Apply bias to reduce shadow acne
    float bias = 4;
    float light = clamp(1 - ((current_depth-bias) - sampled_depth), 0.f, 1.f);
    /*sampled_depth/far_z, current_depth/far_z, */


	vec3 light_to_frag = cube_map_position - WorldPosition;
    vec3 light_direction = normalize(light_to_frag);
    float d = dot(light_direction, -Normal);
    float diffuse_intensity = clamp(d, 0.f, 1.f);

    output_color.xyz = vec3( (diffuse_intensity) * light * clamp((far_z - current_depth) / far_z, 0.f, 1.f));
}