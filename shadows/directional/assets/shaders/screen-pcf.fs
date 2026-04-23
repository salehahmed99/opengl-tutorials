#version 330

in Varyings {
    vec2 tex_coords;
    vec3 frag_pos;
    vec3 normal;
    vec4 frag_pos_light_space;
} fs_in;

uniform vec3 light_pos;
uniform vec3 view_pos;

uniform sampler2D depth_map;
uniform sampler2D diffuse_texture;

out vec4 frag_color;

float calcShadow(vec3 lightDir, vec3 normal) {

    // perform perspective division
    // actually this is not really needed since we are using orthographic projection for the light space,
    // so the w component is always 1, but we do it for the sake of completeness
    vec3 projCoords = fs_in.frag_pos_light_space.xyz / fs_in.frag_pos_light_space.w;

    // transform the projCoords from ndc space [-1,1] to texCoords space [0,1]
    // now this value represents the location of the current fragment in the depth map

    projCoords = projCoords * 0.5 + 0.5;

    float shadow = 0.0;
    
    float currentDepth = projCoords.z;

    float bias = max(0.05 * (1 - dot(lightDir, normal)), 0.005);
    
    // textureSize(depth_map, 0) returns the resolution of the shadow map — e.g. vec2(1024, 1024).
    // Dividing 1.0 by that gives you the size of one texel in UV space:
    // 1.0 / 1024 = 0.000977...
    // So texelSize is roughly vec2(0.001, 0.001). This is the step size you need to move exactly one texel in any direction.
    vec2 texelSize = 1.0 / textureSize(depth_map, 0);
    
    for (int i = -1; i <= 1; ++i){
        for (int j = -1; j <=1; ++j){
            float pcfDepth = texture(depth_map, projCoords.xy + vec2(i,j) * texelSize).r;
            shadow += currentDepth > pcfDepth + bias ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;


    if(projCoords.z > 1.0)
        shadow = 0.0;
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
    float shadow = calcShadow(lightDir, normal);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    frag_color = vec4(lighting, 1.0);
}
