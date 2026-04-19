#version 330

layout(location = 0) in vec3 position;   // from cpu
layout(location = 1) in vec2 texCoords;  // from cpu

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out Varyings {
    vec2 tex_coords;
} vs_out;

void main() {

    gl_Position = projection * view * model * vec4(position, 1.0);

    vs_out.tex_coords = texCoords;
}