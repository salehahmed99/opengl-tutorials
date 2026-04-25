#version 330

layout(location = 0) in vec3 position;   // from cpu

uniform mat4 model;

void main() {

    gl_Position =  model * vec4(position, 1.0);
}

// Normally the vertex shader transforms a vertex all the way from local space to clip space:
// Here it only does the first step — local space to world space:
// Why it stops at world space:
// Normally you'd also multiply by view and projection here. 
// But you can't — because you have 6 different view+projection matrices (one per cubemap face). 
// The vertex shader runs once per vertex and doesn't know which face it's going to. 
// That decision happens in the geometry shader. So the vertex shader just gets the vertex into world space and hands it off. 
// The geometry shader will handle the rest.