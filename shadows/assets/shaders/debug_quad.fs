#version 330

out vec4 frag_color;

in Varyings {
    vec2 tex_coords;
} vs_out;

uniform sampler2D depth_map;

void main() {
    float depthValue = texture(depth_map, vs_out.tex_coords).r;
    frag_color = vec4(vec3(depthValue), 1.0);
}




// ### 1. Single-Channel Texture Sampling
// When you created your depth map in C++, you defined its format as `GL_DEPTH_COMPONENT`. 
// This means the texture only stores one single float value per pixel (the depth). 

// However, the GLSL `texture()` function *always* returns a `vec4`. When you ask OpenGL to sample a single-channel texture, it puts that single value into the red channel and fills the rest with default values:
// * **Red (`.r`):** The actual depth value
// * **Green (`.g`):** `0.0`
// * **Blue (`.b`):** `0.0`
// * **Alpha (`.a`):** `1.0`

// ### 2. The Result of Your Proposed Code
// If you use `frag_color = texture(depth_map, vs_out.tex_coords);`, you are passing that raw `vec4` directly to the screen. 

// Because the green and blue channels are hardcoded to `0.0` by OpenGL, your debug quad will render as a **black-to-red gradient**. 

// ### 3. The Result of the Original Code
// The original code explicitly grabs the depth value (the `.r` component) and applies it to all three RGB color channels simultaneously:
// ```glsl
// float depthValue = texture(depth_map, vs_out.tex_coords).r;
// // vec3(depthValue) is shorthand for vec3(depthValue, depthValue, depthValue)
// frag_color = vec4(vec3(depthValue), 1.0); 
// ```
// By setting red, green, and blue to the exact same value, the shader outputs a **grayscale (black-to-white) gradient**.

