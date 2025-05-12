#version 460 core
uniform vec4 albedo = vec4(1,1,1,1);

out vec4 output_color;
void main(){
    output_color = albedo;
}
