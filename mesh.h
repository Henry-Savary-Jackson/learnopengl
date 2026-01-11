#pragma once
#include "shader_utils.h"
#include <cglm/cglm.h>
#include <cglm/types-struct.h>
#include <cglm/types.h>
#include <cglm/vec2.h>
#include <sys/types.h>

typedef struct{
    vec3s pos;
    vec3s col;
    vec2s texPos;
}  Vertex ;
// maybe put __attribute__((packed)) incase of changes

typedef struct{
    uint id;
    int width;
    int height;
    const char* name;
} Texture;

typedef struct {
    uint vao;
    uint vbo;
    uint ebo;
    Vertex* vertices;
    int numVertices;
    int* elementOrder;
    int numElements;
    Texture* texture;
    int numTextures;
    ShaderProg* shaderProgram;
} Mesh;

void initMesh(Mesh* mesh, Vertex* vertices,int numVertices, int elements[], int numElements);

void setTexturesMesh(Mesh* mesh, Texture* texture , int numTextures);
void setShaderProgramMesh(Mesh *mesh, ShaderProg *shaderProgram) ;

void setBuffersMesh(Mesh* mesh);

int getEncodedVertexDataLength(Mesh* mesh );

void encodeVertexData(Mesh* mesh, float* data);

void freeMesh(Mesh* mesh);

void drawMesh(Mesh *mesh) ;
void useShader(Mesh *mesh) ;