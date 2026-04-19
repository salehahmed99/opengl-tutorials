#version 330

out vec4 frag_color;

in Varyings{
    vec2 tex_coords;
} vs_out;

uniform sampler2D tex;

void main() {
    frag_color = texture(tex, vs_out.tex_coords);
}