#version 300 es
precision highp float;

out vec4 FragColor;

in vec2 tex_coord;
in vec4 color;
in vec3 normal;

uniform sampler2D tex;

void main() {
    FragColor = texture(tex, tex_coord);
}
