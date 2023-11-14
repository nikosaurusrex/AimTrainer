#ifndef GFX_H
#define GFX_H

#include "common.h"

typedef u32 CubeMap;

CubeMap CreateCubeMap(const char *faces[6]);
void DestroyCubeMap(CubeMap cube_map);
void Bind(CubeMap cube_map);

struct SimpleMesh {
    u32 vao;
    u32 vbo;

    s32 vertices_count;
};

SimpleMesh *CreateSimpleMesh(f32 *vertices, s32 num_vertices);
void DestroySimpleMesh(SimpleMesh *mesh);
void Bind(SimpleMesh *mesh);
void Draw(SimpleMesh *mesh);

struct Mesh {
    GLuint vao; 
    GLuint vbos[4];
    u32 indices_count;
};

Mesh *CreateMesh(
    f32 *vertices, u32 vertices_count,
    f32 *tex_coords, u32 tex_coords_count,
    f32 *normals, u32 normals_count,
    u32 *indices, u32 indices_count
);
void DestroyMesh(Mesh *mesh);

void Bind(Mesh *mesh);
void Draw(Mesh *mesh);

typedef GLuint Texture;

Texture LoadTexture(String file_path, GLint format);
void DestroyTexture(Texture texture);
void BindTexture(Texture texture, s32 slot);

struct PositionalLight {
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    glm::vec3 pos;
};

struct Shader;
void Install(PositionalLight light, Shader *shader);

Mesh *LoadObjFile(const char *file);

#endif
