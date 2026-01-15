#include "batch_render.h"
#include "global_info.h"
#include "mesh.h"
#include "render.h"
#include <cglm/mat4.h>
#include <cglm/types.h>
#include <glad/glad.h>
#include <stdlib.h>
#include <string.h>

static const uint SHADER_BUFFER_LIGHT_BINDING = 0;
static const uint UNIFORM_VIEW_PROJ_BINDING = 1;
static const uint UNIFORM_MATERIAL_BINDING = 2;
static const uint SHADER_BUFFER_MODELS_BIND = 3;

void setUpBuffers(BatchRenderer *renderer) {
  // Initialize VAO
  glBindVertexArray(renderer->vao);

  // create VBO for accumulated vertex Information
  glGenBuffers(1, &(renderer->vertexVbo));

  glBindBuffer(GL_ARRAY_BUFFER, renderer->vertexVbo);

  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * renderer->maxVertexCount, NULL,
               GL_DYNAMIC_DRAW);

  // create shader attributes
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(0); // vertex position attr

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)sizeof(vec3s));

  glEnableVertexAttribArray(1); // textur uv coords attr

  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)sizeof(vec3s) + sizeof(vec2s));

  glEnableVertexAttribArray(3); // normal vectors attribute

  // create a new buffer for indexes into transformation matrices

  glGenBuffers(1, &(renderer->objectIndexVBO));
  glBindBuffer(GL_ARRAY_BUFFER, renderer->objectIndexVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * renderer->maxVertexCount, NULL,
               GL_DYNAMIC_DRAW);

  // create corresponding shader attribute

  glVertexAttribIPointer(2, 1, GL_INT, sizeof(int), (void *)0);

  glEnableVertexAttribArray(2);

  // unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void initBatchRenderer(BatchRenderer *renderer, TransformationMatrixManager* transformMatrixManager,
                       uint maxVertices) {
  renderer->maxVertexCount = maxVertices;
  renderer->numVertices = 0;
  renderer->transformManager = transformMatrixManager;
  renderer->vertexBuffer = malloc(sizeof(Vertex)*maxVertices);
  renderer->objectIndexBuffer = malloc(sizeof(uint)*maxVertices);
  glGenVertexArrays(1, &(renderer->vao));
  setUpBuffers(renderer);
}

void flush(BatchRenderer *renderer) {
  if (renderer->transformManager->numModels == 0 || renderer->numVertices == 0)
    return;

  glBindVertexArray(renderer->vao);

  // send data in vbo buffers
  glBindBuffer(GL_ARRAY_BUFFER, renderer->vertexVbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * renderer->numVertices,
                  renderer->vertexBuffer);

  glBindBuffer(GL_ARRAY_BUFFER, renderer->objectIndexVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(uint) * renderer->numVertices,
                  renderer->objectIndexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // flush data on transformation matrices
  flushTransformationManager(renderer->transformManager);

  // how to draw permanently
  glDrawArrays(GL_TRIANGLES, 0, renderer->numVertices);

  // unbind after render
  glBindVertexArray(0);

  renderer->numVertices = 0;

};

void renderObjectBatch(BatchRenderer *renderer, GameObject *object) {
  int newNumVertices = renderer->numVertices + object->mesh->numVertices;

  TransformationMatrixManager* manager = renderer->transformManager;
  int newNumObject = renderer->transformManager->numModels + 1;
  if (renderer->maxVertexCount <= newNumVertices ||
      manager->maxModels <= newNumObject) {
    flush(renderer);
    newNumVertices = object->mesh->numVertices;
  }

  // add current index to all vertices in index vbo
  for (int i = 0; i < object->mesh->numVertices; i++) {
    renderer->objectIndexBuffer[i + renderer->numVertices] =
        manager->numModels;
  }
  mat4 transform;
  getModelMatrix(object, transform);
  addTransformationToManager(manager, transform);

  // add vertex data to vertex vbo
  memcpy(renderer->vertexBuffer + renderer->numVertices, object->mesh->vertices,
         object->mesh->numVertices * sizeof(Vertex));

  // get transform matrix for object

  renderer->numVertices = newNumVertices;
  // maintain dictionary of shaders to buffer
  //
}

void freeBatchRenderer(BatchRenderer *renderer) {
  free(renderer->vertexBuffer);
  free(renderer->objectIndexBuffer);
}