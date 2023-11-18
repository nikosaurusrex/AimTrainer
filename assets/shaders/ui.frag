#version 330 core

uniform vec3 in_color = vec3(1.0);

out vec4 out_color;

void main() {
    out_color = vec4(in_color, 1.0);
}