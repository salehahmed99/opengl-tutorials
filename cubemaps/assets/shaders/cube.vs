#version 330

layout(location = 0) in vec3 position;   // from cpu
layout(location = 1) in vec2 tex_coords; // from cpu

out Varyings {
    vec2 tex_coords;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
void main() {

    gl_Position = projection * view * model * vec4(position, 1.0);

    vs_out.tex_coords = tex_coords;

}