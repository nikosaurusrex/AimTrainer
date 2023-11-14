#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex;
layout (location = 2) in vec3 in_normal;

out vec4 out_frag_pos;
out vec3 out_normal;
    
uniform mat4 proj_mat;
uniform mat4 view_mat;
uniform mat4 model_mat;

void main() {
    out_frag_pos = model_mat * vec4(in_pos, 1.0);
    out_normal = abs(in_normal);

    gl_Position = proj_mat * view_mat * out_frag_pos;
}
