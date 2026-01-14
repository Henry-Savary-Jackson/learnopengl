#pragma once
#include "shader.h"
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
    uint id;
    int width;
    int height;
} Texture;

typedef struct {
    int numElements;
    int numVertices;
    Vertex* vertices;
    uint* elementOrder;
    int numTextures;
} Mesh;

void initMesh(Mesh *mesh, Vertex *vertices, int numVertices, uint elements[],int numElements) ;