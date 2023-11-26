#ifndef SHADER_H
#define SHADER_H

#include "common.h"

#include <glm/glm.hpp>

struct shader_t {
    hashmap_t<string_t, GLint>  uniform_locations;
    GLuint                      id;
};

shader_t    *CreateShader(const char *vert_path, const char *frag_path);
void        DestroyShader(shader_t *shader);
GLuint      LoadShader(const char *source, GLenum type, const char *type_name);

void PrintShaderLog(GLuint shader);
void PrintProgramLog(GLuint program);

void    Use(shader_t *shader);
GLint   GetUniformLocation(shader_t *shader, string_t name);
void    LoadInt(shader_t *shader, string_t name, s32 value);
void    LoadVec3(shader_t *shader, string_t name, glm::vec3 vec);
void    LoadVec4(shader_t *shader, string_t name, glm::vec4 vec);
void    LoadMatrix(shader_t *shader, string_t name, glm::mat4 matrix);

#endif
