#include "mesh.h"
#include "shader_utils.h"
#include "textures.h"
#include <cglm/types.h>
#include <glad/glad.h>
#include <stddef.h>
#include <string.h>

void initMesh(Mesh *mesh, Vertex *vertices, int numVertices, int elements[],
              int numElements) {
uint vao  ;
  glGenVertexArrays(1,&vao );
  mesh->vao = vao;
  mesh->vertices = vertices;
  mesh->numVertices = numVertices;
  mesh->elementOrder = elements;
  mesh->numElements = numElements;
  mesh->texture = NULL;
  mesh->shaderProgram = NULL;
}

void addTextureToMesh(Mesh *mesh, Texture *texture) { mesh->texture = texture; }
void setShaderProgram(Mesh *mesh, ShaderProg *shaderProgram) { mesh->shaderProgram = shaderProgram; }

void setBuffersMesh(Mesh *mesh) {
  // bind
  glBindVertexArray(mesh->vao);


  glGenBuffers(1, &(mesh->ebo));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * mesh->numElements,
               mesh->elementOrder, GL_STATIC_DRAW);

  GLenum result2 = glGetError();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glGenBuffers(1, &(mesh->vbo));

  glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);

  float *data = (float *)(mesh->vertices);
  int dataLength = getEncodedVertexDataLength(mesh);
  glBufferData(GL_ARRAY_BUFFER, dataLength, data, GL_STATIC_DRAW);

  // TODO should be more resilient to changes in structure of Vertex

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  GLenum result1 = glGetError();
  glEnableVertexAttribArray(0); // vertex pos

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)sizeof(f_vec3));
  GLenum result = glGetError();
  glEnableVertexAttribArray(1); // color info

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)(sizeof(f_vec3) * 2));
  glEnableVertexAttribArray(2); // texture coords

  // unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void drawMesh(Mesh *mesh) {
  if (mesh->texture){
    glBindTexture(GL_TEXTURE_2D, mesh->texture->id);
  }
  if (mesh->shaderProgram){
    useShaderProgram(mesh->shaderProgram);
  }

  glBindVertexArray(mesh->vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
  glDrawElements(GL_TRIANGLES, mesh->numElements, GL_UNSIGNED_INT, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
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
