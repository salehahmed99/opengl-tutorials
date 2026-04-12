#version 330

layout (location = 0) in vec3 position;   // from cpu
layout (location = 1) in vec4 color;      // from cpu

out vec4 vertex_color;

uniform mat4 MVP;
void main(){

    gl_Position =  MVP * vec4(position, 1.0);
    vertex_color = color;
}