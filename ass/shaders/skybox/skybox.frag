#version 330 core

in vec3 UV3D;

uniform vec4        albedo = vec4(1);
uniform samplerCube skybox_cubemap;

out vec4 output_color;
void main()
{    
    output_color = texture(skybox_cubemap, UV3D) * albedo;
}
