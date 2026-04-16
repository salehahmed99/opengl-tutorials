#version 330

out vec4 frag_color;

uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_pos; // position of the light source in world space
uniform vec3 view_pos;  // position of the camera in world space (used for specular lighting)

in Varyings {
    vec3 normal;    // normal vector in world space
    vec3 frag_pos;   // fragment position in world space
} fs_in;

void main() {
    // ambient lighting
    float ambient_strength = 0.1;
    vec3 ambient = ambient_strength * light_color;

    // diffuse lighting
    vec3 norm = normalize(fs_in.normal);
    vec3 light_dir = normalize(light_pos - fs_in.frag_pos);

    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    // specular lighting
    vec3 view_dir = normalize(view_pos - fs_in.frag_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);   // Note that we negate the lightDir vector. The reflect function expects the first vector to point from the light source towards the fragment's position, but the lightDir vector is currently pointing the other way around

    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    float specular_strength = 0.5;

    vec3 specular = specular_strength * spec * light_color;

    frag_color = vec4((ambient + diffuse + specular) * object_color, 1.0);
}

//If the angle between both vectors is greater than 90 degrees then the result of the dot product
// will actually become negative and we end up with a negative diffuse component. 
// For that reason we use the max function that returns the highest of both its parameters to
// make sure the diffuse component (and thus the colors) never become negative.