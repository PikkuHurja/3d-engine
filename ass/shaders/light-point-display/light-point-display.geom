#version 440 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec4 color_radius[];

out vec2 UV;
out vec3 color;
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

void main() {
    vec3 center = gl_in[0].gl_Position.xyz;

    // Generate camera-facing quad
    float radius = color_radius[0].w;
    vec3 c_color = color_radius[0].xyz;
    vec3 right = vec3(view[0][0], view[1][0], view[2][0]) * radius;
    vec3 up    = vec3(view[0][1], view[1][1], view[2][1]) * radius;

    vec3 corners[4] = vec3[](
        center - right - up,
        center + right - up,
        center - right + up,
        center + right + up
    );

    vec2 uvs[4] = vec2[](
        vec2(-1.0, -1.0),
        vec2( 1.0, -1.0),
        vec2(-1.0,  1.0),
        vec2( 1.0,  1.0)
    );

    for (int i = 0; i < 4; ++i) {
        gl_Position = perspective_view * vec4(corners[i], 1.0);
        UV = uvs[i];
        color = c_color;
        EmitVertex();
    }
    EndPrimitive();
}
