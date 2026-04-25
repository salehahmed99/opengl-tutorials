#version 330

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadow_matrices[6];
out Varyings {
    vec4 frag_pos;
} gs_out;

void main() {
    for(int face = 0; face < 6; ++face) {
        // It tells OpenGL: "send the next primitive to cubemap face number face." 
        // Without this, everything would just go to face 0.
        gl_Layer = face;
        for(int i = 0; i < 3; ++i) {
            gs_out.frag_pos = gl_in[i].gl_Position;                 // world space position → sent to fragment shader
            gl_Position = shadow_matrices[face] * gs_out.frag_pos;   // transform to THIS face's clip space
            EmitVertex();                                           // output this vertex
        }
        EndPrimitive();                                             // finish this triangle
    }
}


// The geometry shader receives one complete triangle at a time — 3 vertices already in world space from the vertex shader. 
// They come in through gl_in[0], gl_in[1], gl_in[2].

// So one triangle goes in, six triangles come out — each transformed into a different face's coordinate space and directed to that face via gl_Layer.