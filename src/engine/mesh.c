#include <learnopengl/engine/mesh.h>
#include <cglm/affine-pre.h>
#include <cglm/cglm.h>
#include <cglm/struct/mat3.h>
#include <cglm/types-struct.h>
#include <glad/glad.h>
#include <stddef.h>
#include <cglm/struct.h>

void initMesh(Mesh *mesh, Vertex *vertices, int numVertices, uint elements[],
              int numElements) {
  mesh->vertices = vertices;
  mesh->numVertices = numVertices;
  mesh->elementOrder = elements;
  mesh->numElements = numElements;
}
