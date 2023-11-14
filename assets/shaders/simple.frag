#version 330 core

in vec4 out_frag_pos;
in vec3 out_normal;

out vec4 out_color;

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirectionalLight light;
uniform vec3 view_pos;
uniform vec3 object_color;

void main() {
    /*
    vec3 ambient = light.ambient * object_color;

    vec3 norm = normalize(out_normal);
    vec3 light_dir = normalize(-light.direction);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = light.diffuse * (diff * object_color);

    float specular_strength = 0.5;
    vec3 view_dir = normalize(view_pos - out_frag_pos.xyz);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = light.specular * (specular_strength * spec);

    vec3 result = ambient + diffuse + specular;*/
    out_color = vec4(object_color, 1.0);
}