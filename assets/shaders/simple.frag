#version 330 core

in vec4 out_frag_pos;
in vec3 out_normal;

layout(location=0) out vec4 out_color;
layout(location=1) out int out_entity;

uniform vec3 light_pos;
uniform vec3 object_color;
uniform int in_entity;

void main() {
    if (in_entity == 0) {
        float ambient_strength = 0.1;
        vec3 ambient = ambient_strength * vec3(1.0);

        vec3 norm = normalize(out_normal);
        vec3 light_dir = normalize(light_pos - out_frag_pos.xyz);  
    
        float diff = max(abs(dot(norm, light_dir)), 0.0);
        vec3 diffuse = diff * vec3(0.9);

        vec3 result = (ambient + diffuse) * object_color;

        out_color = vec4(result, 1.0);
    } else {
        out_color = vec4(object_color, 1.0);
    }
    out_entity = in_entity;
}