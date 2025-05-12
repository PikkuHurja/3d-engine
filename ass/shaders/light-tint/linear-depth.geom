
#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowmap_matricies[6];

out vec4 frag_pos; // FragPos from GS (output per emitvertex)

void main()
{
    /*
        kind of like multi viewport rendering*/
    for(int face = 0; face < 6; ++face){
        gl_Layer = face; // built-in variable that specifies to which face we render.

        for(int i = 0; i < 3; ++i) {// for each triangle vertex
        
            frag_pos = gl_in[i].gl_Position;
            gl_Position = shadowmap_matricies[face] * frag_pos;
            EmitVertex();
        
        }
        EndPrimitive();
    }
} 