#version 330


uniform samplerCube sky_box;

in Varyings {
    vec3 tex_coords;
} fs_in;

out vec4 frag_color;

void main(){
    frag_color = texture(sky_box, fs_in.tex_coords);
}