#pragma once
#include <learnopengl/engine/shader.h>
#include <cglm/cglm.h>
#include <cglm/types-struct.h>
#include <cglm/types.h>
#include <cglm/vec2.h>
#include <sys/types.h>

typedef struct{
    vec3s pos;
    vec2s texPos;
    vec3s normal;
}  Vertex ;
// maybe put __attribute__((packed)) incase of changes
typedef struct{
    const char* name;
    uint glID;
    int width;
    int height;
} Texture;

typedef struct {
    uint id;
    int numElements;
    int numVertices;
    Vertex* vertices;
    int* elementOrder;
} Mesh;

void initMesh(Mesh *mesh, Vertex *vertices, int numVertices, int elements[],int numElements) ;

void freeMesh(Mesh* mesh);
