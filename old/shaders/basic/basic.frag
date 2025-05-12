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



    // radius could be removed, since it could be calculated using brightness
struct point_light_t{
    uint cubemap_index16_reserved16;    //2 bytes shadowmap index, rest is reserved
    vec4 position_radius;               //xyz position, w radius
    vec4 color_brightness;              //xyz color, w brightness/intensity
};

void point_light_t_get_cubemap_index(inout point_light_t pl, out uint index){
    index = pl.cubemap_index16_reserved16 & 0x0000FFFF;
}
void point_light_t_get_position(inout point_light_t pl, out vec3 position){
    position = pl.position_radius.xyz;
}
void point_light_t_get_radius(inout point_light_t pl, out float radius){
    radius = pl.position_radius.w;
}

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

/*
lets do multisampling
*/
const float step_factor = 1/1000;

int auto_num_samples(float distance_from_lightsrc){
    return 5;
}

const float sample_factor = 1/1000; //less close, more further
const float bias          = 4;


float to_distance(float shadowmap_sample){
    return shadowmap_sample*(far_z-near_z) + near_z;
}

float sample_cubemap_shadowmap(int number_of_samples, float distance_from_lightsrc, vec3 fragment_normal, vec3 fragment_position){
    float cumulative = 0;
    
    float step_size = distance_from_lightsrc * step_factor;
    float offset_angle = 3.14159 * 2.0 / float(number_of_samples);
    for(int i = 0; i < number_of_samples; i++){
        vec3 sample_position = fragment_position;
        float shadowmap_sample = texture(cube_map, sample_position).r;
        float sampled_depth = to_distance(shadowmap_sample);
        float light = clamp(1 - ((distance_from_lightsrc - bias) - sampled_depth), 0.f, 1.f);
        cumulative += light;
    }
    
    cumulative/=number_of_samples;
    return cumulative;
}


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
    float d = dot(light_direction, Normal);
    float diffuse_intensity = clamp(d, 0.f, 1.f);

    output_color.xyz = vec3( (diffuse_intensity) * light * clamp((far_z - current_depth) / far_z, 0.f, 1.f));
}