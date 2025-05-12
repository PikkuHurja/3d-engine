#version 460 core



struct point_light_t{
    uint index;
    uint padding[3];
    vec4 position_radius;
    vec4 color_intensity;
};

layout(std430, binding = 0) buffer _M_GPULightData{
    point_light_t light_data[];
};

layout(std430, binding = 1) buffer _M_GPULightMatricies{
    mat4 light_matricies[];
};

layout(std430, binding = 2) buffer _M_GPUEnabledLights{
    uint light_count;
    uint light_index[];
};

/*
    part of the light rendering process
    the first step, of rendering the z buffer from opaque objects.
    instances, 6 * light_count * instance_count

*/
void main(){

}
