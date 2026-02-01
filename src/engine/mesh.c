#include <learnopengl/engine/mesh.h>
#include <cglm/affine-pre.h>
#include <cglm/cglm.h>
#include <cglm/struct/mat3.h>
#include <cglm/types-struct.h>
#include <glad/glad.h>
#include <stddef.h>
#include <cglm/struct.h>

static uint NUM_MESHES = 0;

void initMesh(Mesh *mesh, Vertex *vertices, int numVertices, int elements[],
              int numElements) {
  mesh->vertices = vertices;
  mesh->id = NUM_MESHES;
  NUM_MESHES++;
  mesh->numVertices = numVertices;
  mesh->elementOrder = elements;
  mesh->numElements = numElements;
}
