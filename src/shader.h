#ifndef SHADER_H
#define SHADER_H

#include "common.h"

#include <glm/glm.hpp>

struct Shader {
    HashMap<String, GLint> uniform_locations;
    GLuint id;
};

Shader *CreateShader(const char *vert_path, const char *frag_path);
void DestroyShader(Shader *shader);
GLuint LoadShader(const char *source, GLenum type, const char *type_name);

void PrintShaderLog(GLuint shader);
void PrintProgramLog(GLuint program);

void Use(Shader *shader);
GLint GetUniformLocation(Shader *shader, String name);
void LoadInt(Shader *shader, String name, s32 value);
void LoadVec3(Shader *shader, String name, glm::vec3 vec);
void LoadVec4(Shader *shader, String name, glm::vec4 vec);
void LoadMatrix(Shader *shader, String name, glm::mat4 matrix);

#endif
