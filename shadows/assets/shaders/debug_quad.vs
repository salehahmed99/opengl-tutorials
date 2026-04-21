#version 330

layout(location = 0) in vec3 position;   // from cpu
layout(location = 1) in vec2 texCoords;  // from cpu

out Varyings {
    vec2 tex_coords;
} vs_out;

void main() {

    gl_Position = vec4(position, 1.0);

    vs_out.tex_coords = texCoords;
}