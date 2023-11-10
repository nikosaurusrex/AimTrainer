#include "shader.h"

#include <glm/gtc/type_ptr.hpp>

bool CheckOpenGLError() {
    bool found = false;
    s32 err = glGetError();
    while (err != GL_NO_ERROR) {
        LogError("glError: %d", err);
        found = true;
        err = glGetError();
    }
    return found;
}

Shader *CreateShader(const char *vert_path, const char *frag_path) {
    Shader *shader = (Shader *) malloc(sizeof(Shader));

    char *vert_source = ReadEntireFile(vert_path);
    char *frag_source = ReadEntireFile(frag_path);

    if (!vert_source) {
        LogFatal("Failed to read vertex shader file '%s'", vert_path);
    }

    if (!frag_source) {
        LogFatal("Failed to read fragment shader file '%s'", frag_path);
    }

    GLuint vert_shader = LoadShader(vert_source, GL_VERTEX_SHADER, "Vertex Shader");
    GLuint frag_shader = LoadShader(frag_source, GL_FRAGMENT_SHADER, "Fragment Shader");

    shader->id = glCreateProgram();

    glAttachShader(shader->id, vert_shader);
    glAttachShader(shader->id, frag_shader);

    glLinkProgram(shader->id);
    CheckOpenGLError();

    GLint linked;
    glGetProgramiv(shader->id, GL_LINK_STATUS, &linked);
    if (linked != 1) {
        PrintProgramLog(shader->id);
        LogFatal("Linking error");
    }

	glDetachShader(shader->id, vert_shader);
	glDetachShader(shader->id, frag_shader);
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    delete[] vert_source;
    delete[] frag_source;

    return shader;
}

void DestroyShader(Shader *shader) {
    glDeleteProgram(shader->id);

    free(shader);
}

GLuint LoadShader(const char *source, GLenum type, const char *type_name) {
    GLint compiled;

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, 0);

    glCompileShader(shader);
    CheckOpenGLError();
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled != 1) {
        LogInfo("%s Compilation failed", type_name);
        PrintShaderLog(shader);
    }

    return shader;
}

void PrintShaderLog(GLuint shader) {
    s32 length = 0, chars_written = 0;
    char *log;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

    if (length <= 0) {
        return;
    }

    log = new char[length];
    glGetShaderInfoLog(shader, length, &chars_written, log);
    LogFatal("Shader Info Log: %s", log);
    delete[] log;
}

void PrintProgramLog(GLuint program) {
    s32 length = 0, chars_written = 0;
    char *log;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

    if (length <= 0) {
        return;
    }

    log = new char[length];
    glGetProgramInfoLog(program, length, &chars_written, log);
    LogFatal("Program Info Log: %s", log);
    delete[] log;
}

void Use(Shader *shader) {
    glUseProgram(shader->id);
}

GLint GetUniformLocation(Shader *shader, String name) {
    /* @Todo: use cache */
    return glGetUniformLocation(shader->id, name.data);
}

void LoadInt(Shader *shader, String name, s32 value) {
    GLint location = GetUniformLocation(shader, name);
    glUniform1i(location, value);
}

void LoadVec3(Shader *shader, String name, glm::vec3 vec) {
    GLint location = GetUniformLocation(shader, name);
    glUniform3f(location, vec.x, vec.y, vec.z);
}

void LoadVec4(Shader *shader, String name, glm::vec4 vec) {
    GLint location = GetUniformLocation(shader, name);
    glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
}

void LoadMatrix(Shader *shader, String name, glm::mat4 matrix) {
    GLint location = GetUniformLocation(shader, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}