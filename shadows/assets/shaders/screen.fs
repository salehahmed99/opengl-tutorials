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


float calcShadow(vec3 lightDir, vec3 normal){

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


    // Here is why we need the bias.
    // The shadow map is a flat 2D grid of depth values. Each texel covers a rectangular patch of the scene. 
    // Now imagine the light is shining straight down from above onto a flat floor:
    // Each texel covers a flat horizontal patch. The floor is parallel to the texel grid. 
    // Every fragment on the floor maps cleanly to a depth that matches its texel. No problem.

    // Now angle the light: Now the light is hitting the floor diagonally. 
    // The shadow map texels are still a flat grid, but the depth values they record now represent a slanted slice through space. 
    // The single depth value recorded for that texel corresponds to the front edge of the floor patch it covers. 
    // But the back edge of that same floor patch is physically farther from the light — yet it has to use the same recorded depth value.
    // So when a fragment at the back edge of the texel asks "am I in shadow?":
    
    // closestDepth = 0.50  (recorded at front edge of texel)
    // currentDepth = 0.54  (actual depth of back edge fragment)
    // 0.54 > 0.50 → flagged as shadow ❌
    // But it's just the floor! It should be lit! The steeper the light angle, the bigger this depth difference becomes across a single texel, and the worse the acne gets.
    // This is why wee need a bias, and it's not just a constant value. 
    // When the light is nearly straight down (perpendicular to the floor), the depth difference across one texel is tiny — maybe 0.001. A small fixed bias of 0.005 easily covers it.
    // When the light hits at a steep angle (nearly parallel to the floor), the depth difference across one texel could be 0.04 or more. A fixed bias of 0.005 doesn't cover it — you still get acne.
    
    float bias = max(0.05 * (1 -  dot(lightDir, normal)), 0.005);

    // The problem with the bias (Peter panning)
    // We're essentially telling every fragment: "pretend you're closer to the light than you actually are."
    // This is fine for small values — it just absorbs the numerical error. 
    // But if the bias is too large, fragments that are genuinely in shadow start thinking they're lit, specifically the fragments right next to the base of an object.
    // That causes the shadow near the base of the object to disappear making the object look detached from its shadow.
    float shadow = currentDepth  > closestDepth + bias  ? 1.0 : 0.0;

    // A little trick that solves this is the front face culling fix.
    // When rendering the depth map, we can cull the front faces instead of the back faces.
    // This way, the depth map records the depth of the back faces of the objects instead
    // The back face depth is naturally a bit deeper than the front face. 
    // So when we compare in Pass 2, the stored closestDepth is already slightly larger — we effectively get bias "for free" without artificially shifting anything.
    // This only works for closed objects though, since if the back face is missing (in case of a plane for exmaple), 
    // there will be nothing left since the floor has only one face, so no depth value will be recorded for the floor in the depth map (default is 1.0).
    // therefore when we compare to calculate shadow, the currentDepth will always be smaller the closestDepth and the floor will be always lit.
    
    

    // Why GL_CLAMP_TO_BORDER Doesn't Help Here
    // GL_CLAMP_TO_BORDER only handles out-of-range xy coordinates (the texture UV lookup). 
    // Any UV coordinate outside [0,1] will return the border color which was set to 1.0 (or technically the depth value since shadow maps store depth not color)
    // The problem here is that due to the light frustum having near and far planes, any fragment that is outside the light's view will have projCoords.z > 1.0, 
    // which means that it will always be greater than the closestDepth meaning that it will always be in shadow.
    // that's why simply force the shadow value to 0.0 whenever the projected vector's z coordinate is larger than 1.0
    if (projCoords.z > 1.0)
        shadow = 0.0;
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
    float shadow = calcShadow(lightDir, normal);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    frag_color = vec4(lighting, 1.0);
}

