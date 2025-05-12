#version 430 core

in flat uint   InstanceIndex;

in vec3        LocalPosition;
in vec3        WorldPosition;
in vec3        Normal;

in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

out vec4 output_color;

layout(std140, binding = 0) uniform ubCamera{
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

void main(){
    float height = LocalPosition.y;

    vec3 color;
    if(height >= 0.2){
        color = mix(
            vec3(0, 0.5+height/2, 0), 
            vec3(log(height/16+1)), 
            clamp(height-1.2, 0, 1)
        );
    }else if(height >= 0.){
        color = mix(
            vec3(179./255., 169./255., 108./255.),
            vec3(0, 0.5+height/2, 0), 
            clamp(height*5, 0, 1)
        );
    }else{
        color = mix(
            vec3(0,0,1+height/8),
            vec3(179./255., 169./255., 108./255.),
            clamp(height+1, 0, 1)
        );
    }


    output_color = vec4(vec3(color), 1);
}