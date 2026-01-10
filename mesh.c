#include "mesh.h"
#include "textures.h"
#include <GL/glew.h>
#include <cglm/types.h>
#include <stddef.h>
#include <string.h>

void initMesh(Mesh *mesh, Vertex *vertices, int numVertices, int elements[],
              int numElements) {
  glGenVertexArrays(1, &(mesh->vao));
  mesh->vertices = vertices;
  mesh->numVertices = numVertices;
  mesh->elementOrder = elements;
  mesh->numElements = numElements;
}

void addTextureToMesh(Mesh *mesh, Texture *texture) { mesh->texture = texture; }

void setBuffersMesh(Mesh *mesh) {

  glBindVertexArray(mesh->vao);

  glGenVertexArrays(1, &(mesh->ebo));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * mesh->numElements,
               mesh->elementOrder, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glGenVertexArrays(1, &(mesh->vbo));

  glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);

  float *data = (float *)(mesh->vertices);
  int dataLength = getEncodedVertexDataLength(mesh);
  glBufferData(GL_ARRAY_BUFFER, dataLength, data, GL_STATIC_DRAW);

  // TODO should be more resilient to changes in structure of Vertex

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(0); // vertex pos

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)sizeof(f_vec3));
  glEnableVertexAttribArray(1); // color info

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)(sizeof(f_vec3) * 2));
  glEnableVertexAttribArray(2); // texture coords

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void drawMesh(Mesh *mesh) {
  glBindTexture(GL_TEXTURE_2D, mesh->texture->id);
  glBindVertexArray(mesh->vao);
  glDrawElements(GL_TRIANGLES, mesh->numElements, GL_UNSIGNED_INT, 0);
}

int getEncodedVertexDataLength(Mesh *mesh) {
  return sizeof(Vertex) * mesh->numVertices;
}

void encodeVertexData(Mesh *mesh, float *data) {
  for (int i = 0; i < mesh->numVertices; i++) {
    memcpy(data + (i * sizeof(Vertex)), mesh->vertices + i, sizeof(Vertex));
  }
}

void freeMesh(Mesh *mesh) {
  glDeleteVertexArrays(1, &(mesh->vao));
  glDeleteBuffers(1, &(mesh->vbo));
  glDeleteBuffers(1, &(mesh->ebo));
  if (mesh->texture)
    freeTexture(mesh->texture);
}
