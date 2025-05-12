#version 330 core
layout (location = 0) in vec3 aVertex;

out vec3 UV3D;

    //remember to remove transforms etc using mat4(mat3(matrix));
uniform mat4 projection_view;

void main()
{
    UV3D = aVertex;
    vec4 pos = projection_view * vec4(aVertex, 1.0);
    gl_Position = pos.xyww;
}  