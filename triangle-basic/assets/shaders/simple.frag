#version 330

in vec4 vertex_color;
out vec4 frag_color;

// The flow is all follows:
// 1- Vertex shader runs for each vertex and outputs vertex_color (red/green/blue values).
// 2- OpenGL assembles the triangle and rasterizes it into fragments (pixel candidates)
// 3- During rasterization, OpenGL automatically interpolates each out value from the vertex shader across the triangle surface.
// 4- Fragment shader runs per fragment and receives that interpolated value as in vec4 vertex_color.
void main(){
    frag_color = vertex_color;  // rThat line just writes the already-interpolated color.
}