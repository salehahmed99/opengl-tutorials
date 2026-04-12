#version 330

uniform vec2 u_resolution;
out vec4 frag_color;
void main(){
    vec2 st = gl_FragCoord.xy / u_resolution;
    frag_color = vec4(st.x,0,0.0,1.0);
}