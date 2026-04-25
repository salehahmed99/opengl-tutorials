#version 330


in Varyings {
    vec4 frag_pos;
} fs_in;
uniform vec3 light_pos;
uniform float far_plane;

void main() {
    float lightDistance = length(fs_in.frag_pos.xyz - light_pos); // This is linear depth — the actual geometric distance, not the non-linear clip-space depth OpenGL normally uses.
    lightDistance = lightDistance / far_plane;                  // normalize to [0,1] range by dividing by far_plane
    
    // Manually writes this value as the fragment's depth. This overrides what OpenGL would normally write automatically. 
    // The cubemap now stores real linear distances instead of non-linear clip-space depths.
    gl_FragDepth = lightDistance;

    // Why Linear Depth?
    // In regular shadow mapping you let OpenGL write depth automatically — it was non-linear clip-space depth. 
    // That worked because both currentDepth and closestDepth were in the same non-linear space so the comparison was still valid.

    // Here you store linear distance instead because in Pass 2 you need to compare
    // against length(fragToLight) — which is a real world-space distance.
    // You need both values in the same space, and real distance is more natural to compute than reverse-engineering clip-space depth.
}