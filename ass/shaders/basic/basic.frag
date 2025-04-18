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

    float fl = floor(LocalPosition.y);
    float fr = LocalPosition.y-fl;
    int f = int(abs(fl))%3;


    float x = dot(Normal, vec3(0,1,0));


    float height = LocalPosition.y/256;
    /*
    if(height <= 0.f)
        output_color = vec4(vec3(0, 0, 1+height*2), 1);
    else if(height <= 0.2f)
        output_color = mix(
            vec4(0.60, 0.53, 0.26, 1),
            vec4(0.35, 0.87, 0.26, 1),
            height*5
        );
    else if(height <= 1.f){
        output_color = mix(
            vec4(0.35, 0.87, 0.26, 1),
            vec4(1),
            (height-0.2)*1.25
        );
    }else{
        output_color = vec4(1,0,1,1);
    }
    */
    output_color.xyz=vec3(abs(pow(x, 6)));
    output_color.a = 1;
    //output_color = vec4(vec3(fr), 1);
}