#version 330

out vec4 frag_color;

in Varyings {
    vec2 tex_coords;
} vs_out;

uniform sampler2D screen_texture;

void main() {
    vec4 fragment_color = texture(screen_texture, vs_out.tex_coords);
    float average = 0.2126 * fragment_color.r + 0.7152 * fragment_color.g + 0.0722 * fragment_color.b;
    frag_color = vec4(average, average, average, 1.0);
}