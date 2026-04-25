#version 330

in Varyings {
    vec2 tex_coords;
    vec3 frag_pos;
    vec3 normal;
} fs_in;

uniform vec3 light_pos;
uniform vec3 view_pos;
uniform float far_plane;

uniform samplerCube depth_map;
uniform sampler2D diffuse_texture;

out vec4 frag_color;

float calcShadow() {
    vec3 fragToLight = fs_in.frag_pos - light_pos; // vector from light to fragment in world space

    float closestDepth = texture(depth_map, fragToLight).r;

    closestDepth *= far_plane; // rettransform back to original depth value [0, far_plane]

    float currentDepth = length(fragToLight);   // current depth is the distance from the light to the fragment in world space

    float bias = 0.05; // add some bias to prevent shadow acne
    float shadow = currentDepth > closestDepth + bias ? 1.0 : 0.0;
    return shadow;
  
}
void main() {
    vec3 color = texture(diffuse_texture, fs_in.tex_coords).rgb;
    vec3 normal = normalize(fs_in.normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * lightColor;

    // diffuse
    vec3 lightDir = normalize(light_pos - fs_in.frag_pos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    vec3 viewDir = normalize(view_pos - fs_in.frag_pos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 256.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = calcShadow();
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    frag_color = vec4(lighting, 1.0);
}
