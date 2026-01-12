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
}  Vertex ;
// maybe put __attribute__((packed)) incase of changes

typedef struct{
    uint id;
    int width;
    int height;
    const char* name;
} Texture;

typedef struct {
    Vertex* vertices;
    int numVertices;
    uint* elementOrder;
    int numElements;
    int numTextures;
} Mesh;

void initMesh(Mesh *mesh, Vertex *vertices, int numVertices, uint elements[],int numElements) ;