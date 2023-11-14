#include "gfx.h"

#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

CubeMap CreateCubeMap(const char *faces[6]) {
    u32 id;

    glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	s32 width, height, nrChannels;
	u8 *data;  

    stbi_set_flip_vertically_on_load(false);
	for(u32 i = 0; i < 6; i++) {
		data = stbi_load(faces[i], &width, &height, &nrChannels, STBI_rgb);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
			0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
		);
    	
    	stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 

    return id;
}

void DestroyCubeMap(CubeMap cube_map) {
    glDeleteTextures(1, &cube_map);
}

void Bind(CubeMap cube_map) {
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map);
}

SimpleMesh *CreateSimpleMesh(f32 *vertices, s32 num_vertices) {
    SimpleMesh *mesh = (SimpleMesh *) malloc(sizeof(SimpleMesh));

    glGenVertexArrays(1, &mesh->vao);
    glBindVertexArray(mesh->vao);

    mesh->vertices_count = num_vertices / 3;

    glGenBuffers(1, &mesh->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * num_vertices, &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);

    return mesh;
}

void DestroySimpleMesh(SimpleMesh *mesh) {
    free(mesh);
}

void Bind(SimpleMesh *mesh) {
    glBindVertexArray(mesh->vao);
}

void Draw(SimpleMesh *mesh) {
    glDrawArrays(GL_TRIANGLES, 0, mesh->vertices_count);
}

Mesh *CreateMesh(
    f32 *vertices, u32 vertices_count,
    f32 *tex_coords, u32 tex_coords_count,
    f32 *normals, u32 normals_count,
    u32 *indices, u32 indices_count
) {
    Mesh *mesh = (Mesh *) malloc(sizeof(Mesh));
    mesh->indices_count = indices_count;

    glGenVertexArrays(1, &mesh->vao);
    glBindVertexArray(mesh->vao);

    glGenBuffers(4, mesh->vbos);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices_count * sizeof(f32), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, tex_coords_count * sizeof(f32), tex_coords, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, 0);
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbos[2]);
    glBufferData(GL_ARRAY_BUFFER, normals_count * sizeof(f32), normals, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, 0, 0, 0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->vbos[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * indices_count, indices, GL_STATIC_DRAW);

    return mesh;
}

void DestroyMesh(Mesh *mesh) {
    glDeleteVertexArrays(1, &mesh->vao);
    glDeleteBuffers(4, mesh->vbos);

    free(mesh);
}

void Bind(Mesh *mesh) {
    glBindVertexArray(mesh->vao);
}

void Draw(Mesh *mesh) {
    glDrawElements(GL_TRIANGLES, mesh->indices_count, GL_UNSIGNED_INT, 0);
}

Texture LoadTexture(String file_path, GLint format) {
    Texture texture;

    s32 w, h;

    stbi_set_flip_vertically_on_load(true);
    s32 read_mode = format == GL_RGBA ? STBI_rgb_alpha : STBI_rgb;
    u8 *image = stbi_load(file_path.c_str(), &w, &h, 0, read_mode);

    if (!image) {
        LogFatal("Failed to load texture '%s'", file_path.c_str());
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(image);

    return texture;
}

void DestroyTexture(Texture texture) {
    glDeleteTextures(1, &texture);
}

void BindTexture(Texture texture, s32 slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void Install(PositionalLight light, Shader *shader) {
    Use(shader);
    LoadVec4(shader, "light.ambient", light.ambient);
    LoadVec4(shader, "light.diffuse", light.diffuse);
    LoadVec4(shader, "light.specular", light.specular);
    LoadVec3(shader, "light.pos", light.pos);
}

Mesh *LoadObjFile(const char *file) {
    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (scene->mNumMeshes != 1) {
        LogFatal("Illegal number of meshes in object file!\n");
    }

    aiMesh *mesh = scene->mMeshes[0];

    s32 num_vertices = mesh->mNumVertices * 3;
    f32 *vertices = new f32[num_vertices];

    s32 num_tex_coords = mesh->mNumVertices * 2;
    f32 *tex_coords = new f32[num_tex_coords];

    s32 num_normals = mesh->mNumVertices * 3;
    f32 *normals = new f32[num_normals];

    s32 num_indices = mesh->mNumFaces * 3;
    u32 *indices = new u32[num_indices];

    aiVector3D zero_vector(0.0f);
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        aiVector3D pos = mesh->mVertices[i];
        aiVector3D tex_coords = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : zero_vector;
        aiVector3D normal = mesh->mNormals[i];

        vertices[i * 3] = pos.x;
        vertices[i * 3 + 1] = pos.y;
        vertices[i * 3 + 2] = pos.z;

        tex_coords[i * 2] = tex_coords.x;
        tex_coords[i * 2 + 1] = tex_coords.y;

        normals[i * 3] = normal.x;
        normals[i * 3 + 1] = normal.y;
        normals[i * 3 + 2] = normal.z;
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        indices[i * 3] = face.mIndices[0];
        indices[i * 3 + 1] = face.mIndices[1];
        indices[i * 3 + 2] = face.mIndices[2];
    }

    return CreateMesh(vertices, num_vertices, tex_coords, num_tex_coords, normals, num_normals, indices, num_indices);
}
