#version 330

layout(location = 0) in vec3 position;   // from cpu
layout(location = 1) in vec3 normal;     // from cpu
layout(location = 2) in vec2 texCoords;  // from cpu
out Varyings {
    vec2 tex_coords;
    vec3 frag_pos;
    vec3 normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool reverse_normals; // for drawing the inside of the cube with correct lighting
void main() {

    vs_out.tex_coords = texCoords; // pass tex coords to fragment shader
    vs_out.frag_pos = vec3(model * vec4(position, 1.0)); // calculate world space position of the fragment and pass to fragment shader
    if (reverse_normals){
        vs_out.normal = -mat3(transpose(inverse(model))) * normal; // calculate normal in world space and pass to fragment shader
    } else {
        vs_out.normal = mat3(transpose(inverse(model))) * normal; // calculate normal in world space and pass to fragment shader
    }

    gl_Position = projection * view * model * vec4(position, 1.0);
}