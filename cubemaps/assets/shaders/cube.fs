#version 330


uniform sampler2D cube_texture;

in Varyings {
    vec2 tex_coords;
} fs_in;

out vec4 frag_color;

void main(){
    frag_color = texture(cube_texture, fs_in.tex_coords);
}