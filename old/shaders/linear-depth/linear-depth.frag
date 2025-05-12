
#version 330 core
in vec4 frag_pos;

uniform vec3  light_pos;
uniform float far_z     = 500;
uniform float near_z    = 0.1;

void main()
{
    // get distance between fragment and light source
    float light_distance = length(frag_pos.xyz - light_pos);
    // map to [0;1] range by dividing by far_plane
    light_distance = (light_distance-near_z) / (far_z-near_z);
    // write this as modified depth
    gl_FragDepth = light_distance;
}  