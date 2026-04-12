#version 330

in vec4 vertex_color;   // from vertex shader

out vec4 frag_color;

void main(){
    frag_color = vertex_color;  

}