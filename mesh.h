#pragma once
#include <cglm/cglm.h>
#include <cglm/types.h>
#include <cglm/vec2.h>
#include <sys/types.h>

typedef struct{
    float x;
    float y;
    float z;
} f_vec3 ;

typedef  struct{
    float x;
    float y;
} f_vec2 ;

typedef struct{
    f_vec3 pos;
    f_vec3 col;
    f_vec2 texPos;
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
} Mesh;

void initMesh(Mesh* mesh, Vertex* vertices,int numVertices, int elements[], int numElements);

void addTextureToMesh(Mesh* mesh, Texture* texture);

void setBuffersMesh(Mesh* mesh);

int getEncodedVertexDataLength(Mesh* mesh );

void encodeVertexData(Mesh* mesh, float* data);

void freeMesh(Mesh* mesh);

void drawMesh(Mesh *mesh) ;