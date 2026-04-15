#version 330

in vec2 vertex_texCoord; // from vertex shader

out vec4 frag_color;

uniform sampler2D texture1; // reads from texture unit 0
uniform sampler2D texture2; // reads from texture unit 1

// The sampler2D in GLSL just says "which texture unit to read from."
void main() {
    frag_color = mix(texture(texture1, vertex_texCoord), texture(texture2, vertex_texCoord), 0.2);
    // The final output color is now the combination of two texture lookups. 
    // GLSL's built-in mix function takes two values as input and linearly interpolates between them based on its third argument.
    // If the third value is 0.0 it returns the first input; if it's 1.0 it returns the second input value. 
    // A value of 0.2 will return 80% of the first input color and 20% of the second input color, resulting in a mixture of both our textures.

}