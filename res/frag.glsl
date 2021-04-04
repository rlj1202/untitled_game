#version 300 es
precision highp float;

out vec4 FragColor;
  
in vec4 vertexColor;
in vec2 tex_coord;

uniform sampler2D tex;

void main() {
    FragColor = texture(tex, tex_coord);
}
