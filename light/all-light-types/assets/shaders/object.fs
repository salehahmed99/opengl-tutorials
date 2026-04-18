#version 330

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight {
    vec3 direction; // global direction pointing from the light source to the objects in the scene (e.g., for sunlight, this would be the direction of the sun rays)

    // Light colors
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    // Attenuation parameters
    float constant; 
    float linear;
    float quadratic;

    // Light colors
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;    // direction spotlight is pointing towards (e.g., for a flashlight, this would be the direction the flashlight is aiming at)
    float innerConeCos;      // cosine of the inner cutoff angle (the angle within which the spotlight has full intensity)
    float outerConeCos; // cosine of the outer cutoff angle (the angle beyond which the spotlight has no intensity)
  
  // Attenuation parameters
    float constant;
    float linear;
    float quadratic;
  
  // Light colors
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define NR_POINT_LIGHTS 4

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform vec3 view_pos;  // position of the camera in world space (used for specular lighting)


out vec4 frag_color;

in Varyings {
    vec3 normal;    // normal vector in world space
    vec3 frag_pos;   // fragment position in world space
    vec2 tex_coords; // texture coordinates
} fs_in;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


void main() {

    vec3 norm = normalize(fs_in.normal);

    vec3 viewDir = normalize(view_pos - fs_in.frag_pos);

    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and a flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================

     // phase 1: directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    // phase 2: point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, fs_in.frag_pos, viewDir);   
        
    // phase 3: spot light
    result += CalcSpotLight(spotLight, norm, fs_in.frag_pos, viewDir);     

    frag_color = vec4(result, 1.0);

}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction); // negate the light direction so that it points from the fragment to the light source
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, fs_in.tex_coords));
    vec3 diffuse = diff * light.diffuse * vec3(texture(material.diffuse, fs_in.tex_coords));
    vec3 specular = spec * light.specular * vec3(texture(material.specular, fs_in.tex_coords));

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, fs_in.tex_coords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, fs_in.tex_coords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, fs_in.tex_coords));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}


// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  

    // spotlight intensity
    float cosTheta = dot(lightDir, normalize(-light.direction));  // cosine of the angle between light direction and spotlight direction (as angle between them decreases (i.e., as the fragment gets closer to the center of the spotlight), cosTheta increases)
    float epsilon = light.innerConeCos - light.outerConeCos; // cosine difference between the inner (ϕ) and the outer cone (γ)

    float intensity = clamp((cosTheta - light.outerConeCos) / epsilon, 0.0, 1.0); // as theta increases towards the outer cone angle, its cosine decreases, resulting in a lower intensity.

    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, fs_in.tex_coords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, fs_in.tex_coords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, fs_in.tex_coords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}