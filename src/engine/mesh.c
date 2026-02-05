#include <learnopengl/engine/mesh.h>
#include <cglm/affine-pre.h>
#include <cglm/cglm.h>
#include <cglm/struct/mat3.h>
#include <cglm/types-struct.h>
#include <glad/glad.h>
#include <stddef.h>
#include <cglm/struct.h>
#include <stdlib.h>
#include <string.h>

static uint NUM_MESHES = 0;

void initMesh(Mesh *mesh, Vertex *vertices, int numVertices, int elements[],
              int numElements) {
  mesh->vertices = malloc(sizeof(Vertex)* numVertices);
  memcpy(mesh->vertices, vertices, sizeof(Vertex)*numVertices);
  mesh->id = NUM_MESHES;
  NUM_MESHES++;
  mesh->numVertices = numVertices;
  mesh->elementOrder = malloc(sizeof(uint)*numElements);
  memcpy(mesh->elementOrder, elements, sizeof(uint)*numElements);
  mesh->numElements = numElements;
}

void freeMesh(Mesh* mesh){
  free(mesh->elementOrder);
  free(mesh->vertices);
}
