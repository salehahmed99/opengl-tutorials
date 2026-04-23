#version 330

layout(location = 0) in vec3 position;   // from cpu

out Varyings {
    vec3 tex_coords;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
void main() {

    vec4 pos = projection * view * vec4(position, 1.0);
    // we set the w component the same as z so that after perspective division, 
    //the z component (depth) will be 1.0 so that the skybox will always be
    // rendered at the farthest depth
    gl_Position = pos.xyww; 
    vs_out.tex_coords = position;
}