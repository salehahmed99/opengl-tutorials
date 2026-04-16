#version 330

struct Material {
    vec3 ambient;      // This is the base color of your cube when it's in the shadows. Even if direct light isn't hitting an object in the real world, light bounces off walls and the environment to illuminate it slightly. This value defines what color the cube looks like under that weak, scattered light.
    vec3 diffuse;      // This is the "actual" color of your cube under direct light. If you have a red plastic block, the diffuse value would be red. It determines how the object looks where the light hits it directly.
    vec3 specular;     // This determines the color of the shiny highlight (the bright spot where light reflects directly into your eye). For plastics, this is usually white. For metals (like gold), the highlight is tinted the color of the metal.
    float shininess;   // This controls the size and sharpness of the specular highlight. A low number (like 2.0 or 4.0) makes a large, spread-out, blurry highlight (like matte rubber). A high number (like 32.0 or 64.0) makes a tiny, extremely sharp, bright dot (like polished plastic or glass).
};

struct Light {
    vec3 position;   // The position of the light source in world space
    vec3 ambient;    // The color and intensity of the baseline background light. This is usually kept very low (like a dark gray vec3(0.2, 0.2, 0.2)). If it's too high, your whole scene will look washed out and flat
    vec3 diffuse;    // The main color and brightness of the light rays. A standard white lightbulb would be vec3(1.0, 1.0, 1.0). If you changed this to pure green, your entire cube would be tinted green.
    vec3 specular;   // The intensity and color of the light that creates the shiny highlight. It's usually kept the same as the diffuse light, but you can tweak it to make the highlight punchier.
};

uniform Light light;
uniform Material material;
uniform vec3 view_pos;  // position of the camera in world space (used for specular lighting)

out vec4 frag_color;

in Varyings {
    vec3 normal;    // normal vector in world space
    vec3 frag_pos;   // fragment position in world space
} fs_in;

void main() {
    // ambient lighting
    vec3 ambient = light.ambient * material.ambient;

    // diffuse lighting
    vec3 norm = normalize(fs_in.normal);
    vec3 light_dir = normalize(light.position - fs_in.frag_pos);

    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light.diffuse * material.diffuse;

    // specular lighting
    vec3 view_dir = normalize(view_pos - fs_in.frag_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);   // Note that we negate the lightDir vector. The reflect function expects the first vector to point from the light source towards the fragment's position, but the lightDir vector is currently pointing the other way around

    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular = spec * light.specular * material.specular;

    frag_color = vec4(ambient + diffuse + specular, 1.0);
}

//If the angle between both vectors is greater than 90 degrees then the result of the dot product
// will actually become negative and we end up with a negative diffuse component. 
// For that reason we use the max function that returns the highest of both its parameters to
// make sure the diffuse component (and thus the colors) never become negative.