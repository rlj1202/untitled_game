#version 300 es
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_tex_coord;

out vec4 vertexColor;
out vec2 tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 tex_transform;

void main() {
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
    vertexColor = vec4(0.5, 0.0, 0.0, 1.0);
    tex_coord = (tex_transform * vec4(a_tex_coord, 0, 1)).xy;
}