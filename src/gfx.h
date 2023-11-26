#ifndef GFX_H
#define GFX_H

#include "common.h"

typedef u32 cubemap_t;

cubemap_t   CreateCubeMap(const char *faces[6]);
void        DestroyCubeMap(cubemap_t cube_map);
void        Bind(cubemap_t cube_map);

struct simple_mesh_t {
    u32 vao;
    u32 vbo;
    s32 vertices_count;
};

simple_mesh_t   *CreateSimpleMesh(f32 *vertices, s32 num_vertices);
void            DestroySimpleMesh(simple_mesh_t *mesh);
void            Bind(simple_mesh_t *mesh);
void            Draw(simple_mesh_t *mesh);

struct mesh_t {
    GLuint  vao; 
    GLuint  vbos[4];
    u32     indices_count;
};

mesh_t  *CreateMesh(
    f32 *vertices,      u32 vertices_count,
    f32 *tex_coords,    u32 tex_coords_count,
    f32 *normals,       u32 normals_count,
    u32 *indices,       u32 indices_count
);
void    DestroyMesh(mesh_t *mesh);
void    Bind(mesh_t *mesh);
void    Draw(mesh_t *mesh);

typedef GLuint texture_t;

texture_t   LoadTexture(string_t file_path, GLint format);
void        DestroyTexture(mesh_t texture);
void        BindTexture(mesh_t texture, s32 slot);

struct positional_light_t {
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    glm::vec3 pos;
};

struct shader_t;
void Install(positional_light_t light, shader_t *shader);

mesh_t *LoadObjFile(const char *file);

#endif
