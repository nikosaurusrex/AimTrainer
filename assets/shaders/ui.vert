#version 330 core

layout (location = 0) in vec3 in_pos;
    
uniform mat4 proj_mat;
uniform mat4 view_mat;
uniform mat4 model_mat;

void main() {
    gl_Position = proj_mat * view_mat * model_mat * vec4(in_pos, 1.0);
}
