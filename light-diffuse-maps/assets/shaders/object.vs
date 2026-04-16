#version 330

layout (location = 0) in vec3 position;   // from cpu
layout (location = 1) in vec3 normal;     // from cpu

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out Varyings {
    vec3 normal;    // normal vector in world space
    vec3 frag_pos;  // fragment position in world space
} vs_out;


void main(){
    gl_Position = projection * view * model * vec4(position, 1.0);
    
    vs_out.normal = mat3(transpose(inverse(model))) * normal;
    vs_out.frag_pos = vec3(model * vec4(position, 1.0));
}