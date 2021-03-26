#version 300 es
precision highp float;

out vec4 FragColor;
  
in vec4 vertexColor;
in vec2 tex_coord;

uniform sampler2D tex;

void main() {
    // FragColor = texture(tex, tex_coord);
    FragColor = texture(tex, tex_coord);
    // FragColor = vec4(1.0, 0.0, 1.0, 1.0);
}
