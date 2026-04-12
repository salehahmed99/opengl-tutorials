#version 330

in vec4 vertex_color;   // from vertex shader
in vec2 vertex_texCoord; // from vertex shader

out vec4 frag_color;

uniform sampler2D our_texture;
void main(){
    frag_color = texture(our_texture, vertex_texCoord);  

}