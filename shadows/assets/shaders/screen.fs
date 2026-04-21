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


float calcShadow(){

    // perform perspective division
    // actually this is not really needed since we are using orthographic projection for the light space,
    // so the w component is always 1, but we do it for the sake of completeness
    vec3 projCoords = fs_in.frag_pos_light_space.xyz / fs_in.frag_pos_light_space.w;

    // transform the projCoords from ndc space [-1,1] to texCoords space [0,1]
    // now this value represents the location of the current fragment in the depth map
     
    projCoords = projCoords * 0.5 + 0.5;

    // in the debug_quad.fs, we did a similar thing but the difference is that we passed the texture coordinates of the whole screen
    // since we were just drawing a quad. so the tex coords where [0,0], [0,1], [1,0], [1,1]
    // if you took a look at the the quadVertices in main, these were the texure coordinates we passed.
    // after interpolation, the tex coords represented the location of each fragment in the screen space. 
    // so when we sampled the depth map with those tex coords, we got the depth map value at that screen-space UV location, which corresponds to the depth of whatever the light saw at that position

    // Here, we have no useful pre-built UVs to sample the shadow map with. we need to compute where this fragment lives in the light's view,
    // which is what projCoords gives us. The inputs we're working from are world-space positions (frag_pos), but projCoords itself is not world space — 
    // it's the result of transforming through light space all the way to UV space.

    
    // now this texture function gets the color of the fragmentat that location which is technically not a color 
    //but rather the depth value of that fragment (i.e. the closest depth since the depth map stores
    // the closest depth value for each texel direction from the light's perspective).
    float closestDepth = texture(depth_map, projCoords.xy).r;  

    float currentDepth = projCoords.z;

    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
    return shadow;
}
void main()
{           
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

